import cv2
import serial
import time
tracker = cv2.TrackerKCF_create()
#tracker = cv2.TrackerCSRT_create()
ser = serial.Serial('COM4', 115200)#initializes serial. must select proper COM port.
detectorFace = cv2.CascadeClassifier(cv2.data.haarcascades+"haarcascade_frontalface_default.xml") #initializes independent face detector
video = cv2.VideoCapture(0)
ok, frame = video.read() #receives the image
detectedFaces = ()
'''
the following code is intended for independent face reconnaissance
we try to capture a face and store it in a tuple, using AI
'''
while len(detectedFaces) == 0:
    status, image = video.read()  #captures an image
    imageGrey = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  #transforms the image to greyscale
    detectedFaces = detectorFace.detectMultiScale(imageGrey, scaleFactor=1.5,minSize=(150, 150))  #detects faces in the image
#print(detectedFaces)
bbox = (detectedFaces[0][0], detectedFaces[0][1], detectedFaces[0][2], detectedFaces[0][3])#we then set the object to be tracked as the face detected
#print(type(bbox))
#print(bbox)
ok = tracker.init(frame, bbox) #initialize the tracker
totalSize = len(frame[0]) #this allows us to see the x-size of the recording
#print(totalSize)
#print(ok)
def cvt(displac):
    return int(((displac + (totalSize/2))*20/totalSize)) #this function converts face position to motor angle, using proportions
qttErrors = 0
while True:
    ok, frame = video.read()
    if not ok: #if video input is over
        break
    ok, bbox = tracker.update(frame)#updates the frame analyzed
    #print(bbox)
    if ok:
        qttErrors = 0
        x,y,w,h = [int(v) for v in bbox]#records the tracked object's (x,y) position, its width w and its heigh h
        ser.write(bytes(str(cvt((2*x+w)/2 - totalSize/2)), 'ascii')) #here we send the angle the head motor should rotate, based on object position
        cv2.rectangle(frame, (x,y), (x+w, y+h), (255,0,0), 2)#draws a rectangle around the object
        #print(ser.readline())
        print(bytes(str(cvt((2*x+w)/2 - totalSize/2)), 'ascii'))
    else:#if it cannot see the object tracked, it shows an error message
        cv2.putText(frame, "Error", (100, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (0,0,255), 2)

        qttErrors += 1 #this variable stores how many consecutive frames resulted in a error while tracking
        if(qttErrors >= 50): #if greater than a certain amount, it restarts the process of selecting something to track
            detectedFaces = ()
            while len(detectedFaces) == 0:
                status, image = video.read()  # captures an image
                imageGrey = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)  # transforms the image to greyscale
                detectedFaces = detectorFace.detectMultiScale(imageGrey, scaleFactor=1.5, minSize=(150, 150))  # detects faces in the image
            # print(detectedFaces)
            bbox = (detectedFaces[0][0], detectedFaces[0][1], detectedFaces[0][2], detectedFaces[0][3])  # we then set the object to be tracked as the face detected
            tracker = cv2.TrackerKCF_create()
            tracker.init(frame, bbox)
            qttErrors = 0
    cv2.imshow('Tracking', frame)#shows the frame
    if cv2.waitKey(1) & 0XFF == 27: #pressing ESC stops code
        break