#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy

data_array = numpy.genfromtxt("fitting_log.csv", delimiter=',', names=True)

plt.figure()
for col_name in data_array.dtype.names:
    plt.plot(data_array[col_name])

plt.show()
