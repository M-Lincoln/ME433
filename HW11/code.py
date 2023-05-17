print("Hello World!")
import matplotlib.pyplot as plt
import numpy as np
a = [] # make an empty array

t = np.linspace(0.0, 100.0, 1024) # time at 20s. array length = 1024
# print(t)
# print(len(t))
# Create 3 different sine waves:
for i in range(len(t)):
    #print(i)
    #print(2.0 * np.sin(2 * np.pi * 2.3 * t[i]) + 2.5 + 2.0 * np.sin(2 * np.pi * 10 * t[i]) + 2.5 + 2.0 * np.sin(2 * np.pi * 4 * t[i]) + 2.5)
    a.append(2.0 * np.sin(2 * np.pi * 2.3 * t[i]) + 2.5 + 2.0 * np.sin(2 * np.pi * 10 * t[i]) + 2.5 + 2.0 * np.sin(2 * np.pi * 4 * t[i]) + 2.5)# 2.3Hz
     # 10Hz
     # 4Hz
print(a)
# FFT of this array:
Fs = 100 # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = a # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(t,y,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(frq,abs(Y),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()