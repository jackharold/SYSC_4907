import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
import serial
import time

# unit = 4 inches

PATH_1_DISTANCE = 40
PATH_2_DISTANCE = 5
PATH_3_DISTANCE = 5
PATH_4_DISTANCE = 5
PATH_5_DISTANCE = 5

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

def check_camera():

    car_right_counter = 0
    car_left_counter = 0
    num_checks = 0

    cap = cv.VideoCapture(0)

    while (num_checks < 10) :
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
    elif car_left_counter >= 5:
        send_command("<reposition, 2>")

    cap.release()
    cv.destroyAllWindows()

def check_ultrasonic():
    response = send_command("<ultrasonic, 1>")
    return True

def check_gps(path, distance):
    if path == 1 and distance >= PATH_1_DISTANCE:
        path = 6
    elif path == 2 and distance >= PATH_2_DISTANCE:
        response = send_command("<rotate, 45>")
        path = 5
    elif path == 3 and distance >= PATH_3_DISTANCE:
        response = send_command("<rotate, 90>")
        path = 4
    elif path == 4 and distance >= PATH_4_DISTANCE:
        path = 5
    elif path == 5 and distance >= PATH_5_DISTANCE:
        path = 6
    return path

def reset_to_start(path, distance):
    response = send_command("<stop, 1>")
    if path == 1:
        response = send_command("<rotate, 180>")
        response = send_command("<drive, " + str(distance) + ">")
        response = send_command("<rotate, 135>")
        path = 2
    elif path == 2:
        response = send_command("<rotate, 180>")
        response = send_command("<drive, " + str(distance) + ">")
        response = send_command("<rotate, 135>")
        path = 3
    elif path == 3:
        response = send_command("<rotate, 180>")
        response = send_command("<drive, " + str(distance) + ">")
        response = send_command("<rotate, 270>")
        path = 1
    elif path == 4:
        response = send_command("<rotate, 180>")
        response = send_command("<drive, " + str(distance) + ">")
        response = send_command("<rotate, 270>")
        response = send_command("<drive, " + str(distance) + ">")
        response = send_command("<rotate, 270>")
        path = 1
    elif path == 5:
        response = send_command("<rotate, 180>")
        response = send_command("<drive, " + str(distance) + ">")
        response = send_command("<rotate, 315>")
        response = send_command("<drive, " + str(distance) + ">")
        response = send_command("<rotate, 135>")
    return path

def send_command(command):
    print("command: " + command)
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=30)
    ser.write(command.encode('utf-8'))
    time.sleep(.1)
    response = ser.readline().decode('utf-8').rstrip()
    print("response: " + response)
    return response

def check_ir_sensor(ser):
    response = send_comamnd("<infrared, 1>")
    if bool(response):
        return True
    return False

def main():
    '''
    path = 1
    while (path != 6):
        if check_ir_sensor():
            if check_ultrasonic():
                check_camera()
                path = check_gps(path)
                send_command("<drive, 1>")
            else:
                path = reset_to_start(path, distance)
        else:
            path = reset_to_start(path, distance)
    '''
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.flushInput()
    ser.flushOutput()
    ser.write("<stop, 1>".encode('utf-8'))
    time.sleep(.1)
    response = ser.readline().decode('utf-8').rstrip()
    #check_ultrasonic()
    send_command("<stop, 1>")
    path = 0
    if not send_command("<ultrasonic, 1>") == "True":
        path = 3
        #send_command("<rotate, 315>")
    elif not send_command("<ultrasonic, 2>") == "True":
        path = 2
    elif not send_command("<ultrasonic, 3>") == "True":
        path = 1
        send_command("<rotate, 45>")

    if path == 1:
        distance = 1
        while path != 6:
            check_camera()
            path = check_gps(path, distance)
            send_command("<drive, 1>")
            distance += 1
    send_command("<stop, 1>")
    '''
    path = 1
    distance = 1
    while path != 6:
        check_camera()
        path = check_gps(path, distance)
        send_command("<drive, 1>")
        distance += 1
        
    send_command("<stop, 1>")'''

if __name__ == "__main__":
    main()


