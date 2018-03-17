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
#include <astra/capi/astra.h>
#include <stdio.h>
#include <key_handler.h>

void print_color(astra_colorframe_t colorFrame)
{
    astra_image_metadata_t metadata;
    astra_rgb_pixel_t* colorData_rgb;

    uint32_t colorByteLength;

    astra_colorframe_get_data_rgb_ptr(colorFrame, &colorData_rgb, &colorByteLength);

    astra_colorframe_get_metadata(colorFrame, &metadata);

    int width = metadata.width;
    int height = metadata.height;
    size_t index = ((width * (height / 2)) + (width / 2));

    astra_frame_index_t frameIndex;
    astra_colorframe_get_frameindex(colorFrame, &frameIndex);

    astra_rgb_pixel_t middle = colorData_rgb[index];
    printf("color frameIndex: %d  r: %d    g: %d    b: %d \n", frameIndex, (int)(middle.r), (int)(middle.g), (int)(middle.b));
}

void frame_ready(void* clientTag, astra_reader_t reader, astra_reader_frame_t frame)
{
    astra_colorframe_t colorFrame;
    astra_frame_get_colorframe(frame, &colorFrame);

    print_color(colorFrame);
}

int main(int argc, char* argv[])
{
    set_key_handler();

    astra_initialize();

    astra_streamsetconnection_t sensor;

    astra_streamset_open("device/default", &sensor);

    astra_reader_t reader;
    astra_reader_create(sensor, &reader);

    astra_colorstream_t colorStream;
    astra_reader_get_colorstream(reader, &colorStream);
    astra_stream_start(colorStream);

    astra_reader_callback_id_t callbackId;
    astra_reader_register_frame_ready_callback(reader, &frame_ready, NULL, &callbackId);

    do
    {
        astra_update();

    } while (shouldContinue);

    astra_reader_unregister_frame_ready_callback(&callbackId);

    astra_reader_destroy(&reader);
    astra_streamset_close(&sensor);

    astra_terminate();
}
