import os
import sys
from PIL import Image

WIDTH = 64

if len(sys.argv) != 2:
    raise ValueError("Please provide output directory")

path = "frames/"
files = sorted(os.listdir(path))

frame_num = 0
for file in files:
    if file[-4:] != ".png": 
        continue
    if os.path.isfile(os.path.join(path, file)):
        img = Image.open(os.path.join(path, file))
        pix_val = list(img.getdata())
        # flatten pix_val
        data = [x for sets in pix_val for x in sets]
        
        fout = open(sys.argv[1] + "/frame" + str(frame_num), "w")
        for i in range(0, len(data), 4):
            pixel_num = i // 4
            x = pixel_num % WIDTH
            y = pixel_num // WIDTH    
            r = data[i]
            g = data[i + 1]
            b = data[i + 2]
            a = data[i + 3]
            if r + g + b != 0 and a != 0:
                fout.write(str(x) + " " + str(y) + " " + str(r) + " " + str(g) + " " + str(b) + "\n") 
        fout.close()
        frame_num += 1
            
