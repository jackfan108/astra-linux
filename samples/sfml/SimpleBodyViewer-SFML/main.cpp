// This file is part of the Orbbec Astra SDK [https://orbbec3d.com]
// Copyright (c) 2015-2017 Orbbec 3D
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Be excellent to each other.
#include <SFML/Graphics.hpp>
#include <astra/astra.hpp>
#include <iostream>
#include <cstring>
#include <fstream>
using namespace std;

#include <cstdlib>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

class sfLine : public sf::Drawable
{
public:
    sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color color, float thickness)
        : color_(color)
    {
        const sf::Vector2f direction = point2 - point1;
        const sf::Vector2f unitDirection = direction / sqrt(direction.x*direction.x + direction.y*direction.y);
        const sf::Vector2f normal(-unitDirection.y, unitDirection.x);

        const sf::Vector2f offset = (thickness / 2.f) * normal;

        vertices_[0].position = point1 + offset;
        vertices_[1].position = point2 + offset;
        vertices_[2].position = point2 - offset;
        vertices_[3].position = point1 - offset;

        for (int i = 0; i<4; ++i)
            vertices_[i].color = color;
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(vertices_, 4, sf::Quads, states);
    }

private:
    sf::Vertex vertices_[4];
    sf::Color color_;
};

class BodyVisualizer : public astra::FrameListener
{
public:
    enum class WorkoutMode {
        BicepCurl,
        ShoulderPress,
        Squat,
    };

    enum class WorkoutState {
        NoOp = 0,
        BicepUp = 1,
        BicepDown = 2,
        ShoulderPressUp = 3,
        ShoulderPressDown = 4,
        SquatDown = 5,
        SquatUp = 6,
    };


    static sf::Color get_body_color(uint8_t bodyId)
    {
        if (bodyId == 0)
        {
            // Handle no body separately - transparent
            return sf::Color(0x00, 0x00, 0x00, 0x00);
        }
        // Case 0 below could mean bodyId == 25 or
        // above due to the "% 24".
        switch (bodyId % 24) {
        case 0:
            return sf::Color(0x00, 0x88, 0x00, 0xFF);
        case 1:
            return sf::Color(0x00, 0x00, 0xFF, 0xFF);
        case 2:
            return sf::Color(0x88, 0x00, 0x00, 0xFF);
        case 3:
            return sf::Color(0x00, 0xFF, 0x00, 0xFF);
        case 4:
            return sf::Color(0x00, 0x00, 0x88, 0xFF);
        case 5:
            return sf::Color(0xFF, 0x00, 0x00, 0xFF);

        case 6:
            return sf::Color(0xFF, 0x88, 0x00, 0xFF);
        case 7:
            return sf::Color(0xFF, 0x00, 0xFF, 0xFF);
        case 8:
            return sf::Color(0x88, 0x00, 0xFF, 0xFF);
        case 9:
            return sf::Color(0x00, 0xFF, 0xFF, 0xFF);
        case 10:
            return sf::Color(0x00, 0xFF, 0x88, 0xFF);
        case 11:
            return sf::Color(0xFF, 0xFF, 0x00, 0xFF);

        case 12:
            return sf::Color(0x00, 0x88, 0x88, 0xFF);
        case 13:
            return sf::Color(0x00, 0x88, 0xFF, 0xFF);
        case 14:
            return sf::Color(0x88, 0x88, 0x00, 0xFF);
        case 15:
            return sf::Color(0x88, 0xFF, 0x00, 0xFF);
        case 16:
            return sf::Color(0x88, 0x00, 0x88, 0xFF);
        case 17:
            return sf::Color(0xFF, 0x00, 0x88, 0xFF);

        case 18:
            return sf::Color(0xFF, 0x88, 0x88, 0xFF);
        case 19:
            return sf::Color(0xFF, 0x88, 0xFF, 0xFF);
        case 20:
            return sf::Color(0x88, 0x88, 0xFF, 0xFF);
        case 21:
            return sf::Color(0x88, 0xFF, 0xFF, 0xFF);
        case 22:
            return sf::Color(0x88, 0xFF, 0x88, 0xFF);
        case 23:
            return sf::Color(0xFF, 0xFF, 0x88, 0xFF);
        default:
            return sf::Color(0xAA, 0xAA, 0xAA, 0xFF);
        }
    }

    void init_depth_texture(int width, int height)
    {
        if (displayBuffer_ == nullptr || width != depthWidth_ || height != depthHeight_)
        {
            depthWidth_ = width;
            depthHeight_ = height;
            int byteLength = depthWidth_ * depthHeight_ * 4;

            displayBuffer_ = BufferPtr(new uint8_t[byteLength]);
            memset(displayBuffer_.get(), 0, byteLength);

            texture_.create(depthWidth_, depthHeight_);
            sprite_.setTexture(texture_, true);
            sprite_.setPosition(0, 0);
        }
    }

    void init_overlay_texture(int width, int height)
    {
        if (overlayBuffer_ == nullptr || width != overlayWidth_ || height != overlayHeight_)
        {
            overlayWidth_ = width;
            overlayHeight_ = height;
            int byteLength = overlayWidth_ * overlayHeight_ * 4;

            overlayBuffer_ = BufferPtr(new uint8_t[byteLength]);
            fill(&overlayBuffer_[0], &overlayBuffer_[0] + byteLength, 0);

            overlayTexture_.create(overlayWidth_, overlayHeight_);
            overlaySprite_.setTexture(overlayTexture_, true);
            overlaySprite_.setPosition(0, 0);
        }
    }

    void check_fps()
    {
        double fpsFactor = 0.02;

        clock_t newTimepoint= clock();
        long double frameDuration = (newTimepoint - lastTimepoint_) / static_cast<long double>(CLOCKS_PER_SEC);

        frameDuration_ = frameDuration * fpsFactor + frameDuration_ * (1 - fpsFactor);
        lastTimepoint_ = newTimepoint;
        double fps = 1.0 / frameDuration_;

        printf("FPS: %3.1f (%3.4Lf ms)\n", fps, frameDuration_ * 1000);
    }

    void processDepth(astra::Frame& frame)
    {
        const astra::DepthFrame depthFrame = frame.get<astra::DepthFrame>();

        int width = depthFrame.width();
        int height = depthFrame.height();

        init_depth_texture(width, height);

        const int16_t* depthPtr = depthFrame.data();
        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                int index = (x + y * width);
                int index4 = index * 4;

                int16_t depth = depthPtr[index];
                uint8_t value = depth % 255;

                displayBuffer_[index4] = value;
                displayBuffer_[index4 + 1] = value;
                displayBuffer_[index4 + 2] = value;
                displayBuffer_[index4 + 3] = 255;
            }
        }

        texture_.update(displayBuffer_.get());
    }

    void toggleDataCollection() {
        if (collecting) {
            cout << "data collection stopped" << endl;
            data_count++;
        } else {
            cout << "data collection started :D" << endl;
        }
        collecting = !collecting;
    }

    void setWorkoutMode(WorkoutMode wm) {
        workout_mode = wm;
        if (wm == WorkoutMode::BicepCurl) {
            cout << "now recording bicep curl" << endl;
        } else if (wm == WorkoutMode::ShoulderPress) {
            cout << "now recording shoulder press" << endl;
        } else if (wm == WorkoutMode::Squat) {
            cout << "now recording squat" << endl;
        } else {
            cout << "ugh oh... idk what's going on" << endl;
        }
    }

    void nextState() {
        switch( workout_state ) {
            case WorkoutState::NoOp:
                switch( workout_mode ) {
                    case WorkoutMode::BicepCurl:
                        setWorkoutState(WorkoutState::BicepUp);
                        cout << "state set to bicep up" << endl;
                        break;
                    case WorkoutMode::ShoulderPress:
                        setWorkoutState(WorkoutState::
                            ShoulderPressUp);
                        cout << "state set to shoulder press up" << endl;
                        break;
                    case WorkoutMode::Squat:
                        setWorkoutState(WorkoutState::SquatDown);
                        cout << "state set to squat down" << endl;
                        break;
                }
                break;
            case WorkoutState::BicepUp:
                setWorkoutState(WorkoutState::BicepDown);
                cout << "state set to bicep down" << endl;
                break;
            case WorkoutState::BicepDown:
                setWorkoutState(WorkoutState::NoOp);
                cout << "state set to no-op" << endl;
                break;
            case WorkoutState::ShoulderPressUp:
                setWorkoutState(WorkoutState::ShoulderPressDown);
                cout << "state set to shoulder press down" << endl;
                break;
            case WorkoutState::ShoulderPressDown:
                setWorkoutState(WorkoutState::NoOp);
                cout << "state set to no-op" << endl;
                break;
            case WorkoutState::SquatDown:
                setWorkoutState(WorkoutState::SquatUp);
                cout << "state set to squat up" << endl;
                break;
            case WorkoutState::SquatUp:
                setWorkoutState(WorkoutState::NoOp);
                cout << "state set to no-op" << endl;
                break;
        }

    }

    void setWorkoutState(WorkoutState ws) {
        workout_state = ws;
    }

    void processBodies(astra::Frame& frame)
    {
        const float jointScale = depthWidth_ / 120.f;
        astra::BodyFrame bodyFrame = frame.get<astra::BodyFrame>();

        ofstream file; // open this file per frame to write data

        jointPositions_.clear();
        circles_.clear();
        circleShadows_.clear();
        boneLines_.clear();
        boneShadows_.clear();

        if (!bodyFrame.is_valid())
        {
            clear_overlay();
            return;
        }

        const auto& bodies = bodyFrame.bodies();

        string file_name = "fake_data_";
        file_name.append(to_string(data_count));
        file_name.append(".csv");
        file.open(file_name, ios_base::app);
        string data = "";

        for (auto& body : bodies)
        {
            // printf("Processing frame #%d body %d left hand: %u\n",
            //     bodyFrame.frame_index(), body.id(), unsigned(body.hand_poses().left_hand()));
            for(auto& joint : body.joints())
            {
                if (collecting) {
                    // char* joint_data[1000] = {};
                    string x = to_string(joint.world_position().x);
                    string y = to_string(joint.world_position().y);
                    string z = to_string(joint.world_position().z);
                    data.append(x + ", " + y + ", " + z + ", ");
                }


                jointPositions_.push_back(joint.depth_position());
            }

            if (collecting && data != "") {
                data = to_string(static_cast<int>(workout_state)) + ", " + data;
                file << data.substr(0, data.size() - 2) << endl;
                frame_count++;
            }


            file.close();

            if (collecting && frame_count > 300) {
                frame_count = 1;
                data_count ++;
                collecting = false;
                setWorkoutState(WorkoutState::NoOp);
                cout << "- done writing data into current file, please press enter to write in new file -" << endl;
            }

            update_body(body, jointScale);
        }

        const auto& floor = bodyFrame.floor_info(); //floor
        // if (floor.floor_detected())
        // {
        //     const auto& p = floor.floor_plane();
        //     cout << "Floor plane: ["
        //         << p.a() << ", " << p.b() << ", " << p.c() << ", " << p.d()
        //         << "]" << endl;

        // }

        const auto& bodyMask = bodyFrame.body_mask();
        const auto& floorMask = floor.floor_mask();

        update_overlay(bodyMask, floorMask);
    }

    void update_body(astra::Body body,
                     const float jointScale)
    {
        const auto& joints = body.joints();

        if (joints.empty())
        {
            return;
        }

        vector<float> joint_cords;

        for (const auto& joint : joints)
        {
            // push joint xyz into joint_cords
            // this is for sending frame data to the python server
            joint_cords.push_back(joint.world_position().x);
            joint_cords.push_back(joint.world_position().y);
            joint_cords.push_back(joint.world_position().z);

            astra::JointType type = joint.type();
            const auto& pos = joint.depth_position();

            if (joint.status() == astra::JointStatus::NotTracked)
            {
                continue;
            }

            auto radius = jointRadius_ * jointScale; // pixels
            sf::Color circleShadowColor(0, 0, 0, 255);

             auto color = sf::Color(0x00, 0xFF, 0x00, 0xFF);

            if ((type == astra::JointType::LeftHand && astra::HandPose::Grip==body.hand_poses().left_hand()) ||
                (type == astra::JointType::RightHand &&  astra::HandPose::Grip==body.hand_poses().right_hand()))
            {
                radius *= 1.5f;
                circleShadowColor = sf::Color(255, 255, 255, 255);
                color = sf::Color(0x00, 0xAA, 0xFF, 0xFF);
            }

            const auto shadowRadius = radius + shadowRadius_ * jointScale;
            const auto radiusDelta = shadowRadius - radius;

            sf::CircleShape circle(radius);

            circle.setFillColor(sf::Color(color.r, color.g, color.b, 255));
            circle.setPosition(pos.x - radius, pos.y - radius);
            circles_.push_back(circle);

            sf::CircleShape shadow(shadowRadius);
            shadow.setFillColor(circleShadowColor);
            shadow.setPosition(circle.getPosition() - sf::Vector2f(radiusDelta, radiusDelta));
            circleShadows_.push_back(shadow);
        }

        // cout << "okay test connection with real body data :D" << endl;
        // test_connection(joint_cords);

        update_bone(joints, jointScale, astra::JointType::Head, astra::JointType::ShoulderSpine);

        update_bone(joints, jointScale, astra::JointType::ShoulderSpine, astra::JointType::LeftShoulder);
        update_bone(joints, jointScale, astra::JointType::LeftShoulder, astra::JointType::LeftElbow);
        update_bone(joints, jointScale, astra::JointType::LeftElbow, astra::JointType::LeftHand);

        update_bone(joints, jointScale, astra::JointType::ShoulderSpine, astra::JointType::RightShoulder);
        update_bone(joints, jointScale, astra::JointType::RightShoulder, astra::JointType::RightElbow);
        update_bone(joints, jointScale, astra::JointType::RightElbow, astra::JointType::RightHand);

        update_bone(joints, jointScale, astra::JointType::ShoulderSpine, astra::JointType::MidSpine);
        update_bone(joints, jointScale, astra::JointType::MidSpine, astra::JointType::BaseSpine);

        update_bone(joints, jointScale, astra::JointType::BaseSpine, astra::JointType::LeftHip);
        update_bone(joints, jointScale, astra::JointType::LeftHip, astra::JointType::LeftKnee);
        update_bone(joints, jointScale, astra::JointType::LeftKnee, astra::JointType::LeftFoot);

        update_bone(joints, jointScale, astra::JointType::BaseSpine, astra::JointType::RightHip);
        update_bone(joints, jointScale, astra::JointType::RightHip, astra::JointType::RightKnee);
        update_bone(joints, jointScale, astra::JointType::RightKnee, astra::JointType::RightFoot);
    }

    void update_bone(const astra::JointList& joints,
                     const float jointScale,astra::JointType j1,
                     astra::JointType j2)
    {
        const auto& joint1 = joints[int(j1)];
        const auto& joint2 = joints[int(j2)];

        if (joint1.status() == astra::JointStatus::NotTracked ||
            joint2.status() == astra::JointStatus::NotTracked)
        {
            //don't render bones between untracked joints
            return;
        }

        //actually depth position, not world position
        const auto& jp1 = joint1.depth_position();
        const auto& jp2 = joint2.depth_position();

        auto p1 = sf::Vector2f(jp1.x, jp1.y);
        auto p2 = sf::Vector2f(jp2.x, jp2.y);

        sf::Color color(255, 255, 255, 255);
        float thickness = lineThickness_ * jointScale;
        if (joint1.status() == astra::JointStatus::LowConfidence ||
            joint2.status() == astra::JointStatus::LowConfidence)
        {
            color = sf::Color(128, 128, 128, 255);
            thickness *= 0.5f;
        }

        boneLines_.push_back(sfLine(p1,
            p2,
            color,
            thickness));
        const float shadowLineThickness = thickness + shadowRadius_ * jointScale * 2.f;
        boneShadows_.push_back(sfLine(p1,
            p2,
            sf::Color(0, 0, 0, 255),
            shadowLineThickness));
    }

    void update_overlay(const astra::BodyMask& bodyMask,
                        const astra::FloorMask& floorMask)
    {
        const auto* bodyData = bodyMask.data();
        const auto* floorData = floorMask.data();
        const int width = bodyMask.width();
        const int height = bodyMask.height();

        init_overlay_texture(width, height);

        const int length = width * height;

        for (int i = 0; i < length; i++)
        {
            const auto bodyId = bodyData[i];
            const auto isFloor = floorData[i];

            sf::Color color(0x0, 0x0, 0x0, 0x0);

            if (bodyId != 0)
            {
                color = get_body_color(bodyId);
            }
            else if (isFloor != 0)
            {
                color = sf::Color(0x0, 0x0, 0xFF, 0x88);
            }

            const int rgbaOffset = i * 4;
            overlayBuffer_[rgbaOffset] = color.r;
            overlayBuffer_[rgbaOffset + 1] = color.g;
            overlayBuffer_[rgbaOffset + 2] = color.b;
            overlayBuffer_[rgbaOffset + 3] = color.a;
        }

        overlayTexture_.update(overlayBuffer_.get());
    }

    void clear_overlay()
    {
        int byteLength = overlayWidth_ * overlayHeight_ * 4;
        fill(&overlayBuffer_[0], &overlayBuffer_[0] + byteLength, 0);

        overlayTexture_.update(overlayBuffer_.get());
    }

    void test_connection(vector<float> arr)
    {
        string url = "localhost:5000/?";

        for (int i = 0; i < 57; i++)
        {
            url += to_string(i);
            url += "=";
            url += to_string(arr[i]);
            url += "&";
        }
        curlpp::Cleanup cleaner;
        curlpp::Easy request;

        using namespace curlpp::Options;
        request.setOpt(Verbose(true));
        request.setOpt(Url(url));

        request.perform();
    }

    virtual void on_frame_ready(astra::StreamReader& reader,
                                astra::Frame& frame) override
    {
        if (mapper_ == nullptr)
        {
            auto& mapper = reader.stream<astra::DepthStream>().coordinateMapper();
            mapper_ = astra::make_unique<astra::CoordinateMapper>(mapper);
        }

        processDepth(frame);
        processBodies(frame);
        // check_fps();
    }

    void draw_bodies(sf::RenderWindow& window)
    {
        const float scaleX = window.getView().getSize().x / overlayWidth_;
        const float scaleY = window.getView().getSize().y / overlayHeight_;

        sf::RenderStates states;
        sf::Transform transform;
        transform.scale(scaleX, scaleY);
        states.transform *= transform;

        for (const auto& bone : boneShadows_)
            window.draw(bone, states);

        for (const auto& c : circleShadows_)
            window.draw(c, states);

        for (const auto& bone : boneLines_)
            window.draw(bone, states);

        for (auto& c : circles_)
            window.draw(c, states);

    }

    void draw_to(sf::RenderWindow& window)
    {
        if (displayBuffer_ != nullptr)
        {
            const float scaleX = window.getView().getSize().x / depthWidth_;
            const float scaleY = window.getView().getSize().y / depthHeight_;
            sprite_.setScale(scaleX, scaleY);

            window.draw(sprite_); // depth
        }

        if (overlayBuffer_ != nullptr)
        {
            const float scaleX = window.getView().getSize().x / overlayWidth_;
            const float scaleY = window.getView().getSize().y / overlayHeight_;
            overlaySprite_.setScale(scaleX, scaleY);
            window.draw(overlaySprite_); //bodymask and floormask
        }

        draw_bodies(window);
    }

private:
    long double frameDuration_{ 0 };
    clock_t lastTimepoint_ { 0 };
    sf::Texture texture_;
    sf::Sprite sprite_;

    using BufferPtr = unique_ptr < uint8_t[] >;
    BufferPtr displayBuffer_{ nullptr };

    unique_ptr<astra::CoordinateMapper> mapper_;
    vector<astra::Vector2f> jointPositions_;

    int depthWidth_{0};
    int depthHeight_{0};
    int overlayWidth_{0};
    int overlayHeight_{0};

    vector<sfLine> boneLines_;
    vector<sfLine> boneShadows_;
    vector<sf::CircleShape> circles_;
    vector<sf::CircleShape> circleShadows_;

    float lineThickness_{ 0.5f }; // pixels
    float jointRadius_{ 1.0f };   // pixels
    float shadowRadius_{ 0.5f };  // pixels

    BufferPtr overlayBuffer_{ nullptr };
    sf::Texture overlayTexture_;
    sf::Sprite overlaySprite_;

    bool collecting = false;
    int data_count = 1;
    int frame_count = 1;
    WorkoutState workout_state = WorkoutState::NoOp;
    WorkoutMode workout_mode;
};

astra::DepthStream configure_depth(astra::StreamReader& reader)
{
    auto depthStream = reader.stream<astra::DepthStream>();

    //We don't have to set the mode to start the stream, but if you want to here is how:
    astra::ImageStreamMode depthMode;

    depthMode.set_width(160);
    depthMode.set_height(120);
    depthMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
    depthMode.set_fps(30);

    depthStream.set_mode(depthMode);

    return depthStream;
}

int main(int argc, char** argv)
{
    astra::initialize();

    const char* licenseString = "<INSERT LICENSE KEY HERE>";
    orbbec_body_tracking_set_license(licenseString);

    sf::RenderWindow window(sf::VideoMode(1280, 960), "Simple Body Viewer");

#ifdef _WIN32
    auto fullscreenStyle = sf::Style::None;
#else
    auto fullscreenStyle = sf::Style::Fullscreen;
#endif

    const sf::VideoMode fullScreenMode = sf::VideoMode::getFullscreenModes()[0];
    const sf::VideoMode windowedMode(1280, 960);
    bool isFullScreen = false;

    astra::StreamSet sensor;
    astra::StreamReader reader = sensor.create_reader();

    BodyVisualizer listener;

    auto depthStream = configure_depth(reader);
    depthStream.start();

    reader.stream<astra::BodyStream>().start();
    reader.add_listener(listener);

    while (window.isOpen())
    {
        astra_update();

        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
            {
                if (event.key.code == sf::Keyboard::C && event.key.control)
                {
                    window.close();
                }
                switch (event.key.code)
                {
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::F:
                    if (isFullScreen)
                    {
                        window.create(windowedMode, "Simple Body Viewer", sf::Style::Default);
                    }
                    else
                    {
                        window.create(fullScreenMode, "Simple Body Viewer", fullscreenStyle);
                    }
                    isFullScreen = !isFullScreen;
                    break;
                case sf::Keyboard::R:
                    depthStream.enable_registration(!depthStream.registration_enabled());
                    break;
                case sf::Keyboard::M:
                    depthStream.enable_mirroring(!depthStream.mirroring_enabled());
                    break;
                case sf::Keyboard::Space:
                    // this is used to start/stop data collection
                    cout <<  "SPACE PRESSED" << endl;
                    listener.toggleDataCollection();
                    break;
                case sf::Keyboard::Num1:
                    // switch to recording bicep_curl
                    listener.setWorkoutMode(BodyVisualizer::WorkoutMode::BicepCurl);
                    break;
                case sf::Keyboard::Num2:
                    // switch to recording shoulder_press
                    listener.setWorkoutMode(BodyVisualizer::WorkoutMode::ShoulderPress);

                    break;
                case sf::Keyboard::Num3:
                    // switch to recording squat
                    listener.setWorkoutMode(BodyVisualizer::WorkoutMode::Squat);
                    break;
                case sf::Keyboard::Return:
                    // switch to the next state of current workout
                    listener.nextState();
                    break;
                default:
                    break;
                }
                break;
            }
            default:
                break;
            }
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        listener.draw_to(window);
        window.display();
    }

    astra::terminate();

    return 0;
}
