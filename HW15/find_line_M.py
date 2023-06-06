# get a line of raw bitmap and plot the components
import serial
import numpy as np
ser = serial.Serial('COM4',230400) # the name of your Pico port
print('Opening port: ')
print(ser.name)

ser.write(b'hi\r\n') # send a newline to request data
data_read = ser.read_until(b'\n',50) # read the echo

sampnum = 0
index = 0
raw = []
reds = []
greens = []
blues = []
bright = []

# Pico sends back index and raw pixel value
while sampnum < 60: # width of bitmap
    data_read = ser.read_until(b'\n',50) # read until newline
    data_text = str(data_read,'utf-8') # convert bytes to string
    data = list(map(int,data_text.split())) # convert string to values

    if(len(data)==2):
        index = data[0]
        raw.append(data[1])
        reds.append(((data[1]>>5)&0x3F)/0x3F*100) # red value is middle 6 bits
        greens.append((data[1]&0x1F)/0x1F*100) # green value is rightmost 5 bits
        blues.append(((data[1]>>11)&0x1F)/0x1F*100) # blue vale is leftmost 5 bits
        bright.append((data[1]&0x1F)+((data[1]>>5)&0x3F)+((data[1]>>11)&0x1F)) # sum of colors
        sampnum = sampnum + 1

# print the raw color as a 16bit binary to double check bitshifting
for i in range(len(reds)):
    print(f"{raw[i]:#018b}")

# plot the colors 
import matplotlib.pyplot as plt 
x = range(len(reds)) # time array
fig1 = plt.figure(1)
fig1 = plt.plot(x,reds,'r*-',x,greens,'g*-',x,blues,'b*-')
plt.ylabel('color')
plt.xlabel('position')


# make an array of binary values, 1 = line present, 0 = line not present
blk = [0 for element in range(len(x))]
print(range(len(x)))
print(blk)
for i in x:
    if(
        reds[i]<90 and
        reds[i]>60 and
        greens[i]<90 and
        greens[i]>40 and
        blues[i]<80 and
        blues[i]>60
    ):
        blk[i] = 1
    else:
        blk[i] = 0

# make a square wave to convolve with the noisy signal
convsig = [0 for element in range(len(x))]
for i in x:
    if i>=25 and i<=35:
        convsig[i] = 1
    else:
        convsig[i] = 0

# find the position signal using the convolution function
position = np.convolve(blk,convsig, "same")

# find the maximum value of the convolved function
max_position = max(position)

# make an array to store the positions of values from the convolved signal
avg_pos_array = []

# record all of the position indices where the maximum convolution value is observed
for i in x:
    if position[i] == max_position:
        avg_pos_array.append(x[i])

# compute the position of the line
line_pos = np.average(avg_pos_array)
print("\nThe average position of the line is " + str(line_pos) + " pixels.\n")

# plot the position convolution plot
fig2 = plt.figure(2)
fig2 = plt.plot(x,position,"k*-")
plt.ylabel('Line Present?')
plt.xlabel('position')
plt.show()

# be sure to close the port
ser.close()