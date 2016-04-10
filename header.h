#include "Ball.h"
#include <SFML/Graphics.hpp>

#ifndef HEADER_H
#define HEADER_H

#define HEIGHT 700
#define LENGTH 1200
#define PI 3.14159265

 // Prototypes
int circlesColliding(Ball* b1, Ball* b2); // returns true if both balls are in contact
void handleBallCollision(Ball** b1, Ball** b2);   // handles the collision between both balls
void debug(sf::RenderWindow* window, std::vector<Ball*> balls); // displaying center & old center of the balls
float exactCollision(Ball* b1, Ball* b2); // returns the amount of time we need to go backward for an exact collision point
void pool(std::vector<Ball*> balls); // sets up 6 balls pool-like
void bowling(std::vector<Ball*> balls); // sets up 6 balls bowling-like

#endif // HEADER_H
