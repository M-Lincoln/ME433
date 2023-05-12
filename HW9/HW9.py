# HW9.py
import csv
import matplotlib.pyplot as plt
import numpy as np

'''
Load in the .csv data using code from "python_csv.py"
'''
#with open("python_csv.py") as python_csv:
#    exec(python_csv.read())
t = [] # column 0
data1 = [] # column 1
# data2 = [] # column 2


with open('sigC.csv') as f:
    # open the csv file
    reader = csv.reader(f) # will loop through every row in your file
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column
        # data2.append(float(row[2])) # third column
        
## Mads add: 
#data_Length = len(t)
#for i in range(data_Length):
#    # print the data to verify it was read
#    # print(str(t[i]) + ", " + str(data1[i]) + ", " + str(data2[i]))
#    print(str(t[i]) + ", " + str(data1[i]))

'''
Calculate the sample rate of the collected data
Sample rate = (number of data points) / (total time of samples)
dt = data_Length/t[data_Length];

Where:
    dt = sample rate
    data_Length is defined in python_csv.py
    t = list of time values defined in python_csv.py
'''
#dt = data_Length/t[data_Length-1]; # data_Length is defined in python_csv.py

'''
low-pass filter with a Moving Average Filter
'''
X = 5 # number of cycles you want to average your data over
filter_output = [] #create blank array that is the output of my filter
window = [] # create blank array that stores the values to take the average over
for i in range(len(data1)): # loop through the raw data (datapoints in data1,)
    #print(i)
    if len(filter_output)<X+1:
        filter_output.append(data1[i]) # first X times through, fill the array with the raw data values
    else:
        window = [] # reset window back to blank
        for j in range(X):
            #print('j = '+ str(j))
            window.append(data1[i-X+j+1])
        filter_output.append(np.mean(window))
print('length data1 = ' + str(len(data1)))
#and plot....
#Plot your data:Plot UNFILTERED signal vs time and plot UNFILTERED FFT
#Plot FILTERED signal vs time and plot FILTERED FFT
#from "python_fft.py":
dt = t[1]-t[0]
Fs = 1/dt # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y1 = data1 # the data to make the fft from
y2 = filter_output # the IIR filtered data
n = len(y1) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y1 = np.fft.fft(y1)/n # fft computing and normalization
Y1 = Y1[range(int(n/2))]
Y2 = np.fft.fft(y2)/n # fft computing and normalization
Y2 = Y2[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y1,'k',t,y2,'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax1.title.set_text('SigC, ' + str(X) + ' data points averaged')
ax1.legend(['Raw','FIR (MAF) Filter'])
ax2.loglog(frq,abs(Y1),'k',frq,abs(Y2),'r') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()


'''
low-pass filter with an IIR filter
And PLOT
'''
#filter_output = [] #create blank array that is the output of my filter
## define weights (A+B = 1):
#A = 0.5 # weight to multiply to old_estimate
#B = 0.5 # weight to multiply to raw signal
#for dp in data1: # loop through the raw data (datapoints in data1,)
#    if len(filter_output)==0:
#        filter_output.append(0) # first time through, fill the array with a zero
#    else:
#        filter_output.append(filter_output[-1]*A+dp*B) # [-1] element = previous value of filter_output

##print('length data1 = ' + str(len(data1)))
##print('length filter_output = ' + str(len(filter_output)))

##Plot UNFILTERED signal vs time and plot UNFILTERED FFT
##Plot FILTERED signal vs time and plot FILTERED FFT
##from "python_fft.py":

##Plot your data:
#dt = t[1]-t[0]
#Fs = 1/dt # sample rate
#Ts = 1.0/Fs; # sampling interval
#ts = np.arange(0,t[-1],Ts) # time vector
#y1 = data1 # the data to make the fft from
#y2 = filter_output # the IIR filtered data
#n = len(y1) # length of the signal
#k = np.arange(n)
#T = n/Fs
#frq = k/T # two sides frequency range
#frq = frq[range(int(n/2))] # one side frequency range
#Y1 = np.fft.fft(y1)/n # fft computing and normalization
#Y1 = Y1[range(int(n/2))]
#Y2 = np.fft.fft(y2)/n # fft computing and normalization
#Y2 = Y2[range(int(n/2))]

#fig, (ax1, ax2) = plt.subplots(2, 1)
#ax1.plot(t,y1,'k',t,y2,'r')
#ax1.set_xlabel('Time')
#ax1.set_ylabel('Amplitude')
#ax1.title.set_text('SigC, Weights: A = ' + str(A) + ', B = ' + str(B))
#ax1.legend(['Raw','IIR Filter'])
#ax2.loglog(frq,abs(Y1),'k',frq,abs(Y2),'r') # plotting the fft
#ax2.set_xlabel('Freq (Hz)')
#ax2.set_ylabel('|Y(freq)|')
#plt.show()