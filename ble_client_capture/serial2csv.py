import serial
import sys

argv = sys.argv[1:]
BAUD_RATE = 9600


if len(argv) == 0:
   print('Please provide a com port number!')
else:
   COM_PORT = 'COM' + argv[0]
   s = serial.Serial(COM_PORT)
   s.timeout = 0 # non-blocking, allow keyboard interrupt to terminate the program
   f = open('0.csv', 'w')

   # print the first header row
   # eg. f.write('aX,aY,aZ,gX,gY,gZ\n')
   f.write('aX,aY,aZ,gX,gY,gZ\n')
   while (True):
      try:
         read = s.readline()
         if read:
            print(read.decode('utf-8'), end="")
            f.write(read.decode('utf-8'))
      except KeyboardInterrupt:
         break