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

log_tag = root.find("log")
if log_tag == None:
    print("Error! Tag 'log' not found")
    sys.exit(1)
paths = log_tag.find("paths")
if paths == None:
    print("Error! Tag 'paths' not found")
    sys.exit(1)

generations = []
for generation_tag in log_tag.findall("generation"):
    items = []
    items_length = []
    items_danger = []
    for item_log in generation_tag.findall("item"):
        path_summary = item_log.find("pathsummary")
        items_length.append(float(path_summary.get("length")))
        items_danger.append(float(path_summary.get("danger")))
        path_tag = item_log.find("lplevel")
        pathx = []
        pathy = []
        for node in path_tag.findall("node"):
            pathx.append(int(node.get("y")))
            pathy.append(int(node.get("x")))
        items.append((np.asarray(pathx), np.asarray(pathy)))
    generations.append((items, np.array(items_length), np.array(items_danger)))

grid = np.asarray(grid)
backup_grid = grid.copy()

fig = plt.figure(constrained_layout = True)
spec = fig.add_gridspec(ncols = 2, nrows = 3, height_ratios = [1, 10, 1])
ax1 = fig.add_subplot(spec[1, 0])
ax2 = fig.add_subplot(spec[1, 1])
ax_silder = fig.add_subplot(spec[2, :2])
ax_generation_slider = fig.add_subplot(spec[0, :2])

cur_generation = len(generations) - 1
generation_slider = Slider(ax_generation_slider, "Generation", 0, len(generations) - 1, valinit = cur_generation)
slider = Slider(ax_silder, "Path", 0, len(generations[cur_generation][0]) - 1, valinit = 0)

cur_num = 0
grid[generations[cur_generation][0][cur_num][0], generations[cur_generation][0][cur_num][1]] = 2

im = ax1.imshow(grid)
dots, = ax2.plot(generations[cur_generation][1], generations[cur_generation][2], "b.")
red_dot, = ax2.plot(generations[cur_generation][1][cur_num], generations[cur_generation][2][cur_num], "ro")
ax2.set_xlabel("length")
ax2.set_ylabel("danger")
ax2.grid()

def update(val):
    global cur_num
    global cur_generation
    grid[generations[cur_generation][0][cur_num][0], generations[cur_generation][0][cur_num][1]] = backup_grid[generations[cur_generation][0][cur_num][0], generations[cur_generation][0][cur_num][1]]
    cur_num = int(val)
    grid[generations[cur_generation][0][cur_num][0], generations[cur_generation][0][cur_num][1]] = 2
    
    im.set_array(grid)
    red_dot.set_xdata(generations[cur_generation][1][cur_num])
    red_dot.set_ydata(generations[cur_generation][2][cur_num])
    fig.canvas.draw()

def generation_update(val):
    global cur_generation
    global cur_num
    grid[generations[cur_generation][0][cur_num][0], generations[cur_generation][0][cur_num][1]] = backup_grid[generations[cur_generation][0][cur_num][0], generations[cur_generation][0][cur_num][1]]
    cur_generation = int(val)
    cur_num = 0
    slider.valmax = len(generations[cur_generation][0]) - 1
    slider.set_val(0)
    grid[generations[cur_generation][0][cur_num][0], generations[cur_generation][0][cur_num][1]] = 2
    
    im.set_array(grid)
    dots.set_xdata(generations[cur_generation][1])
    dots.set_ydata(generations[cur_generation][2])
    red_dot.set_xdata(generations[cur_generation][1][cur_num])
    red_dot.set_ydata(generations[cur_generation][2][cur_num])
    fig.canvas.draw()
    
slider.on_changed(update)
generation_slider.on_changed(generation_update)
plt.show()