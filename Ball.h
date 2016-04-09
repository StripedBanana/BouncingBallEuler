#ifndef BALL_H
#define BALL_H
#include <SFML/Graphics.hpp>

class Ball
{
    public:
        Ball();
        Ball(float x, float y, float sx, float sy, float accx, float accy, float radius, float mass);
        void handleWallCollision();
        virtual ~Ball();

        void updatePos(float step); // Using Euleur method

        // Getters
        float getM(){return mass;};
        float getX(){return pos.x;};
        float getY(){return pos.y;};
        float getSpeedX(){return speed.x;};
        float getSpeedY(){return speed.y;};
        float getAccX(){return acc.x;};
        float getAccY(){return acc.y;};
        float getRadius(){return radius;};

        // Setters
        void setRadius(float newr);
        void setXY(float x, float y);
        void setSpeedXY(float sx, float sy);
        void setAccXY(float newaccx, float newaccy);

    protected:

    private:
        // Ball positions
        sf::Vector2f pos, speed, acc;
        float radius;
        float mass;
};

#endif // BALL_H
