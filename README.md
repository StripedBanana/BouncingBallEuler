# Bouncing balls
This program is a simulation of the behaviour of balls in a closed environment, using the SFML library for graphics purposes.

You can add a ball and give it an initial speed by left clicking and dragging in the window:
![alt tag](http://i.imgur.com/9kmdTgH.gif)

Collision with the walls and balls is handled.

It also handles inelastic collisions and gravity effect:
![alt tag](http://i.imgur.com/F73cq2J.gif)

It uses Euler integration to compute a ball's trajectory, and a quadtree to avoid writing an o(n²) algorithm for collision detection:
![alt tag](http://i.imgur.com/V1E7zL1.gif)
