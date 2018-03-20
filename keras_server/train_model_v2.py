import os
import numpy as np
from keras.preprocessing import sequence
from keras.models import Sequential
from keras.layers import Dense, Embedding, TimeDistributed
from keras.layers import LSTM
from keras.utils.np_utils import to_categorical

# max_features = 20000 corresponds to embedding layer, don't need this for our project
maxlen = 300  # cut frames after this number of frames (TimeStep)
batch_size = 1
data_dim = 57
split_index = 28 # take 28 training data points, and make the rest testing data

BASE_DIR = '/home/jackfan108/AstraSDK/data/v2/'
FILES = ['no_op', 'bicep_cont', 'shoulder_press_cont', 'squat_cont']

def get_file_names():
  return [BASE_DIR + afile for afile in os.listdir(BASE_DIR)]


def load_data():
  file_names = get_file_names()
  labels = []
  stacked_data = []
  for fn in file_names:
    with open(fn, 'r') as f:
      data = f.read()
      data = np.array(list(map(lambda s: list(map(lambda f: float(f), s.split(","))), data.split("\n")[:-1])))
      # grab the first column as labels
      labels.append(data[:,0])
      # grab the rest as data
      stacked_data.append(data[:,1:])

  # cast to integer for labeling
  labels = np.array(labels, dtype='int')
  # 2d -> 3d 1 hot array
  labels = to_categorical(labels, num_classes=7)

  stacked_data = np.array(stacked_data)

  training_label, training_data = labels[:split_index], stacked_data[:split_index]
  testing_label, testing_data = labels[split_index:], stacked_data[split_index:]
  # can shuffle data here

  return training_data, training_label, testing_data, testing_label


def main():
  print('Loading data...')
  x_train, y_train, x_test, y_test = load_data()

  print("x_train shape: ", x_train.shape)
  print("y_train shape: ", np.array(y_train).shape)

  print('Build model...')
  model = Sequential()
  # model.add(Embedding(max_features, 128)) # not needed for our project
  model.add(LSTM(128, input_length=maxlen, input_dim=data_dim, dropout=0.2, recurrent_dropout=0.2, activation='tanh', return_sequences=True)) # Try activation='relu', might be better.
  # mode.add(TimeDistribution(Dense(3, acitvation= 'softmax'))) Simply use fully connected layer instead of lstm
  model.add(TimeDistributed(Dense(7, activation='softmax')))

  # try using different optimizers and different optimizer configs
  model.compile(loss='categorical_crossentropy',
  # model.compile(loss='mean_squared_error',
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

  model.save('lstm_test_v2.h5')  # creates a HDF5 file 'my_model.h5'
  print('Test score:', score)
  print('Test accuracy:', acc)

if __name__ == '__main__':
  main()
