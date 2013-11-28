#!/usr/bin/python

import matplotlib.pyplot as plt
from matplotlib import cm
import pylab
import numpy

data_array = numpy.genfromtxt("fitting_log.csv", delimiter=',', names=True)

plt.figure()

ax1 = plt.subplot(1, 2, 1)

marker_count = len(data_array.dtype.names)
marker_names = data_array.dtype.names
marker_colors = [None] * marker_count

average_errors = numpy.zeros(marker_count)
for col_name in sorted(data_array.dtype.names):
    marker_index = data_array.dtype.names.index(col_name)
    average_errors[marker_index] = numpy.average(data_array[col_name])
    marker_colors[marker_index] = cm.jet (1. * marker_index / marker_count)

bar_pos = numpy.arange(marker_count) + 0.5
rects = ax1.bar(bar_pos, average_errors, align='center', color=marker_colors)

ax1.axhline(numpy.average(average_errors), label="average", color="red")

pylab.xticks(bar_pos, marker_names, rotation=90)
ax1.legend()
pylab.ylim([0, 0.0175])
ax1.set_title ('Average Marker Errors')

ax2 = plt.subplot(1, 2, 2)

for col_name in sorted(data_array.dtype.names):
    marker_index = data_array.dtype.names.index(col_name)
    plt.plot(data_array[col_name], color=marker_colors[marker_index])

ax2.set_title ('Marker Error per Capture Frame')
pylab.ylim([0, 0.035])
plt.show()
