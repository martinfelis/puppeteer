#!/usr/bin/python

import matplotlib.pyplot as plt
import pylab
import numpy

data_array = numpy.genfromtxt("fitting_log.csv", delimiter=',', names=True)

plt.figure()

ax1 = plt.subplot(1, 2, 1)

marker_count = len(data_array.dtype.names)
marker_names = data_array.dtype.names

average_errors = numpy.zeros(marker_count)
for col_name in sorted(data_array.dtype.names):
    average_errors[data_array.dtype.names.index(col_name)] = numpy.average(data_array[col_name])
bar_pos = numpy.arange(marker_count) + 0.5
rects = ax1.bar(bar_pos, average_errors, align='center')

ax1.axhline(numpy.average(average_errors), label="average", color="red")

pylab.xticks(bar_pos, marker_names, rotation=90)
ax1.legend()
ax1.set_title ('Average Marker Errors')

ax2 = plt.subplot(1, 2, 2)

for col_name in sorted(data_array.dtype.names):
    plt.plot(data_array[col_name])

ax2.set_title ('Marker Error per Capture Frame')

plt.show()
