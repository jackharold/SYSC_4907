import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
#import serial
import time

steering_right_counter = 0
steering_left_counter = 0
car_right_coutner = 0
car_left_coutner = 0
num_checks = 0
start_coutner = 0

def do_canny(frame):
    gray = cv.cvtColor(frame, cv.COLOR_RGB2GRAY)
    blur = cv.GaussianBlur(gray, (5, 5), 0)
    canny = cv.Canny(blur, 100, 200)
    return canny

def calculate_position(frame, lines):
    max_y1 = 0
    max_y2 = 0
    slope = 0
    y_intercept = 0
    for line in lines:
        x1,y1,x2,y2 = line.reshape(4)
        if y2 > max_y2:
            max_y2 = y2
        if y1 > max_y1:
            max_y1 = y1
    print("max_y1: " + str(max_y1) + " max_y2: " + str(max_y2))
    
    if (max_y2 > 350) and (max_y1 > 350):
        return "Car Right"
    elif (max_y2 < 150) and (max_y1 < 150):
        return "Car Left"
    '''
    if (max_y2 - max_y1) > 30:
        return "Steering Left"
    elif (max_y1 - max_y2) > 30:
        return "Steering Right"
    '''
    return "Center"

cap = cv.VideoCapture(0)
#ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
#ser.flush()


while (cap.isOpened()):
    ret, frame = cap.read()
    canny = do_canny(frame)
    cv.imshow("canny", canny)
    #cv.imshow("video", frame)
    #plt.imshow(frame)
    #plt.show()
    hough = cv.HoughLinesP(canny, 2, np.pi / 180, 100, np.array([]), minLineLength = 100, maxLineGap = 50)
    position = calculate_position(frame, hough)
    print(position)
    if cv.waitKey(10) & 0xFF == ord('q'):
        break

cap.release()
cv.destroyAllWindows()
