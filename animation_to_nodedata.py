#!/usr/bin/python

import pylab
import numpy
import math
import sys

frame_first = 239
frame_rate = 100

phases = [
        { 'name': 'RightFlat', 'start_frame': 303, 'nshoot': 30},
        { 'name': 'RightToe', 'start_frame': 333, 'nshoot': 10},
        { 'name': 'RightToeTouchDownLeftHeel', 'start_frame': 343, 'nshoot': 1},
        { 'name': 'RightToeLeftHeel', 'start_frame': 343, 'nshoot': 5},
        { 'name': 'RightToeTouchDownLeftToe', 'start_frame': 350, 'nshoot': 1},
        { 'name': 'RightToeLeftFlat', 'start_frame': 350, 'nshoot': 5},
        { 'name': 'End', 'start_frame': 360}
        ]
    
animation_data = numpy.genfromtxt("animation.csv", delimiter=',')

dof_names = [
"Pelvis_TX", "Pelvis_TY", "Pelvis_TZ", "Pelvis_RY", "Pelvis_RX", "Pelvis_RZ",
"Thigh_r_RY", "Thigh_r_RX", "Thigh_r_RZ", "Shank_r_RY", "Foot_r_RY",
"Foot_r_RX", "Foot_r_RZ", "Thigh_l_RY", "Thigh_l_RX", "Thigh_l_RZ",
"Shank_l_RY", "Foot_l_RY", "Foot_l_RX", "Foot_l_RZ", "MiddleTrunk_RY",
"MiddleTrunk_RX", "MiddleTrunk_RZ", "UpperArm_r_RY", "UpperArm_r_RX",
"UpperArm_r_RZ", "LowerArm_r_RY", "UpperArm_l_RY", "UpperArm_l_RX",
"UpperArm_l_RZ", "LowerArm_l_RY", "Head_RY", "Head_RX", "Head_RZ", 
]

def calc_phase_durations (data):
    h = numpy.ndarray(len(phases) - 1)

    for i in range(0,len(phases)):
        if phases[i]["name"] == "End":
            break
        
        phase_start = (phases[i]["start_frame"] - float(frame_first)) / frame_rate
        phase_end = (phases[i + 1]["start_frame"] - float(frame_first)) / frame_rate

        h[i] = phase_end - phase_start

    return h

def calc_interpolated_values (data, timestamp):
    prev_line = math.floor(timestamp * frame_rate)
    next_line = prev_line + 1
    fraction = (timestamp - prev_line / frame_rate) * frame_rate

    prev_values = data[prev_line][1:]
    next_values = data[next_line][1:]

    return prev_values * (1. - fraction) + fraction * next_values

def calc_state_derivatives (data, timestamp, step):
    return (calc_interpolated_values (data, timestamp + step) - calc_interpolated_values (data, timestamp - step)) / (2 * step)

def print_phase (data, phase_index, time_start, time_end, num_nodes):
    print("*\n* Phase: " + str(phase_index) + " " + phases[phase_index]["name"] + "\n*")
    for i in range(0,num_nodes):
        timestamp = time_start
        line = math.floor(timestamp * frame_rate)
        values = data[line][1:]

        if num_nodes > 1:
            timestamp = time_start + i * (time_end - time_start) / (num_nodes - 1)
            values = calc_interpolated_values (data, timestamp)

        values_dt = calc_state_derivatives (data, timestamp, 1.0e-7)

        print ("* t = " + str(timestamp))
        print("sd(" + str(phase_index) + ", " + str(i) + ")")
        for j in range(0, len(values)):
            print (str(j) + ": " + str(values[j]))

        for j in range(0, len(values_dt)):
            print (str(j + len(values)) + ": " + str(values_dt[j]))

        
        print ("")

def print_node_data ():
    for i in range(0,len(phases)):
        if phases[i]["name"] == "End":
            break

        phase_start = (phases[i]["start_frame"] - float(frame_first)) / frame_rate
        phase_end = (phases[i + 1]["start_frame"] - float(frame_first)) / frame_rate

        print_phase (animation_data, i, phase_start, phase_end, phases[i]["nshoot"] + 1)

def print_phase_durations ():
    h = calc_phase_durations(animation_data)

    print ("h")
    for i in range(0,len(h)):
        print (str(i) + ": " + str(h[i]))

    print ("")

def print_nshoot():
    print ("nshoot")
    for i in range(0,len(phases)):
        if phases[i]["name"] == "End":
            break

        print (str(i) + ": " + str(phases[i]["nshoot"]))

    print ("")

def print_xdname():
    print ("xd_name")
    for i in range (0, len(dof_names)):
        print (str(i) + ": " + dof_names[i])
    for i in range (0, len(dof_names)):
        print (str(i + len(dof_names)) + ": " + dof_names[i] + "_dot" )
    print ("")

def print_uname():
    print ("u_name")
    for i in range (6, len(dof_names)):
        print (str(i - 6) + ": " + dof_names[i - 6])

# shift the data such that the pelvis origin has x,y coordinates 0,0
start_line = phases[0]["start_frame"] - frame_first
pos_start = animation_data[start_line][1:3]
animation_data[:,1:3] = animation_data[:,1:3] - pos_start

print ("* GENERATED INITIAL START")
print_nshoot()
print_phase_durations()
print_node_data()
print_xdname()
print_uname()

print ("* GENERATED INITIAL END")


