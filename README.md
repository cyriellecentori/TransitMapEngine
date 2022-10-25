# Transit Map Engine

This program is an engine that can easily generate public transport maps from a data file and a 1 pixel 1 square map. There is no UI and I don’t plan creating one; feel free to do it if you’re interested.

## Manual

You’ll need two files to generate a transport map: a 1 pixel 1 square image in a PIL compatible format (png for instance) and a YAML file (also works with JSON since YAML is compatible).

### Image
The image will contain the 2D map. One pixel represent one square whose size can be defined in the YAML file. You can then draw your lines on the map with one pixel width. Each pixel defines a square, that you can connect to trace lines. The color of the pixel will define the color of the line. However, there are two special colors you can’t use for lines:
* White, the background color. Every pixel outside of the path of a line must be white.
* Black, the multiline color. Automatically detects surroundings lines to connect them. You can make lines cross by putting a black pixel at their intersection. Note that if you put multiple black pixels next to each other, it will behave weirdly. However, this is mostly the case under large transfer stations or stops that will hide this strange phenomenon, so I’m too lazy to fix it.

### YAML file
The YAML file contains the parameters used to build the final image and a list of every station or stop. Here is a example:
```yaml
scale: [50, 50] # The size (in pixels) of a square (1px in the map image will have this size in the final image).
styles: # A list of different line styles. For now, only the "default" type is available.
    default: # Name of the defined style. A style named "default" must be defined for every line that doesn’t have a specified style.
        type: default # Type of the style. The parameters you’ll have to specify depend of it.
        line width: 0.5 # The width of a line, in fraction of the total square. 1 means the line will take the entire square. You can have values greater than 1 (which means the line will be wider than a square).
        stop size: 1.5 # The radius of a stop circle, in fraction of the total square. Works like above.
    trains: # Example of an other style
        type: default
        line width: 0.8
        stop size: 1.5
        lines: # Is ignored with "default" style, but must be defined for every other style
            - FF0000 # Color of a line to which the style will apply
            - 28FF28 # Color of a line to which the style will apply

font: # Contains font-related data
   path: font.ttf # Path to the font
   size: 50 # Text size
   color: [0, 0, 0] # Color of the text, in the RGB format.
output: final-image.png # Output file
map: map-image.png # Path to the map image (cf. Image section)

stops: # Array containing all stations or stops, see examples next
   - position: [10, 23] # Position in squares (you can report the position of the pixel in the map image on which you want to add the stop). If the stop is large, your position field should look like something like [10, 23, 12, 25] ([x1, y1, x2, y2]).
     type: single # One of the types above
     name: Stadium # The name of the station. You can add another line if you surround the name with "" and separate the two lines with \n
     name-placement: [1, 0] # The position of the name, in squares, relative to the center of the stop. More details next.
     name-placement: [1, 0] # The position of the name, in squares, relative to the center of the stop. If name-origin is not specified, the origin is automatically set (more details below).
     name-origin: [0, 0.3] # (Optional, automatic if not specified) Origin of the name label. The numer entered is multiplied by the width (if x) or height (if y) of the name label. [0,0] means top-left, [1, 1] means bottom-right.
     name-rotation: 0 # (Optional, 0 if not specified) Rotation of the name label around the origin, in degrees. 
```

#### Stops (or stations)
Stops are generally defined by four properties. Their position, their type, their name and its positions. There are five types of stops, some of them having influence on the parameters you need to put:
* single: Simple stops on a line, without any transfer. Represented by a circle without border, with the same color than its line.
* transfer: A one square transfer stop, with several lines crossing under it. Represented by a white circle with a black border.
* terminus: A one square terminus on one line, without any transfer. Represented by a white circle with a black border containing a smaller circle without border and of the same color than the line.
* large: A large transfer stop, with several lines crossing on several squares. It’s a rectangle, so its position array must have four numbers: the origin position and the destination position.
* terminus dot: Not a station alone, but can add the internal circle of the terminus stop to a large stop at its given position. It has no name (you can omit all name-related fields).
A stop has to be coded in this format (the fields can be in any order):
```yaml
position: [10, 23] # Position in squares (you can report the position of the pixel in the map image on which you want to add the stop). If the stop is large, your position field should look like something like [10, 23, 12, 25] ([x1, y1, x2, y2]).
type: single # One of the types above
name: Stadium # The name of the station. You can add another line if you surround the name with "" and separate the two lines with \n
name-placement: [1, 0] # The position of the name, in squares, relative to the center of the stop. If name-origin is not specified, the origin is automatically set (more details below).
name-origin: [0, 0.3] # (Optional, automatic if not specified) Origin of the name label. The numer entered is multiplied by the width (if x) or height (if y) of the name label. [0,0] means top-left, [1, 1] means bottom-right.
name-rotation: 0 # (Optional, 0 if not specified) Rotation of the name label around the origin, in degrees. 
```
The origin can be automatically determined from the name placement, but it can be a bit tricky. At the top of the stop, the anchor will be at the bottom of the text, at the bottom, on the top, and if its next to, then the anchor is in the middle. The same logic is applied to the left-right axis. Only 0 counts as the middle. If you encounter difficulties with the name placement, don’t hesitate to manually set the label’s origin. However, this automatic name origin should be enough for most of your basic stops.

### Generation
When you have these two files ready, you only have to launch the Python script and indicate the path of the YAML file in the first argument.
```
python tme.py file.yml
```

## Development
I consider this program to be mainly finished, but I might add some new features in the future (like rivers and parks for instance). This is not the project I’m mainly focused on though, so don’t expect it coming soon. However, you can submit modifications (improvements, bug fixes, new features) and I might add them to the engine. You can also fork this and create your own engine from it, as long as you respect the licence.
