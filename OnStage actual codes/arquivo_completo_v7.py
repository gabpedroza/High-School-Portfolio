import easyocr
import cvlib as cv
from cvlib.object_detection import draw_bbox
import cv2
import audioCode as ac
import ocrCode as oc
import time
import importlib
import serial
import pygame
funcionou = False
import numpy as np
from pygame.locals import *
import pickle
qtd = {}
ser = serial.Serial('COM30', 115200)
cvtPosToAng = {}
cvtPosToAngEsq = {}
resolution = [1280, 720]
#initiates the screen in which our images shall be shown
pygame.init()
pygame.display.set_caption("OpenCV camera stream on Pygame")
screen = pygame.display.set_mode(resolution)

with open("cvtPosToAng.pickle", "rb") as file: #imports the correspondence between positions and angles
    cvtPosToAng = pickle.load(file)
#with open("cvtPosToAngEsq.pickle", "rb") as file:
#    cvtPosToAngEsq = pickle.load(file)
# character recognition
#print("easyocr")
'''
reader = easyocr.Reader(['en'], gpu=True)
captura = cv2.VideoCapture(0, cv2.CAP_DSHOW)
'''
# voice/count recognition
totalSize = 640

def reset():
    # unused
    x = 1
def cvt(displac, totalSize):
    return int(((displac + (totalSize/2))*20/totalSize)) #this function converts face position to motor angle, using proportions
qttErrors = 0
def enviar_arduino(c):
    contador_falha, contador_sucesso = 0, 0  # attempts to send it multple times. If it fails too many times, its an signal to reset and try again
    while contador_falha <= 00 and contador_sucesso < 1:  # that's why I have used counters (that are set for a single "launch"[sends an letter to the arduino],because its reliable)
        try:
            ser.write(c)
            time.sleep(0.1)  # attempts to send the desired character
            contador_sucesso += 1
        except:
            contador_falha += 1
    if (contador_falha > 10):
        return False  # returns "error" if it fails too much
    else:
        return True  # returns "true" if it doesn't fail
    # time.sleep(1)
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

def calc_delta(currentAngles, angles): #calculates the difference between two servo positions. the weights add naturality
    return (currentAngles[0]-angles[0])*(currentAngles[0]-angles[0])*4 \
            + (currentAngles[1]-angles[1])*(currentAngles[1]-angles[1])*2\
            + (currentAngles[2]-angles[2])*(currentAngles[2]-angles[2])
def seek_position(position, currentAngles):
    pos = (round(position[0]*5)/5.0, round(position[1]*5)/5.0, round(position[2]*5)/5.0) #we round them so as to match the pattern used in the mapping
    if pos not in cvtPosToAng:  # if the position seeked is impossible, we try to see if a neighbor is valid. else, return null
        dx = [-0.4,-0.2, 0, +0.2,+0.4]
        win = 17
        for i in range(0, 5):
            for j in range(0, 5):
                for k in range(0, 5):
                    if (pos[0] + dx[i], pos[1] + dx[j], pos[2] + dx[k]) in cvtPosToAng and (i-2)*(i-2) + (j-2)*(j-2) + (k-2)*(k-2) < win:
                        pos = (pos[0] + dx[i], pos[1] + dx[j], pos[2] + dx[k])
                        win = (i-2)*(i-2) + (j-2)*(j-2) + (k-2)*(k-2)
        if win == 17:
            return (-1, -1, -1)
    angles = cvtPosToAng[pos] #receives the possible angles to achieve the position
    answer = ()
    delta = 10e6
    for tuple in angles: #we go through the possible angles in order to decide the one closest to our current angles
        a = calc_delta(currentAngles, tuple)
        if(a < delta):
            delta = a
            answer = tuple
    return answer #we return the winner
def showCV2Frame(frame, screen): #this code shows a cv2 frame object in a pygame screen
    screen.fill([0, 0, 0])
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame = np.rot90(frame)
    frame = pygame.surfarray.make_surface(frame)
    screen.blit(frame, (0, 0))
    pygame.display.update()
animations = {"blink":cv2.VideoCapture("blink.mp4"), "processing":cv2.VideoCapture("processing.mp4"), "pissed":cv2.VideoCapture("processing.mp4"), "glitch":cv2.VideoCapture("glitch.mp4")}
animationsRead = {"blink":cv2.VideoCapture("blink.mp4").read(), "processing":cv2.VideoCapture("processing.mp4").read(), "pissed":cv2.VideoCapture("processing.mp4").read(), "glitch":cv2.VideoCapture("glitch.mp4").read()}
def show_face(title): #this code shows a mp4 video on the screen
    video = cv2.VideoCapture(title + ".mp4")
    success, video_image = video.read()
    fps = video.get(cv2.CAP_PROP_FPS)
    clock = pygame.time.Clock()

    run = success
    print(run)

    while run:
        clock.tick(fps)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

        success, video_image = video.read()
        print(success)
        if success:
            video_surf = pygame.image.frombuffer(
                video_image.tobytes(), video_image.shape[1::-1], "BGR")
        else:
            run = False
        screen.blit(video_surf, (0, 0)) #sets the video to be shown with upper left corner at (x, y)
        pygame.display.flip()
    print("bye")
def track_objects():
    i = 0
    while i <= 20:
        video = cv2.VideoCapture(0)
        ok, frame = video.read()  # receives the image
        bbox, label, conf = cv.detect_common_objects(frame, model="yolov3")
        print(bbox, label, conf)
        newBbox = []
        newLabel = []
        newConf = []
        for i in range(0, len(bbox)):
            if label[i] == 'person':
                newBbox.append(bbox[i])
                newLabel.append(label[i])
                newConf.append(conf[i])
        if len(newBbox) == 0:
            newBbox.append([totalSize/2, 0, 0, 0])
        x, y, w, h = [int(v) for v in newBbox[0]]  # records the tracked object's (x,y) position, its width w and its heigh h
        ser.write(bytes(str(cvt((2 * x + w) / 2 - totalSize / 2, totalSize)),
                        'ascii'))  # here we send the angle the head motor should rotate, based on object position
        #draw bounding box over detected objects
        #out = draw_bbox(newFrame, newBbox, newLabel, newConf)
        #showCV2Frame(frame, screen)
        i += 1
def rastreador_faces():
    print("bgin tracking")
    tracker = cv2.TrackerKCF_create()
    # tracker = cv2.TrackerCSRT_create()
    detectorFace = cv2.CascadeClassifier(
        cv2.data.haarcascades + "haarcascade_frontalface_default.xml")  # initializes independent face detector
    # tracker = cv2.TrackerCSRT_create()
    #detectorFace = cv2.CascadeClassifier("banana_classifier.xml")  # initializes independent face detector
    video = cv2.VideoCapture(1, cv2.CAP_DSHOW)
    ok, frame = video.read()  # receives the image
    detectedFaces = ()
    while len(detectedFaces) == 0:
        status, image = video.read()  # captures an image
        imageGrey = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # transforms the image to greyscale
        detectedFaces = detectorFace.detectMultiScale(imageGrey, scaleFactor=1.5,
                                                      minSize=(150, 150))  # detects faces in the image
        showCV2Frame(frame, screen)

    bbox = (detectedFaces[0][0], detectedFaces[0][1], detectedFaces[0][2],
            detectedFaces[0][3])  # we then set the object to be tracked as the face detected
    ok = tracker.init(frame, bbox)  # initialize the tracker
    totalSize = len(frame[0])  # this allows us to see the x-size of the recording
    print(totalSize)
    i = 0
    ready = round(time.time() * 1000)
    while round(time.time() * 1000) - ready <= 7000:
        i += 1
        ok, frame = video.read()
        if not ok:  # if video input is over
            break
        ok, bbox = tracker.update(frame)  # updates the frame analyzed
        # print(bbox)
        if ok:
            qttErrors = 0
            x, y, w, h = [int(v) for v in
                          bbox]  # records the tracked object's (x,y) position, its width w and its heigh h
            ser.write(bytes(str(cvt((2 * x + w) / 2 - totalSize / 2, totalSize)),
                            'ascii'))  # here we send the angle the head motor should rotate, based on object position
            cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)  # draws a rectangle around the object
            #print(ser.readline())
            print(bytes(str(cvt((2 * x + w) / 2 - totalSize / 2, totalSize)), 'ascii'))
        else:  # if it cannot see the object tracked, it shows an error message
            cv2.putText(frame, "Error", (100, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

            qttErrors += 1  # this variable stores how many consecutive frames resulted in a error while tracking
            if (
                    qttErrors >= 50):  # if greater than a certain amount, it restarts the process of selecting something to track
                detectedFaces = ()
                while len(detectedFaces) == 0:
                    status, image = video.read()  # captures an image
                    imageGrey = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # transforms the image to greyscale
                    detectedFaces = detectorFace.detectMultiScale(imageGrey, scaleFactor=1.5,
                                                                  minSize=(150, 150))  # detects faces in the image
                # print(detectedFaces)
                bbox = (detectedFaces[0][0], detectedFaces[0][1], detectedFaces[0][2],
                        detectedFaces[0][3])  # we then set the object to be tracked as the face detected
                tracker = cv2.TrackerKCF_create()
                tracker.init(frame, bbox)
                qttErrors = 0
        showCV2Frame(frame, screen)

        if cv2.waitKey(1) & 0XFF == 27:  # pressing ESC stops code
            video.release()
            importlib.reload(oc) # these two lines of code are to prevent future OCR reads from crashing
            break

    enviar_arduino(b'255')
    video.release()
    importlib.reload(oc)  # these two lines of code are to prevent future OCR reads from crashing

'''
the code

the code uses a pattern to activate its functions:

while(funcao == falhaGrave):
    restarts the library


The pattern works like this:it tries to execute the function. the function returns if everything went right (like if it went to the desired "if" and so on)
or not. if the function works it leaves the loop and moves on. else, it leaves the loop (if its a fault at recognizing an cable connection),
or it continues in the loop (if its an failure on the code,in other words, an except). reinitializing the "v" tneds to fix the problems.
'''
arduinoReady = False
while ser.readline() != b'z\r\n':
    print("waiting")
while (1):
    voz = ac.reconhecer_frase()  # here it captures the command of the operator
    # what follows is an series of "if's" to know which command was executed

    print("funfou")
    if 'problemaComum' in voz:
        continue  # the voice command can create problems if it falls on the "except", in that case, simply try again
    elif 'problemaReset' in voz:
        importlib.reload(ac)
        continue  # if the problem is connected to some deep issues, it resets the "v" and tries again
    if 'good morning' in voz:  # trigger of the greeting
        enviar_arduino(b'G')
        show_face("blinkRapido")
        while ser.readline() != b'r\r\n':
            print("waiting")
        while (ac.falar_palavra("Good morning") == 3):
            importlib.reload(ac)
    elif 'recognize' in voz:  # trigger to recognize word through camera
        enviar_arduino(b'O')
        show_face("processingRapido")
        while (1):
            letra = oc.reconhecer_letra()  # here it tries to receive an letter. if it goes wrong it returns "vishkk"
            if (letra != 2 and letra != 3):
                ac.falar_palavra(letra)  # here it tries to read the letter(if it doesn't go wrong)
                break
            elif letra == 3:
                print('falhaGraveLetra')  # if it goes wrong gravely, the ocr library shall be reimported
                importlib.reload(oc)
            else:
                print("falhaComumLetra") # if it just didn't recognise any meaningful word, it simply tries again
    elif 'calculate' in voz:  # trigger to the math operation
        enviar_arduino(b'C')
        show_face("e")
        while (ac.fazer_conta(voz) == 3):
            importlib.reload(ac)
    elif 'receipt' in voz:  # trigger to the first rebelious act, it doesn't read the medical receipt
        enviar_arduino(b'R')
        show_face("pissedRapido")
        while (ac.falar_palavra("no thanks you do it yourself") == 3):
            importlib.reload(ac)
    elif 'angry' in voz:
        enviar_arduino(b'A')
        while (ac.falar_palavra("im not in the mood") == 3):
            importlib.reload(ac)
    elif 'reboot' in voz:  # trigger for self-destruction
        while (1):  # while it fails to send the mini, it tries again
            if (enviar_arduino(b'D')):  # tries to send the mini
                print("mandou")
                break
            else:
                continue
        while (ac.falar_palavra("confirmed... initiating self destruction .") == 3):
            importlib.reload(ac)
        while (ac.falar_palavra(" 3.") == 3):
            importlib.reload(ac)
        time.sleep(0.1)
        while (ac.falar_palavra(" 2.") == 3):
            importlib.reload(ac)
        time.sleep(0.1)
        while (ac.falar_palavra(" 1.") == 3):
            importlib.reload(ac)
        while ser.readline() != b'r\r\n':
            print("waiting")
        while (ac.falar_palavra("ha ha ha ...It was a jok/e.") == 3):
            importlib.reload(ac)
        break  # this break finishes the code
    elif 'detect' in voz:
        print ('tracking')
        enviar_arduino(b'T')
        print ('enviado')
        try:
            print ('try')
            rastreador_faces()
        except :
            print ('except')
    elif 'move' in voz:
        print('moving')
        enviar_arduino(b'M')
        print("enviado")
    elif 'blink' in voz:
        print("facingg")
        show_face("blink")
    elif 'identify' in voz:
        print("object")
        enviar_arduino(b'I')
        track_objects()
    else:
        print('naoReconheceuNada')
        continue
