#!/usr/bin/python

import matplotlib.pyplot as plt
from matplotlib import cm
import pylab
import numpy

dof_names = [ " 0: Pelvis_TX", " 1: Pelvis_TY", " 2: Pelvis_TZ", " 3: Pelvis_RY",
  " 4: Pelvis_RX", " 5: Pelvis_RZ", " 6: Thigh_r_RY", " 7: Thigh_r_RX", " 8: Thigh_r_RZ",
  " 9: Shank_r_RY", "10: Foot_r_RY", "11: Foot_r_RX", "12: Foot_r_RZ", "13: Thigh_l_RY",
  "14: Thigh_l_RX", "15: Thigh_l_RZ", "16: Shank_l_RY", "17: Foot_l_RY", "18: Foot_l_RX",
  "19: Foot_l_RZ", "20: MiddleTrunk_RY", "21: MiddleTrunk_RX", "22: MiddleTrunk_RZ",
  "23: UpperTrunk_RY", "24: UpperTrunk_RX", "25: UpperTrunk_RZ", "26: UpperArm_r_RY",
  "27: UpperArm_r_RX", "28: UpperArm_r_RZ", "29: LowerArm_r_RY", "30: LowerArm_r_RZ",
  "31: UpperArm_l_RY", "32: UpperArm_l_RX", "33: UpperArm_l_RZ", "34: LowerArm_l_RY",
  "35: LowerArm_l_RZ", "36: Head_RY", "37: Head_RX", "38: Head_RZ" ]

custom_data_array = numpy.genfromtxt("custom.csv", delimiter=',') * 180. / 3.141
deleva_data_array = numpy.genfromtxt("deleva.csv", delimiter=',') * 180. / 3.141

fig, axes_grid = plt.subplots (7, 6, sharey=False)
axes = [item for sublist in axes_grid for item in sublist]

for i in range (1, custom_data_array.shape[1]):
    axes[i - 1].plot (custom_data_array[:,0], custom_data_array[:,i] - deleva_data_array[:,i])
    axes[i - 1].set_title(dof_names[i - 1])
#    axes[i - 1].plot (deleva_data_array[:,0], deleva_data_array[:,i])

plt.subplots_adjust(left=0.04, right = 0.98, top=0.98, bottom= 0.04)
plt.show()
