import yaml
from PIL import Image, ImageDraw, ImageFont
import sys
import os

def loading_bar(percent, size, character):
	bar = "["
	for i in range(size):
		if (i / size) * 100 <= percent:
			bar += character
		else:
			bar += " "
	return bar + "]"

if len(sys.argv) != 2:
	print("This program takes one argument: the name of the yaml data file used to build the map. No more, no less.")
	exit(1)

data_file = open(sys.argv[1], "r", encoding="utf-8")
data = yaml.safe_load(data_file)
scale = ((data["scale"][0] // 2) * 2, (data["scale"][1] // 2) * 2) # No center square so the center is the real center
map_tab = []
lines = []

print("Loading lines map…")

img_tab = Image.open(data["map"]).convert("RGB")
font = ImageFont.truetype(data["font"]["path"], data["font"]["size"])

print("File loaded, identifying lines…")

for (count, pixel) in img_tab.getcolors():
	if pixel != (0,0,0) and pixel != (255, 255, 255):
		lines.append(pixel)

print(len(lines), "lines detected.")
print("Extracting map data…")

for y in range(img_tab.size[1]):
	map_tab.append([])
	for x in range(img_tab.size[0]):
		percent = round((((img_tab.size[0] * y - 1) + x) / (img_tab.size[1] * img_tab.size[0])) * 100)
		print(loading_bar(percent, os.get_terminal_size()[0] // 4, "#"), f"- {percent:3d} %", end = '\r')
		r,g,b = img_tab.getpixel((x,y))
		n = 0
		if (r,g,b) in lines:
			n = lines.index((r,g,b)) + 1
		elif (r,g,b) == (0,0,0):
			n = -1
		map_tab[y].append(n)

print(loading_bar(100, os.get_terminal_size()[0] // 4, "#"), "- 100 % - Done!")
		
map_size = (len(map_tab[0]), len(map_tab))

image = Image.new(mode="RGB", size=(map_size[0] * scale[0], map_size[1] * scale[1]), color="white")

draw = ImageDraw.Draw(image)

stop_size = data["stop-size"]
stop_radius = ((scale[0] + scale[1]) / 2) * stop_size / 2
center_radius = int(((scale[0] + scale[1]) / 2) * data["width"] / 4) * 2 + 1

def draw_segment(line, x, y, s_x, s_y, s_x_, s_y_):
	draw_half_segment(line, x, y, s_x, s_y)
	draw_half_segment(line, x, y, s_x_, s_y_)
	
def draw_half_segment(line, x, y, s_x, s_y):
	center = [(x + 0.5) * scale[0], (y + 0.5) * scale[1]]
	origin = [center[0] + s_x * 0.5 * scale[0], center[1] + s_y * 0.5 * scale[1]]
	route = []
	route.append((center[0], center[1]))
	route.append((origin[0], origin[1]))
	draw.line(route, fill = lines[line - 1], width= 2 * int(scale[0] * data["width"] / 2) + 3, joint="curve") # Force odd width
	
def draw_circle(x,y,r,img,fill=None,outline=None,width=1):
	img.ellipse([(x-r,y-r),(x+r,y+r)], fill=fill, outline=outline, width=width)	
def draw_terminus_dot(x, y, line):
	draw_circle((x + 0.5) * scale[0], (y + 0.5) * scale[1], stop_radius / 2, draw, fill=line)
def draw_station(x,y, line):
	draw_circle((x + 0.5) * scale[0], (y + 0.5) * scale[1], stop_radius, draw, fill=line) 
def draw_station_transfer(x,y):
	draw_circle((x + 0.5) * scale[0], (y + 0.5) * scale[1], stop_radius, draw, fill=(255, 255, 255), outline=0, width=round(0.1*stop_radius))
def draw_station_large(x, y, x_, y_):
	draw.rounded_rectangle((x * scale[0] - ((stop_radius - scale[0] / 2) / 0.7), y * scale[1] - ((stop_radius - scale[1] / 2) / 0.7), x_ * scale[0] + ((stop_radius - scale[0] / 2) / 0.7), y_ * scale[1] + ((stop_radius - scale[1] / 2) / 0.7)), radius=stop_radius, fill=(255, 255, 255), outline=0, width=round(0.1*stop_radius))
def draw_station_terminus(x, y, line):
	draw_station_transfer(x, y)
	draw_terminus_dot(x, y, line)


def oddeven_sort(tab):
	ntab = []
	for t in tab:
		if (t // 2) * 2 == t:
			ntab.append(t)
		else:
			ntab.insert(0, t)
	return ntab

# Neighbors for forbidding too close connections
nb = [(1,3), (0, 2), (1, 5), (0, 6), (), (2, 8), (3, 7), (6, 8), (5, 7)]

print("Generating map…")

for x in range(map_size[0]):
	for y in range(map_size[1]):
		percent = round((((map_size[0] * x - 1) + y) / (map_size[1] * map_size[0])) * 100)
		print(loading_bar(percent, os.get_terminal_size()[0] // 4, "#"), f"- {percent:3d} %", end = '\r')
		line = map_tab[y][x]
		if x > 0 and y > 0 and x < map_size[0] - 1 and y < map_size[1] - 1 and line != 0:
			coord_same = []
			if line != -1:
				draw_circle((x + 0.5) * scale[0], (y + 0.5) * scale[1], center_radius, draw, fill=lines[line - 1])
			coorlock = {} # Locks possibilities so there is at least 120° between each line of a same color
			for c in [1, 3, 5, 7, 0, 2, 8]: # No center (4)
				s_x,s_y = c // 3 - 1,c % 3 - 1
				for d in oddeven_sort(range(8 - c)):
					c_ = 8 - d
					if c_ != 4: # Not center
						s_x_,s_y_ = c_ // 3 - 1,c_ % 3 - 1
						# Might be too complicated for what it does, but I’m too lazy to check it
						if (((map_tab[y + s_y][x + s_x] == map_tab[y + s_y_][x + s_x_]) and line == map_tab[y + s_y_][x + s_x_]) or ((map_tab[y + s_y][x + s_x] == -1 and map_tab[y + s_y_][x + s_x_] == line) or (map_tab[y + s_y_][x + s_x_] == -1 and map_tab[y + s_y][x + s_x] == line))) and ( ((not c in coorlock.get(line)) and (not c_ in coorlock.get(line))) if (line in coorlock.keys()) else True): # Draw a segment if two are connected and they are of the same line or if one of the same line is connected with -1 and not too close from existing connection
							draw_segment(line, x, y, s_x, s_y, s_x_, s_y_)
							if not line in coorlock.keys():
								coorlock[line] = []
							coorlock[line] += nb[c]
							coorlock[line] += nb[c_]
							
						elif line == -1 and map_tab[y + s_y][x + s_x] == map_tab[y + s_y_][x + s_x_] and map_tab[y + s_y][x + s_x] != 0 and ( ((not c in coorlock.get(map_tab[y + s_y][x + s_x])) and (not c_ in coorlock.get(map_tab[y + s_y][x + s_x]))) if (map_tab[y + s_y][x + s_x] in coorlock.keys()) else True): # Connect everything if line -1 and not too close to existing connection
							draw_segment(map_tab[y + s_y][x + s_x], x, y, s_x, s_y, s_x_, s_y_)
							if not map_tab[y + s_y][x + s_x] in coorlock.keys():
								coorlock[map_tab[y + s_y][x + s_x]] = []
							coorlock[map_tab[y + s_y][x + s_x]] += nb[c]
							coorlock[map_tab[y + s_y][x + s_x]] += nb[c_]
				if map_tab[y + s_y][x + s_x] == line or map_tab[y + s_y][x + s_x] == -1: # Lists all adjacent tiles of the same line for when there is only one
					coord_same.append((s_x, s_y))
			if len(coord_same) == 1:
				draw_half_segment(line, x, y, coord_same[0][0], coord_same[0][1])
print(loading_bar(100, os.get_terminal_size()[0] // 4, "#"), "- 100 % - Done!")
print("Adding stops…")
stops = data["stops"]
itor = 0
for stop in stops:
	percent = round((itor / len(stops)) * 100)
	itor += 1
	print(loading_bar(percent, os.get_terminal_size()[0] // 4, "#"), f"- {percent:3d} %", end = '\r')
	stop_type = stop["type"]
	stop_x = stop["position"][0]
	stop_y = stop["position"][1]
	stop_line = map_tab[stop_y][stop_x]
	if stop_type == "terminus":
		draw_station_terminus(stop_x, stop_y, lines[stop_line - 1])
	elif stop_type == "transfer":
		draw_station_transfer(stop_x, stop_y)
	elif stop_type == "large":
		draw_station_large(stop_x, stop_y, stop["position"][2], stop["position"][3])
	elif stop_type == "terminus dot":
		draw_terminus_dot(stop_x, stop_y, lines[stop_line - 1])
	else: # "single" by default
		draw_station(stop_x, stop_y, lines[stop_line - 1])
	if stop_type != "terminus dot":
		text_coord = ((stop_x + 0.5 + stop["name-placement"][0]) * scale[0], (stop_y + 0.5 + stop["name-placement"][1]) * scale[1])
		anchor = ""
		align = ""
		if stop["name-placement"][0] < 0:
			anchor = "r"
			align = "right"
		elif stop["name-placement"][0] > 0:
			anchor = "l"
			align = "left"
		else:
			anchor = "m"
			align = "center"
		if stop["name-placement"][1] < 0:
			anchor += "d"
		elif stop["name-placement"][1] > 0:
			anchor += "a"
		else:
			anchor += "m"
		draw.text(text_coord, stop["name"], fill=tuple(data["font"]["color"]), anchor=anchor, align=align, font=font)

print(loading_bar(100, os.get_terminal_size()[0] // 4, "#"), "- 100 % - Done!")
print("Map generated! Saving…")
image.save(data["output"])
print("Saved!")
