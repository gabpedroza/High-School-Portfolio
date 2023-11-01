import cv2
import easyocr
reader = easyocr.Reader(['en'], gpu=True)
captura = cv2.VideoCapture(1, cv2.CAP_DSHOW)
def reconhecer_letra():
    if (1):  # "if" only to  properly setup identification
        try:
            status, frame = captura.read()  # return status ( true or false) and frame


            frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)  # turns image to gray(black and white)

            cv2.imwrite('img.jpg',
                        frame)  # "writes" the image on folder(needs to be jpg)/ it overwrites the last image
            # cv2.imshow("imagem: ", frame)
            #cv2.imshow("ocr", frame)
            print("ocr rodado")
            #captura.release() #this is necessary to prevent future tracking routines from crashing
            #return
        except:
            print("errogravissimo")
            return 3
        try:
            bounds = reader.readtext('img.jpg')  # reads the text on the saved image saved on the ssd//
            #bounds = ((1, 2))
        except:
            print("erroLeitura")
            return 3

        try:
            print(bounds[0][1])  # tries to print the words within the image//

            print(bounds[0][2])  # attempts to print the percentage of certainty of what's written on the image//
            if (bounds[0][2] <= 0.8 or not (bounds[0][1].upper() in ["PYTHON", "OCR", "RGB",
                                                                      "ESTILO", "HELLO", "ROBOCUP"])):
                print("error" + bounds[0][1])
                return 2  # se a certeza for muito baixa, retorna erro
            return bounds[0][1]  # if everything is ok it returns the recognized word
            captura.release()#to prevent error from tracking
        except:
            return 2
