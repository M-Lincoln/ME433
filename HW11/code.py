print("Hello World!")
import time
from ulab import numpy as np # to get access to ulab for numpy functions

length = 100

a = [] # make an empty array for real numbers in FFT


t = np.linspace(0.0, 100.0, num = 1024) # time at 20s. array length = 1024

y = (2.0 * np.sin(2 * np.pi * 2.3 * t) + 2.5 + 2.0 * np.sin(2 * np.pi * 10 * t) + 2.5 + 2.0 * np.sin(2 * np.pi * 4 * t) + 2.5)# 2.3Hz
a, b = np.fft.fft(y)
i = 0
while i in range(len(t)):
    # emit 1 numbers at a time
    time.sleep(0.05) # delay 0.05s
    # print((y[i], y[i+1], y[i+2]))
    #print((a[i], a[i+1], a[i+2]))
    print((a[i],))
    #i = i+3 # increment up by 3
    i = i+1


# print(t)
# print(len(t))
# b = np.zeros(len(t)) # make an empty array for imaginary numbers in FFT
# Create 3 different sine waves:
# for i in range(len(t)):
    #print(i)
    #print(2.0 * np.sin(2 * np.pi * 2.3 * t[i]) + 2.5 + 2.0 * np.sin(2 * np.pi * 10 * t[i]) + 2.5 + 2.0 * np.sin(2 * np.pi * 4 * t[i]) + 2.5)
    # a.append(2.0 * np.sin(2 * np.pi * 2.3 * t[i]) + 2.5 + 2.0 * np.sin(2 * np.pi * 10 * t[i]) + 2.5 + 2.0 * np.sin(2 * np.pi * 4 * t[i]) + 2.5)# 2.3Hz
     # 10Hz
     # 4Hz
# print(a)

# FFT of this array:
#Fs = 100 # sample rate
#Ts = 1.0/Fs; # sampling interval
#ts = np.arange(0,t[-1],Ts) # time vector
#y = a # the data to make the fft from
#n = len(y) # length of the signal
#k = np.arange(n)
#T = n/Fs
#frq = k/T # two sides frequency range
#frq = frq[range(int(n/2))] # one side frequency range
#Y, Im = np.fft.fft(y)/n # fft computing and normalization
#Y = Y[range(int(n/2))]

#fig, (ax1, ax2) = plt.subplots(2, 1)
#ax1.plot(t,y,'b')
#ax1.set_xlabel('Time')
#ax1.set_ylabel('Amplitude')
#ax2.loglog(frq,abs(Y),'b') # plotting the fft
#ax2.set_xlabel('Freq (Hz)')
#ax2.set_ylabel('|Y(freq)|')
#plt.show()
