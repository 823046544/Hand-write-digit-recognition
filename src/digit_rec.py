import numpy as np
np.random.seed(1337)  # for reproducibility
from keras.datasets import mnist
from keras.utils import np_utils
from keras.models import Sequential
from keras.layers import Dense, Activation
from keras.layers import Flatten
from keras.layers.convolutional import Conv2D
from keras.layers.convolutional import MaxPooling2D
from keras.optimizers import RMSprop
from keras.models import load_model
from PIL import Image
from keras import backend as K
import cv2
import csv

L = []
with open('line.csv') as csv_file:
    reader = csv.reader(csv_file)
    for row in reader:
        for item in row:
            L.append(int(item))

K.set_image_dim_ordering('th')
model = load_model('./cnn_model.h5')
Ans = ''
index = 1
for i in range(L[0]):
    img = cv2.imread('./my_num/'+str(i)+'.bmp', cv2.IMREAD_GRAYSCALE)
    # img.show()
    a = []
    for x in range(28):
        for y in range(28):
            a.append(1.0*img[x, y])
    a = np.array([a])
    a = a.reshape(a.shape[0], 1, 28, 28).astype('float32')
    a = a/255.0
    b = model.predict(a)
    x = 0
    y = 0
    for j in range(10):
        if (b[0][j] > y):
            y = b[0][j]
            x = j
    # print (x)
    Ans = Ans+str(x)
    if (index < len(L) and i == L[index]):
        index = index+1
        Ans = Ans+'\n'
print(Ans)