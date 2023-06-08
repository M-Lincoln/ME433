# requires adafruit_ov7670.mpy and adafruit_st7735r.mpy in the lib folder
import time
from displayio import (
    Bitmap,
    Group,
    TileGrid,
    FourWire,
    release_displays,
    ColorConverter,
    Colorspace,
    Palette,
)
from adafruit_st7735r import ST7735R
import board
import busio
import digitalio
from adafruit_ov7670 import (
    OV7670,
    OV7670_SIZE_DIV1,
    OV7670_SIZE_DIV8,
    OV7670_SIZE_DIV16,
)
from ulab import numpy as np

release_displays()
spi = busio.SPI(clock=board.GP2, MOSI=board.GP3)
uart = busio.UART(board.GP4, board.GP5, baudrate=9600) #tx pin, rx pin
display_bus = FourWire(spi, command=board.GP0, chip_select=board.GP1, reset=None)
display = ST7735R(display_bus, width=160, height=128, rotation=90) 

# Ensure the camera is shut down, so that it releases the SDA/SCL lines,
# then create the configuration I2C bus

with digitalio.DigitalInOut(board.GP10) as reset:
    reset.switch_to_output(False)
    time.sleep(0.001)
    bus = busio.I2C(board.GP9, board.GP8) #GP9 is SCL, GP8 is SDA

# Set up the camera (you must customize this for your board!)
cam = OV7670(
    bus,
    data_pins=[
        board.GP12,
        board.GP13,
        board.GP14,
        board.GP15,
        board.GP16,
        board.GP17,
        board.GP18,
        board.GP19,
    ],  # [16]     [org] etc
    clock=board.GP11,  # [15]     [blk]
    vsync=board.GP7,  # [10]     [brn]
    href=board.GP21,  # [27/o14] [red]
    mclk=board.GP20,  # [16/o15]
    shutdown=None,
    reset=board.GP10,
)  # [14]

width = display.width
height = display.height

bitmap = None
# Select the biggest size for which we can allocate a bitmap successfully, and
# which is not bigger than the display
for size in range(OV7670_SIZE_DIV1, OV7670_SIZE_DIV16 + 1):
    #cam.size = size # for 4Hz
    #cam.size = OV7670_SIZE_DIV16 # for 30x40, 9Hz
    cam.size = OV7670_SIZE_DIV8 # for 60x80, 9Hz
    if cam.width > width:
        continue
    if cam.height > height:
        continue
    try:
        bitmap = Bitmap(cam.width, cam.height, 65536)
        break
    except MemoryError:
        continue

print(width, height, cam.width, cam.height)
if bitmap is None:
    raise SystemExit("Could not allocate a bitmap")
time.sleep(4)
g = Group(scale=1, x=(width - cam.width) // 2, y=(height - cam.height) // 2)
tg = TileGrid(
    bitmap, pixel_shader=ColorConverter(input_colorspace=Colorspace.BGR565_SWAPPED)
)
g.append(tg)
display.show(g)

t0 = time.monotonic_ns()
display.auto_refresh = False

# arrays to store the color data
reds = np.zeros(60,dtype=np.uint16) # count the number of red pixels
greens = np.zeros(60,dtype=np.uint16)
blues = np.zeros(60,dtype=np.uint16)
bright = np.zeros(60) # sum of the rgb
while True:
    cam.capture(bitmap) # take a pic
    #bitmap[10,10] = 0 # set a pixel to black
    #bitmap[10,20] = 0 # [Y,X], [0,0] is bottom left
    
    # colors: (decompose it into rgb)
    #0x1F ->   0b0000000000011111 # all green
    #0x7E0 ->  0b0000011111100000 # all red
    #0xF800 -> 0b1111100000000000 # all blue
    
    # wait for a newline from the computer
    input()
    row = 40 # which row to send to the computer (right in the middle of the image)
    # draw a red dot above the row, in the middle of the screen
    bitmap[row+1,30] = 0x3F<<5
    # force some colors to test the bits
    #for i in range(0,20):
    #    bitmap[row,i] = 0xF800 # blue
    #for i in range(20,40):
    #    bitmap[row,i] = 0x1F # green
    #for i in range(40,60):
    #    bitmap[row,i] = 0x7E0 # red
    # calculate the colors
    for i in range(0,60):
        reds[i] = ((bitmap[row,i]>>5)&0x3F)/0x3F*100 # loop through all of the rows and find how much rgb and how bright it is
        greens[i] = ((bitmap[row,i])&0x1F)/0x1F*100
        blues[i] = (bitmap[row,i]>>11)/0x1F*100
        bright[i] = reds[i]+greens[i]+blues[i]

    # CODE TO FIND THE CENTER OF THE LINE:
    #for i in range(0,60):
    #    if (reds[i]>50 and blues[i]>50):
    #        bitmap[row,i] = 0xFFFF
    #    else:
    #        bitmap[row,i] = 0x0000
    # print the raw pixel value to the computer
    # make an array of binary values, 1 = line present, 0 = line not present
    x = range(len(reds)) # time array
    blk = np.array([0 for element in range(len(x))])
    #print(range(len(x)))
    #print(blk)
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
    position = np.convolve(blk,convsig)

    # find the maximum value of the convolved function
    max_position = max(position)

    # make an array to store the positions of values from the convolved signal
    avg_pos_array = []

    # record all of the position indices where the maximum convolution value is observed
    y = range(len(position)) # time array
    for i in y:
        if position[i] == max_position:
            avg_pos_array.append(y[i])
    #for i in x:
    #    if position[i] == max_position:
    #        avg_pos_array.append(x[i])

    # compute the position of the line
    line_pos = np.mean(avg_pos_array)
    if 40<=line_pos<=60:
        line_pos_ulab_corr = line_pos*0.4
    elif line_pos<40:
        line_pos_ulab_corr = line_pos-30
        if line_pos_ulab_corr<0:
            line_pos_ulab_corr=0
    elif line_pos ==59:
        line_pos_ulab_corr = line_pos/2 #when it can't find a line, reads as in the center
    else:
        line_pos_ulab_corr = line_pos*0.6
    print("\nThe average position of the line is " + str(line_pos_ulab_corr) + " pixels.\n")
    # print the line position over UART to the PIC
    # in while True: after reading an image and finding the line
    value_str = str(line_pos_ulab_corr)+'\n'
    uart.write(value_str.encode())
    #for i in range(0,60):
    #    print(str(i)+" "+str(bitmap[row,i])) # print it back to the computer so you can see it in a list

    bitmap.dirty() # updae the image on the screen
    display.refresh(minimum_frames_per_second=0)
    t1 = time.monotonic_ns()
    #print("fps", 1e9 / (t1 - t0))
    t0 = t1