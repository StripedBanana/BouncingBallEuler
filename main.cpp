#include "header.h"
#include "Quadtree.h"
#include <time.h>
#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

int main()
{
    // Loading the font and setting up fps
    sf::Clock clock;
    sf::Font MyFont;
    if (!MyFont.loadFromFile("arial.ttf")) cout << "Error while loading font file" << endl;
    sf::Text fps("Hello", MyFont, 50);
    fps.setColor(sf::Color::Blue);
    fps.setPosition(0,0);

    // User inputs
    int nbBalls, ballRadius, accY;
    float step, stepCollision;
    cout << "Enter the balls number: ";
    cin >> nbBalls;
    cout << "Enter the balls radius: ";
    cin >> ballRadius;
    cout << "Enter the step: ";
    cin >> step;
    cout << "Enter the value of g: ";
    cin >> accY;

    // Variables declarations
    srand(time(NULL));
    int framerate = 60, frameCount = 0;
    double x, y, sx, sy;
    vector<Ball*> balls(nbBalls);
    vector<Ball*> returnBalls(nbBalls);
    vector<sf::CircleShape*> ballShapes(nbBalls);
    Ball* ballTest;
    sf::CircleShape* ballTestShape;
    Quadtree* quad = new Quadtree(0, 0, 0, LENGTH, HEIGHT);
    bool collision;

    // Initializing SFML window
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(LENGTH, HEIGHT), "Bouncing ball", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // Initializing declared arrays
    for(int i=0; i<nbBalls; i++)
    {
        ballShapes[i] = new sf::CircleShape(ballRadius);
        sf::Color color(rand()%255+1, rand()%255+1, rand()%255+1, 255);
        (*ballShapes[i]).setFillColor(color);
        x = rand()%(LENGTH-2*ballRadius);
        y = rand()%(HEIGHT-2*ballRadius);
        sx = rand()%1000+1;
        sy = rand()%1000+1;
        balls[i] = new Ball(x, y, sx, sy, 0, accY, ballRadius, 1);
    }

    pool(balls);
    //bowling(balls);



    // Process loop
    while (window.isOpen())
    {
        // Event handler
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Resetting the window and the quadtree
        window.clear();
        quad->clr();
        //cout << "(" << balls[0]->getX() << "," << balls[0]->getY() << ")" << endl;

        // Loop setting up the quadtree
        for(int i=0; i<nbBalls; i++)
        {
            quad->insertion(balls[i]);
        }

        // Displaying the quadtree
        quad->display(&window);

        // Loop updating objects positions and detecting collisions
        for(int i=0; i<nbBalls; i++)
        {
            balls[i]->updatePos(step);
            balls[i]->handleWallCollision();
            (*ballShapes[i]).setPosition(balls[i]->getX(), balls[i]->getY());
            window.draw(*ballShapes[i]);
        }

        // Loop detecting in-between objects collisions
        for(int i=0; i<nbBalls; i++)
        {
            // Running quadtree for list of possible collisions, one object at a time
            ballTest = balls[i];
            returnBalls.erase(returnBalls.begin(), returnBalls.end());
            quad->retrieve(&returnBalls, ballTest);

            // Checking collision and updating speeds of both balls if so
            for (int x = 0; x < returnBalls.size(); x++) {
                // Running collision detection algorithm between objects
                if(ballTest != returnBalls[x]) collision = circlesColliding(ballTest, returnBalls[x]);
                else collision = false;
                if(collision)
                {
                    // Make the balls that collided move at the exact collision point in time
                    ballTest->updatePos(-step); // first thing: go back when they didn't collide
                    returnBalls[x]->updatePos(-step);
                    stepCollision = exactCollision(ballTest, returnBalls[x]); // second: evaluate when they will collide
                    cout << "t=" << stepCollision << endl;
                    if(-stepCollision < step && stepCollision > 0)
                    {
                        ballTest->updatePos(stepCollision); // third: move accordingly
                        returnBalls[x]->updatePos(stepCollision);
                    }
                    handleBallCollision(&ballTest, &returnBalls[x]);

                    ballTest->updatePos(step-stepCollision); // going back to normal
                    returnBalls[x]->updatePos(step-stepCollision);


                }
            }
        }


        // Displaying fps
        frameCount++;
        if(frameCount > 60)
        {
            framerate = 60 / clock.getElapsedTime().asSeconds();
            clock.restart();
            frameCount = 0;
        }
        stringstream ss;
        ss << framerate;
        fps.setString(ss.str().c_str());
        window.draw(fps);

        // Updating window
        debug(&window, balls); // debug display
        window.display();
    }

    return 0;
}

int circlesColliding(Ball* b1, Ball* b2)
{
    //compare the distance to combined radii
    float x1 = b1->getX(), y1 = b1->getY();
    float x2 = b2->getX(), y2 = b2->getY();
    float r1 = b1->getRadius(), r2 = b2->getRadius();
    bool collision = false;
    float d = (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);

    if(d < (r1 + r2)*(r1 + r2))
    {
        collision = true;
    }
    return collision;
}


void handleBallCollision(Ball** b1, Ball** b2)
{

    /**
     * Modifies les vecteurs vitesses de deux balles qui entrent en collision.
     */

    double R = 1; // coefficient de restitution

    float x1 = (*b1)->getX(), y1 = (*b1)->getY();
    float x2 = (*b2)->getX(), y2 = (*b2)->getY();

    float v1x_i = (*b1)->getSpeedX(), v1y_i = (*b1)->getSpeedY();
    float v2x_i = (*b2)->getSpeedX(), v2y_i = (*b2)->getSpeedY();

    float radius1 = (*b1)->getRadius(), radius2 = (*b2)->getRadius();
    float m1 = (*b1)->getM(), m2 = (*b2)->getM();


    // Calcul de la base orthonormée (n,g)
    double dx = x1 - x2;
    double dy = y1 - y2;
    double r = sqrt(dx*dx + dy*dy);

    // Correction pour éviter un chevauchement
    /*
    sf::Vector2f mtd(dx * ((radius1+radius2) - r)/r, dy * ((radius1+radius2) - r)/r);
    sf::Vector2f correction(mtd.x * m2/(m2 + m1), mtd.x * m2/(m2 + m1));
    (*b1)->setXY(x1 + correction.x, y1 + correction.y);
    (*b1)->setOldXY((*b1)->getOldX() + correction.x, (*b1)->getOldY() + correction.y);
    (*b2)->setXY(x2 - correction.x, y2 - correction.y);
    (*b2)->setOldXY((*b2)->getOldX() - correction.x, (*b2)->getOldY() - correction.y);

    cout << "correctionX" << correction.x << endl;
    cout << "correctionY" << correction.y << endl << endl;*/

    // n est perpendiculaire au plan de collision
    double nx = (x2 - x1) / r;
    double ny = (y2 - y1) / r;

    // g est tangent au plan de collision
    double gx = -ny;
    double gy =  nx;

    // Transition des vitesses de la base (x,y) vers (n,g)

    double v1n = nx*v1x_i + ny*v1y_i;
    double v1g = gx*v1x_i + gy*v1y_i;
    double v2n = nx*v2x_i + ny*v2y_i;
    double v2g = gx*v2x_i + gy*v2y_i;

    // Détermination des nouvelles vitesses dans (n,g)
    double m = m1 + m2;

    double m12 = (m1 - m2)/m;
    double m22 = (m2 + m2)/m;
    double m11 = (m1 + m1)/m;
    double m21 = (m2 - m1)/m;

    double vx_cm = (m1*v1x_i+m2*v2x_i)/(m1+m2) ;
    double vy_cm = (m1*v1y_i+m2*v2y_i)/(m1+m2) ;



    double v1n2 = m12*v1n + m22*v2n; //=v2n if m1=m2
    double v1g2 = m12*v2g + m22*v1g; //=v1g
    double v2n2 = m11*v1n + m21*v2n; //=v1n
    double v2g2 = m11*v2g + m21*v1g; //=v2g

    // Modification des vitesses dans la base (x,y)

    double v1x_f = nx*v1n2 + gx*v1g2;
    double v1y_f = ny*v1n2 + gy*v1g2;
    double v2x_f = nx*v2n2 + gx*v2g2;
    double v2y_f = ny*v2n2 + gy*v2g2;

    v1x_f=(v1x_f-vx_cm)*R + vx_cm;
    v1y_f=(v1y_f-vy_cm)*R + vy_cm;
    v2x_f=(v2x_f-vx_cm)*R + vx_cm;
    v2y_f=(v2y_f-vy_cm)*R + vy_cm;

    (*b1)->setSpeedXY(v1x_f, v1y_f);
    (*b2)->setSpeedXY(v2x_f, v2y_f);

}

float exactCollision(Ball* b1, Ball* b2)
{
    float x1 = b1->getX(), y1 = b1->getY();
    float x2 = b2->getX(), y2 = b2->getY();
    float vx1 = b1->getSpeedX(), vy1 = b1->getSpeedY();
    float vx2 = b2->getSpeedX(), vy2 = b2->getSpeedY();
    sf::Vector2f v_ab(vx2 - vx1, vy2 - vy1);
    sf::Vector2f p_ab(x2 - x1, y2 - y1);
    double a = v_ab.x * v_ab.x + v_ab.y * v_ab.y;
    double b = 2 * (p_ab.x * v_ab.x + p_ab.y * v_ab.y);
    double c = (p_ab.x * p_ab.x + p_ab.y * p_ab.y) - (b1->getRadius() + b2->getRadius()) * (b1->getRadius() + b2->getRadius());
    double discriminant = b * b - 4 * a * c;
    cout << discriminant << endl;
    if(discriminant < 0) return 0; // if the discriminant is negative, then collision never happened and will never happen (conflict)
    else if(discriminant == 0) return -b / 2 / a; // both balls barely touch when they collide
    else // if the discriminant is positive, two solutions are possible: we take the smallest for the initial impact
    {
        double root = sqrt(discriminant);
        double t1 = (-b + root) / 2 / a;
        double t2 = (-b - root) / 2 / a;
        return min(t1, t2);
    }
}

void pool(vector<Ball*> balls)
{
    float r = balls[0]->getRadius();
    float root3 = sqrt(3);
    float safety = 0.05 * r;
    balls[0]->setXY(3*LENGTH/4, HEIGHT/2 - 2*(r+safety));
    balls[0]->setSpeedXY(0,0);
    balls[1]->setXY(3*LENGTH/4, HEIGHT/2);
    balls[1]->setSpeedXY(0,0);
    balls[2]->setXY(3*LENGTH/4, HEIGHT/2 + 2*(r+safety));
    balls[2]->setSpeedXY(0,0);

    balls[3]->setXY(3*LENGTH/4 - (r+safety)*root3, HEIGHT/2 - (r+safety));
    balls[3]->setSpeedXY(0,0);
    balls[4]->setXY(3*LENGTH/4 - (r+safety)*root3, HEIGHT/2 + (r+safety));
    balls[4]->setSpeedXY(0,0);

    balls[5]->setXY(3*LENGTH/4 - 2*(r+safety)*root3, HEIGHT/2);
    balls[5]->setSpeedXY(0,0);

    balls[6]->setXY(0, HEIGHT/2);
    balls[6]->setSpeedXY(500,0);
}

void bowling(vector<Ball*> balls)
{
    float r = balls[0]->getRadius();
    float root3 = sqrt(3);
    balls[0]->setXY(3*LENGTH/4, HEIGHT/2 - 3*r);
    balls[0]->setSpeedXY(0,0);
    balls[1]->setXY(3*LENGTH/4, HEIGHT/2);
    balls[1]->setSpeedXY(0,0);
    balls[2]->setXY(3*LENGTH/4, HEIGHT/2 + 3*r);
    balls[2]->setSpeedXY(0,0);

    balls[3]->setXY(3*LENGTH/4 - 2*r, HEIGHT/2 - 1.5*r);
    balls[3]->setSpeedXY(0,0);
    balls[4]->setXY(3*LENGTH/4 - 2*r, HEIGHT/2 + 1.5*r);
    balls[4]->setSpeedXY(0,0);

    balls[5]->setXY(3*LENGTH/4 - 4*r, HEIGHT/2);
    balls[5]->setSpeedXY(0,0);

    balls[6]->setXY(0, HEIGHT/2);
    balls[6]->setSpeedXY(500,0);
}

void debug(sf::RenderWindow* window, vector<Ball*> balls)
{
    sf::CircleShape center(balls[0]->getRadius()/10);
    center.setFillColor(sf::Color::Green);
    for(int i=0; i<balls.size(); i++)
    {
        center.setPosition(balls[i]->getX(), balls[i]->getY());
        cout << "s" << i << "=(" << balls[i]->getSpeedX() << "," << balls[i]->getSpeedY() << ")" << endl;
        window->draw(center);
    }
}

