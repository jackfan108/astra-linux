import os
import numpy as np
from keras.preprocessing import sequence
from keras.models import Sequential
from keras.layers import Dense, Embedding
from keras.layers import LSTM

# max_features = 20000 corresponds to embedding layer, don't need this for our project
maxlen = 80  # cut frames after this number of frames (TimeStep)
batch_size = 1
data_dim = 57
split_index = 30 # take 30 training data points, and make the rest testing data

BASE_DIR = '/Users/Mac11387/fulcrum/'

def get_file_names(exercise_name):
  path = BASE_DIR + "workout_data/workout_data_normalized_02252018/"
  file_names = [path+afile for afile in os.listdir(path) if exercise_name in afile]
  return file_names

def get_padded_data(exercise_name):
  file_names = get_file_names(exercise_name)
  res = []
  for fn in file_names:
    with open(fn, 'r') as f:
      data = f.read()
      res.append(map(lambda s: map(lambda f: float(f), s.split(",")), data.split("\r\n")[:-1]))
  padded_res = sequence.pad_sequences(res,
    value=np.full(57, 0.0),
    maxlen=80,
    padding='post',
    truncating='post',
    dtype=np.float64,
  )
  return padded_res

def load_data():
  bicep_curl = get_padded_data('bicep_curl')
  shoulder_press = get_padded_data('shoulder_press')
  squat = get_padded_data('squat')
  bc = (bicep_curl[0:split_index], bicep_curl[split_index:])
  sp = (shoulder_press[0:split_index], shoulder_press[split_index:])
  sq = (squat[0:split_index], squat[split_index:])

  return bc, sp, sq

def main():
  print('Loading data...')
  (bc_train, bc_test), (sp_train, sp_test), (sq_train, sq_test) = load_data()

  x_train = np.vstack([bc_train, sp_train, sq_train])
  y_train = np.array([[1, 0, 0]] * split_index + [[0, 1, 0]] * split_index + [[0, 0, 1]] * split_index)
  # TODO: shuffle (x,y)

  print "x_train shape: ", x_train.shape
  print "y_train shape: ", np.array(y_train).shape

  x_test = np.vstack([bc_test, sp_test, sq_test])
  y_test = np.array([[1, 0, 0]] * bc_test.shape[0] + [[0, 1, 0]] * sp_test.shape[0] + [[0, 0, 1]] * sq_test.shape[0])

  print('Build model...')
  model = Sequential()
  # model.add(Embedding(max_features, 128)) # not needed for our project
  model.add(LSTM(128, input_length=maxlen, input_dim=data_dim, dropout=0.2, recurrent_dropout=0.2, activation='tanh')) # Try activation='relu', might be better.
  # mode.add(TimeDistribution(Dense(3, acitvation= 'softmax'))) Simply use fully connected layer instead of lstm
  model.add(Dense(3, activation='softmax'))

  # try using different optimizers and different optimizer configs
  model.compile(loss='categorical_crossentropy',
                optimizer='adam',
                metrics=['accuracy'])

  print('Train...')
  model.fit(x_train, y_train,
            batch_size=batch_size,
            epochs=100,
            # validation_data=(x_test, y_test),
            validation_split=0.12)
  score, acc = model.evaluate(x_test, y_test,
                              batch_size=batch_size)

  model.save('lstm_test.h5')  # creates a HDF5 file 'my_model.h5'
  print('Test score:', score)
  print('Test accuracy:', acc)

if __name__ == '__main__':
  main()
