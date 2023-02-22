#!/usr/bin/env python
import socket
from time import sleep

TCP_IP = '127.0.0.1'
TCP_PORT = 12345
BUFFER_SIZE = 1024

mes1 = "te"
mes2 = "st 1\ntest2\n"
mes3 = "\n"
mes4 = "asdlaskdfjlksadjflskadjflkasdjflkasjdflksajdflkasjdflkjasdlkfjaslkdfjlkasdjflkasdjflkasjdlfkjasdlkfj\nlkjasldkfjlaksdjflkajsdflk\nasdf"
# message = 78


def send_message (message):
    print("messaging the server: " + message)
    bytes_obj = bytes(message, "ASCII")
    s.send(bytes_obj)
    number_of_messages_recivied = 0
    try:
        data = s.recv(BUFFER_SIZE)
        while data:
            number_of_messages_recivied += 1
            print("received data:" + str(data))
            data = s.recv(BUFFER_SIZE)
    except socket.timeout:
        if number_of_messages_recivied > 0:
            print("Message receiving complete " + str(number_of_messages_recivied) + " messages received")
        else:
            print("No data received")


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.settimeout(5)
s.connect((TCP_IP, TCP_PORT))

send_message(mes1)
send_message(mes2)
send_message(mes3)
send_message(mes4)


s.close()
