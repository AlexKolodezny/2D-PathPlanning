import sys
import matplotlib.pyplot as plt
import xml.etree.ElementTree as ET

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
    
for item in paths.findall("item"):
    path = item.find("lplevel")
    for node in path.findall("node"):
        x = int(node.get("x"))
        y = int(node.get("y"))
        grid[y][x] = 2

plt.imshow(grid)
plt.show()