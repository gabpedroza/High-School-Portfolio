import sys
import pickle
import numpy as np
awb = 14.5 #part of the arm aWay from the body
acb = 26 #part of the arm close to the body
def calculate_position(a1, a2, a3):
    #a1 is the angle set to the innermost shoulder servo, a2 to the outermost and a3 to the elbow servo.
    #convention: z upwards, x outwards from the side of the body, y outwards to the front. origin in the shoulder, fixed orientation.
    #because of this, it is easier to pretend that both motor 2 and 3 rotate around the y unit vector (in this order) and then we rotate motor 1 round the x unit vector
    #this makes math much easier
    #since each motor has its own independent angle, an angle triplet determines a single possible coordinate (regardless of the order of rotations)
    #because of this, the above method works as intended
    vectorSE = np.array([0,0,-1*acb]) #vector from shoulder to the elbow
    vectorEH = np.array([0, 0, -1*awb]) #vector from elbow to the hand
    #the above positions correspond to an arm directly downwards (as is the case with all servos zeroed)

    rotationXaxis = np.array([[1, 0, 0], [0, np.cos(a1), -1*np.sin(a1)], [0, np.sin(a1), np.cos(a1)]]) #matrix for rotation around the x axis (rotating the inner servo)
    rotationYaxisShoulder = np.array([[np.cos(-a2), 0, np.sin(-a2)], [0, 1, 0], [-1*np.sin(-a2), 0, np.cos(-a2)]])
    rotationYaxisElbow = np.array([[np.cos(a3), 0, np.sin(a3)], [0, 1, 0], [-1 * np.sin(a3), 0, np.cos(a3)]])

    vectorSE, vectorEH = rotationYaxisShoulder.dot(vectorSE), rotationYaxisShoulder.dot(vectorEH)  # we rotate the outer servo from the shoulder. since this rotates the whole arm, both vectors are affected.
    vectorEH = rotationYaxisElbow.dot(vectorEH)  # we rotate the elbow servo. since this only affects the awb, we only change its vector
    vectorSE, vectorEH = rotationXaxis.dot(vectorSE), rotationXaxis.dot(vectorEH) #we rotate the inner servo. since this rotates the whole arm, both vectors are affected.

    position = vectorSE + vectorEH #we sum the vectors to get the position of the hand
    return position
d = {}
for a1 in range(0, 181, 2): #we go through all angles and calculate their resulting positions
    for a2 in range(0, 181, 2):
        for a3 in range(0, 181, 2):
            pos = calculate_position(np.radians(a1), np.radians(a2), np.radians(a3))
            tripletPos = (round(pos[0]*5)/5.0, round(pos[1]*5)/5.0,round(pos[2]*5)/5.0) #precision of half centimeter
            if tripletPos not in d: #store the angles in the position place
                d[tripletPos] = [(a1, a2, a3)]
            else:
                d[tripletPos].append((a1, a2, a3))
with open("cvtPosToAng.pickle", "wb") as file: #create the file with the resulting map
    pickle.dump(d, file, protocol=pickle.HIGHEST_PROTOCOL)
