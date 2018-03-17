#!/usr/bin/env python

from flask import Flask, render_template, request
import numpy as np
import tensorflow as tf
import keras
import json
import datetime
import logging


app = Flask(__name__)
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)


model = keras.models.load_model('./lstm_test.h5')
graph = tf.get_default_graph()

sec = 0
count_per_sec = 0
frames = np.zeros(57, dtype='float64')

@app.route('/')
def homepage():
  global sec
  global count_per_sec
  global frames

  now = datetime.datetime.now().second
  if now == sec:
    count_per_sec += 1
  else:
    sec = now
    count_per_sec = 0

  print(count_per_sec)



  params = request.args
  frame = np.zeros(57, dtype='float64')
  # print(frames.shape)

  if params:
    for k in params.keys():
      frame[int(k)] = float(params[k])
  frames = np.vstack([frames, frame])

  if len(frames) >= 80:
    predict(normalize(frames[-80:]))

  return render_template('index.html')


def normalize(frames):
  # print("moment of truth please:")
  # print(frames)
  for index in range(len(frames)):
    frame = frames[index]
    head_c = (frame[0], frame[1], frame[2])
    transitioned_frame = [x - head_c[i%3] for i, x in enumerate(frame)]
    # print('check yo')
    # print(frame[36], frame[45], frame[37], frame[46], frame[38], frame[47])
    mid_feet_c = ((frame[36] + frame[45])/2, (frame[37] + frame[46])/2, (frame[38] + frame[47])/2)
    # print("mid_feet_c:", mid_feet_c)
    body_length = (mid_feet_c[0]**2 + mid_feet_c[1]**2 + mid_feet_c[2]**2)**0.5
    # print("body_length: ", body_length)
    scaled_frame = [x/body_length for i, x in enumerate(transitioned_frame)]
    frames[index] = scaled_frame
  return frames




def predict(frames):
  global graph
  input_data = np.array([frames]) # project frames to 3 dimension (1, 80, 57)
  # print("shape:  ", input_data.shape)
  with graph.as_default():
    res = list(model.predict(input_data)[0])
    # print('hi')
    i = res.index(max(res))
    if i == 0:
      print("bicep curl", int(res[i]*100))
    elif i == 1:
      print("shoulder press", int(res[i]*100))
    elif i == 2:
      print("squat", int(res[i]*100))





if __name__ == "__main__":
  app.run(debug=True, host='localhost')
