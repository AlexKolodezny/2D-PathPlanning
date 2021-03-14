import sys
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET
from matplotlib.widgets import Slider

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

for log in root.findall("log"):
    paths = log.find("paths")
    if paths != None:
        break
else:
    print("Error! Tag 'paths' not found!")
    sys.exit(1)

lppaths = []

paths_length = []
paths_danger = []
    
for item in paths.findall("item"):
    path_summary = item.find("pathsummary")
    paths_length.append(float(path_summary.get("length")))
    paths_danger.append(float(path_summary.get("danger")))
    path_tag = item.find("lplevel")
    path = []
    for node in path_tag.findall("node"):
        path.append((int(node.get("y")), int(node.get("x"))))
    lppaths.append(path)

fig = plt.figure(constrained_layout = True)
spec = fig.add_gridspec(ncols = 2, nrows = 2, height_ratios = [10, 1])
ax1 = fig.add_subplot(spec[0, 0])
ax2 = fig.add_subplot(spec[0, 1])
ax_silder = fig.add_subplot(spec[1, :])

slider = Slider(ax_silder, "Path", 0, 1, valinit = 0)

cur_num = 0
for coor in lppaths[cur_num]:
    grid[coor[0]][coor[1]] = 2

im = ax1.imshow(grid)
ax2.plot(paths_length, paths_danger, "b.")
l, = ax2.plot(paths_length[cur_num], paths_danger[cur_num], "ro")
ax2.grid()

def update(val):
    global cur_num
    for coor in lppaths[cur_num]:
        grid[coor[0]][coor[1]] = 0
    cur_num = int(val * (len(lppaths) - 1))
    for coor in lppaths[cur_num]:
        grid[coor[0]][coor[1]] = 2
    
    im.set_array(grid)
    l.set_xdata(paths_length[cur_num])
    l.set_ydata(paths_danger[cur_num])
    fig.canvas.draw()
    
slider.on_changed(update)
plt.show()