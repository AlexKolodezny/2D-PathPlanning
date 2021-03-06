import sys
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
from matplotlib.widgets import Slider
import numpy as np

if len(sys.argv) < 2:
    print("Error! Filename is required!")

tree = ET.parse(sys.argv[1])
root = tree.getroot()

map_tag = root.find("map")
if map_tag == None:
    print("Error! Tag 'map' not found")
    sys.exit()

grid_tag = map_tag.find("grid")
if grid_tag == None:
    print("Error! Tag 'grid' not found")
    sys.exit()

grid = []

for row in grid_tag.findall("row"):
    grid.append(list(map(int, row.text.split())))

grid = np.asarray(grid)

tag = map_tag.find("startx")
if tag == None or tag.text == None:
    print("Error! Tag 'startx' not found")
else:
    st_x = int(tag.text)
    tag = map_tag.find("starty")
    if tag == None or tag.text == None:
        print("Error! Tag 'starty' not found")
    else:
        st_y = int(tag.text)
        tag = map_tag.find("finishx")
        if tag == None or tag.text == None:
            print("Error! Tag 'finishx' not found")
        else:
            fn_x = int(tag.text)
            tag = map_tag.find("finishy")
            if tag == None or tag.text == None:
                print("Error! Tag 'finishy' not found")
            else:
                fn_y = int(tag.text)
                grid[st_y][st_x] = 2
                grid[fn_y][fn_x] = 2

fig = plt.figure(constrained_layout = True)
spec = fig.add_gridspec(ncols = 1, nrows = 1)
ax1 = fig.add_subplot(spec[0, 0])

im = ax1.imshow(grid)

plt.show()