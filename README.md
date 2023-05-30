# woflenstein-style-rendering

This is a small project for me to learn more about how the world was rendered in Wolfenstein 3D.

To cast the rays in this program I check for horizontal and vertical walls individually and then find the shortest distance to a vertical or horizontal wall for a given ray.  
This ray is then used as the distance for selecting the height of the wall, and is also used to map the texture onto the wall.

The map is loaded from a simple PNG file, and the dimensions of the map are defined at the top of main.cpp. Similarly, the size of the window is defined at the top of main.cpp,
and changing these will correctly fill the screen with the 3D view.
