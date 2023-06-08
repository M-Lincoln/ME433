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
blk = np.array([0 for element in range(len(x))])
print(range(len(x)))
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
convsig = np.array([0 for element in range(len(x))])
for i in x:
    if i>=25 and i<=35:
        convsig[i] = 1
    else:
        convsig[i] = 0

# find the position signal using the convolution function
position = np.convolve(blk,convsig, "same")
position_ulab = np.convolve(blk,convsig, "full")
#print(blk)
#print(type(blk))
#print(convsig)
#print(type(convsig))

# find the maximum value of the convolved function
max_position = max(position)
max_position_ulab = max(position_ulab)
# make an array to store the positions of values from the convolved signal
avg_pos_array = []
avg_pos_array_ulab = []
# record all of the position indices where the maximum convolution value is observed
for i in x:
    if position[i] == max_position:
        avg_pos_array.append(x[i])

y = range(len(position_ulab)) # time array

for i in y:
    if position_ulab[i] == max_position_ulab:
        avg_pos_array_ulab.append(y[i])

# COM approx:
#for j in range(len(blk)):
#    if blk[j] ==1:
#        print(j)
#        if blk[j+1]==1:
#            if blk[j+2]==1:
#                if blk[j+3]==1:
#                    edgeleft=j
#                else:
#                    edgeleft=20
count=0
for j in range(len(blk)-1):
    # if consecutive elements are the same:
    if blk[j]==1 & blk[j]==blk[j+1]:
        count=count+1

    
# compute the position of the line
line_pos = np.average(avg_pos_array)
line_pos_ulab = np.average(avg_pos_array_ulab)
if 40<=line_pos_ulab<=60:
    line_pos_ulab_corr = line_pos_ulab*0.4
elif line_pos_ulab<40:
    line_pos_ulab_corr = line_pos_ulab-30
elif line_pos_ulab ==59:
    line_pos_ulab_corr = line_pos_ulab/2 #when it can't find a line, reads as in the center
else:
    line_pos_ulab_corr = line_pos_ulab*0.6

print("\nThe number of consecutive blk readings is " + str(count) + " pixels.\n")
print("\nThe average position of the line is " + str(line_pos) + " pixels.\n")
print("\nThe average position_ulab_corr of the line is " + str(line_pos_ulab_corr) + " pixels.\n")
print("\nThe average position_ulab of the line is " + str(line_pos_ulab) + " pixels.\n")
# plot the position convolution plot
fig2 = plt.figure(2)
fig2 = plt.plot(x,position,"k*-")
plt.ylabel('Line Present?')
plt.xlabel('position')
# plot the position_ulab convolution plot
fig3 = plt.figure(3)
fig3 = plt.plot(position_ulab,"k*-")
plt.ylabel('Line Present?')
plt.xlabel('position_ulab')
#Plot blk
fig4 = plt.figure(4)
fig4 = plt.plot(blk,"k*-")
plt.ylabel('BLK 0 or 1')
plt.xlabel('position on screen')
plt.show()

# be sure to close the port
ser.close()