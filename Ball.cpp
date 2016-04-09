#include "Ball.h"
#include "header.h"
#include <iostream>

using namespace std;

Ball::Ball()
{
    setXY(0,0);
    setSpeedXY(0,0);
    setAccXY(0,0);
    setColor(sf::Color::Red);
    ballShape.setRadius(10);
    this->radius = 10;
    this->mass = 1;
}

Ball::Ball(float x, float y, float sx, float sy, float accx, float accy, float radius, float mass, sf::Color color)
{
    setXY(x,y);
    setSpeedXY(sx,sy);
    setAccXY(accx,accy);
    setColor(color);
    ballShape.setRadius(radius);
    this->radius = radius;
    this->mass = mass;
}

void Ball::updatePos(float step)
{
    // Euler
    this->pos.x += this->speed.x*step;
    this->pos.y += this->speed.y*step;

    ballShape.setPosition(pos);

    this->speed.x += acc.x/mass*step;
    this->speed.y += acc.y/mass*step;
}

void Ball::handleWallCollision()
{

    double R = 0.5; // coefficient de restitution balle/mur

    if (pos.x <= 0) {
        pos.x *= -1;
        speed.x *= -R;
    }
    if (pos.x >= (LENGTH-2*radius)) {
        pos.x = LENGTH-2*radius + (LENGTH-2*radius - pos.x);
        speed.x *= -R;
    }
    if (pos.y <= 0) {
        pos.y *= -1;
        speed.y *= -R;
    }
    if (pos.y >= (HEIGHT-2*radius)) {
        pos.y = HEIGHT-2*radius + (HEIGHT-2*radius - pos.y);
        speed.y *= -R;
    }

    ballShape.setPosition(pos);
}

void Ball::setRadius(float newr)
{
    this->radius=newr;
    ballShape.setRadius(newr);
}

void Ball::setColor(sf::Color color)
{
    ballShape.setFillColor(color);
}

void Ball::setXY(float x, float y)
{
    pos.x = x;
    pos.y = y;
    ballShape.setPosition(x, y);
}

void Ball::setSpeedXY(float sx, float sy)
{
    speed.x = sx;
    speed.y = sy;
}

void Ball::setAccXY(float newaccx, float newaccy)
{
    acc.x = newaccx;
    acc.y = newaccy;
}

Ball::~Ball()
{
    //dtor
}
