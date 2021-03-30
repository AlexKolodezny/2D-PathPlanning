import xml.etree.ElementTree as ET
import sys
import matplotlib.pyplot as plt

def parse_xml(file):
    paths_length = []
    paths_danger = []

    tree = ET.parse(file)
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
    return paths_length, paths_danger


if len(sys.argv) < 2:
    print("Error! Filename is required!")

paths_length1, paths_danger1 = parse_xml(sys.argv[1])
plt.plot(paths_length1, paths_danger1, "b.")

if len(sys.argv) == 3:
    paths_length2, paths_danger2 = parse_xml(sys.argv[2])
    plt.plot(paths_length2, paths_danger2, "r.")

plt.grid()
plt.ylabel("danger")
plt.xlabel("legnth")
plt.show()