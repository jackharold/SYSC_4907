import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import serial
import time

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
    if lines is None:
        return "Car Left"
    for line in lines:
        x1,y1,x2,y2 = line.reshape(4)
        if y2 > max_y2:
            max_y2 = y2
        if y1 > max_y1:
            max_y1 = y1
    #print("max_y1: " + str(max_y1) + " max_y2: " + str(max_y2))
    
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

def check_camera():

    car_right_counter = 0
    car_left_counter = 0
    num_checks = 0

    cap = cv.VideoCapture(0)

    while (cap.isOpened() and num_checks < 10) :
        ret, frame = cap.read()
        canny = do_canny(frame)
        #cv.imshow("canny", canny)
        #cv.imshow("video", frame)
        #plt.imshow(frame)
        #plt.show()
        hough = cv.HoughLinesP(canny, 2, np.pi / 180, 100, np.array([]), minLineLength = 100, maxLineGap = 50)
        position = calculate_position(frame, hough)
        if position == "Car Right":
            car_right_counter += 1
        elif position == "Car Left":
            car_left_counter += 1
        num_checks += 1

    if car_right_counter >= 5:
        send_command("<reposition, 1>")
        return True
    elif car_left_counter >= 5:
        send_command("<reposition, 2>")
        return True

    cap.release()
    cv.destroyAllWindows()
    return False

def check_ultrasonic():
    response = send_command("<ultrasonic, 1>")
    return True

def check_aoa(state):
    angle = int(send_command("<aoa, 1>"))
    if state == 0 and angle > 80:
        state = 1
    elif state == 1 and angle < 20:
        state = 2
    print("STATE IS NOW " + str(state))
    return state
    

def send_command(command):
    print("command: " + command)
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=45)
    ser.flushInput()
    ser.flushOutput()
    ser.flush()
    time.sleep(.1)
    ser.write(command.encode('utf-8'))
    time.sleep(.1)
    response = ser.readline().decode('utf-8').rstrip()
    print("response: " + response)
    return response

def check_ir_sensor():
    if send_command("<infrared, 1>") == "True":
        return True
    return False

def check_ultrasonic():
    if send_command("<ultrasonic, 1>") == "True":
        return True
    return False
    

def main():
    send_command("<stop, 1>")
    send_command("<park, 1>")
    send_command("<stop, 1>")


if __name__ == "__main__":
    main()


