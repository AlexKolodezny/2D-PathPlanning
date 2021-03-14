import xml.etree.ElementTree as ET
import sys
import matplotlib.pyplot as plt

if len(sys.argv) < 2:
    print("Error! Filename is required!")

paths_length = []
paths_danger = []

tree = ET.parse(sys.argv[1])
root = tree.getroot()

for log in root.findall("log"):
    paths = log.find("paths")
    if paths != None:
        break
else:
    print("Error! Tag 'paths' not found!")
    sys.exit(1)
    
for item in paths.findall("item"):
    path_summary = item.find("pathsummary")
    paths_length.append(float(path_summary.get("length")))
    paths_danger.append(float(path_summary.get("danger")))

if len(paths_danger) == 0:
    print("Path not found!")
    sys.exit(1)

plt.plot(paths_length, paths_danger, "b.")
plt.grid()
plt.ylabel("danger")
plt.xlabel("legnth")
plt.show()