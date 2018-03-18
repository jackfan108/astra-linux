import numpy as np
import tensorflow as tf
from keras import models
from keras.preprocessing.sequence import pad_sequences

model = models.load_model('./lstm_test.h5')
graph = tf.get_default_graph()

def normalize(frames):
  for index in range(len(frames)):
    frame = frames[index]
    head_c = (frame[0], frame[1], frame[2])
    transitioned_frame = [x - head_c[i%3] for i, x in enumerate(frame)]

    mid_feet_c = ((frame[36] + frame[45])/2, (frame[37] + frame[46])/2, (frame[38] + frame[47])/2)
    body_length = (mid_feet_c[0]**2 + mid_feet_c[1]**2 + mid_feet_c[2]**2)**0.5
    scaled_frame = [x/body_length for i, x in enumerate(transitioned_frame)]
    frames[index] = scaled_frame
  return frames


def pad(frames):
  return pad_sequences([frames], maxlen=80, dtype='float64', padding='post', truncating='post', value=np.zeros((57,)))


def predict_file(path):
  with open(path) as file:
    frames = np.array(list(map(lambda s: list(map(lambda n: float(n), s.split(','))), file.read().split("\n")[:-1])))
    frames = normalize(frames)
    frames = pad(frames)


    with graph.as_default():
      res = list(model.predict(frames)[0])
      # print('hi')
      i = res.index(max(res))
      if i == 0:
        print("bicep curl", int(res[i]*100))
      elif i == 1:
        print("shoulder press", int(res[i]*100))
      elif i == 2:
        print("squat", int(res[i]*100))




    # return frames
