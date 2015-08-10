#!/usr/bin/python

import matplotlib.pyplot as plt
from matplotlib import cm
import pylab
import numpy
import sys

def usage (arg0):
    print ("Usage: {} [filename.log] [--pdf] [--png]".format(arg0))
    print ("Plots fitting errors for the given filename.")
    print ("--pdf     Save pdf of the plot as [filename].pdf")
    print ("--png     Save png of the plot as [filename].png")
    sys.exit(1)

filename = "fitting_log.csv"
save_pdf = False
save_png = False
if len(sys.argv) > 1:
    filename = sys.argv[1]

if len(sys.argv) > 2:
    index = 2
    while index < len(sys.argv):
        arg = sys.argv[index]
        if arg == "--pdf":
            save_pdf = True
        elif arg == "--png":
            save_png = True
        else:
            usage (sys.argv[0])
        index = index + 1

data_array = numpy.genfromtxt(filename, delimiter=',', names=True)

figure = plt.figure(figsize=(13,4))

ax1 = plt.subplot(1, 3, 1)
marker_names = data_array.dtype.names[2:]
marker_count = len(marker_names)
marker_colors = [None] * marker_count

average_errors = numpy.zeros(marker_count)
for col_name in sorted(marker_names):
    marker_index = marker_names.index(col_name)
    average_errors[marker_index] = numpy.average(data_array[col_name])
    marker_colors[marker_index] = cm.jet (1. * marker_index / marker_count)

bar_pos = numpy.arange(marker_count) + 0.5
rects = ax1.bar(bar_pos, average_errors, align='center', color=marker_colors)

ax1.axhline(numpy.average(average_errors), label="average", color="red")

pylab.xticks(bar_pos, marker_names, rotation=90)
ax1.legend()
ax1.set_title ('Average Marker Errors')
ax1.set_ylabel ('(m)')
pylab.xlim([0, marker_count])
pylab.ylim([0, 0.06])

ax2 = plt.subplot(1, 3, 2)

for col_name in sorted(marker_names):
    marker_index = marker_names.index(col_name)
    plt.plot(data_array[col_name], color=marker_colors[marker_index])
    pylab.xlim([0, len(data_array[col_name])])

ax2.set_title ('Marker Error per Capture Frame')
ax2.set_ylabel ('(m)')
pylab.ylim([0, 0.06])
pylab.xlim ([0, len(data_array['frame'])])

ax3 = plt.subplot(1, 3, 3)
ax3.set_title ('IK Steps')
plt.plot(data_array['frame'], data_array['steps'], 'x')
ax3.axhline(numpy.average(data_array['steps']), label="average", color="red")
pylab.xlim ([0, len(data_array['frame'])])
pylab.ylim ([0, 200])

figure.canvas.set_window_title (filename)

plt.subplots_adjust(left=0.08, right = 0.98, bottom=0.2)
if save_pdf:
    plt.savefig (filename[:-3] + "pdf", format='pdf')

if save_png:
    plt.savefig (filename[:-3] + "png", format='png')
     
plt.show()
