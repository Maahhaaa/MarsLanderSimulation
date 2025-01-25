#include <SFML/Audio.hpp>
#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include "Collision.h"
#include <SFML/Network.hpp>
using namespace sf;

float landingSpeed  = 10;
float g=3.721;
float l[5] = {0,1,2.5,3.721,10.5};
float fuel = 500;
struct Acceleration
{ float x,y;} a;


struct Velocity
{ float x,y;} v;

struct Obstacle
{ int x,y;}  o[20];
int frame = 0;
int level=0;
int angle = 0;
bool isLanded = false;
bool isCrashed = false;
float data[8];
int counter=0;

int N=50,M=50;
int size=16;
int w = size*N;
int h = size*M;

struct Craft
{ float x,y;} c;



void game()
{
    a.x=0; a.y=0;
    v.x=0; v.y=0;
    TcpSocket tcpSscket;
    Socket::Status status = tcpSscket.connect("10.171.224.84", 53000);
    if (status != sf::Socket::Done)
        {
            // error...
            std::cout << "Could not connect to server!"<< std::endl;
        }else{
            std::cout << "Connection established successfully!"<< std::endl;
        }
    srand(time(0));

    RenderWindow window(VideoMode(w, h), "Mars Landing!");
    window.setFramerateLimit(60);

    c.y = window.getSize().y/2;


    SoundBuffer buf1, buf2, buf3;
    Sound engine, crash, success;
    buf1.loadFromFile("sounds/engine.wav");
    buf2.loadFromFile("sounds/crash.wav");
    buf3.loadFromFile("sounds/success.wav");
    engine.setBuffer(buf1);
    crash.setBuffer(buf2);
    success.setBuffer(buf3);
    Music theme;
    theme.openFromFile("sounds/theme.wav");

    Texture tex1, tex2, tex3, texBack[4], texFire[60];
    tex1.loadFromFile("images/newobs.png");
    tex2.loadFromFile("images/white rocket2.png");
    tex3.loadFromFile("images/land.png");
    for(int i=0; i<4; i++)
        texBack[i].loadFromFile("images/s"+std::to_string(i)+".png");

    Sprite sObstacle(tex1), craft(tex2), ground(tex3), background(texBack[frame]);
    background.setScale(0.27,0.27);
    craft.setScale(0.08,0.08);
    craft.setOrigin(craft.getGlobalBounds().width/2,craft.getGlobalBounds().height/2);

    ground.setScale(0.5,0.5);
    ground.setOrigin(ground.getGlobalBounds().width/2,0);
    ground.setPosition(w/2, h-60);

    Font font;
    font.loadFromFile("fonts/University.otf");
    Text t1("", font, 20), t2("", font, 20), t3("", font, 20), t4("", font, 20),t5("", font, 20);

    t4.setString("Successful");
    t5.setString("press R to play again");

    Clock clock;
    float timer=0, delay=0.01;
    c.x = w/2;

    for (int i=0;i<20;i++)
    {
        o[i].y = rand()%120 + h - 120;
        o[i].x=rand()%w;
    }

    theme.setLoop(true);
    theme.play();

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer+=time;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();
            if(!isLanded)
            {
                if (Keyboard::isKeyPressed(Keyboard::Up)&& level!=4) ++level;
                if (Keyboard::isKeyPressed(Keyboard::Down)&&level!=0) --level;
                if (Keyboard::isKeyPressed(Keyboard::Right)&& angle!=45) angle++;
                if (Keyboard::isKeyPressed(Keyboard::Left)&& angle!=-45) angle--;
            }
        }
        if (fuel<=0)
        {
            fuel=0;
            level=0;
        }

        if (!Collision::pixelPerfectTest(ground, craft) && !isCrashed && !isLanded)
        {
            if (timer>delay)
            {
                a.y=g-l[level]*std::cos(angle*3.14159/180);
                c.y += v.y*timer + 0.5*a.y*std::pow(timer,2);
                v.y += a.y*timer;
                a.x = l[level]*std::sin(angle*3.14159/180);
                c.x += v.x*timer + 0.5*a.x*std::pow(timer,2);
                v.x += a.x*timer;
                fuel -= l[level] * timer;
                if(c.x>= w-craft.getOrigin().x)
                {
                    c.x= craft.getOrigin().x +1;
                    frame++;
                }
                if(c.x<= 0)
                {
                    c.x=w-craft.getOrigin().x-1;
                    frame--;
                    if(frame<0) frame=3;
                }
                timer = 0;
            } //ticking
        }else{
            theme.stop();
            if(!isLanded)
            {
                if(v.y > landingSpeed || angle != 0 || isCrashed )
                {
                    isCrashed = true;
                    t4.setString("Failure");
                    if(!crash.getStatus()) crash.play();

                }else{
                    if(!success.getStatus()) success.play();

                }
            }
            a.x=0; a.y=0;
            v.x=0; v.y=0;
            isLanded = true;
        }

        craft.setPosition(c.x,c.y);
        craft.rotate(angle-craft.getRotation());

        t1.setString("Velocity  Vertical : " + std::to_string((int)v.y) + " m/s"+"   Horizontal: " +std::to_string((int)v.x) + " m/s");
        t2.setString("Acceleration  Vertical : " + std::to_string((int)a.y) + " m/s^2"+"   Horizontal: " +std::to_string((int)a.x) + " m/s^2");
        t3.setString("Power Level: " + std::to_string(level)+ "     Angle: " + std::to_string(angle)+" degree   " + "Fuel: " + std::to_string((int)fuel));

        t1.setPosition(10,30);
        t2.setPosition(10,80);
        t3.setPosition(10,130);
        t4.setPosition((w/2)-70,h/2);
        t5.setPosition((w/2)-170,(h/2)+70);

        window.clear(Color::Blue);

        background.setTexture(texBack[frame]);
        window.draw(background);
        if(!Collision::pixelPerfectTest(background, craft)) isCrashed=true;

        for (int i=0;i<20;i++)
        {
            sObstacle.setPosition(o[i].x,o[i].y);
            window.draw(sObstacle);
            if(Collision::pixelPerfectTest(sObstacle, craft)) isCrashed=true;
        }


        window.draw(ground);
        window.draw(craft);
        window.draw(t1);
        window.draw(t2);
        window.draw(t3);
        if(isLanded) {window.draw(t4); window.draw(t5);}


        if (Keyboard::isKeyPressed(Keyboard::R)&&isLanded)
        {
            landingSpeed  = 10;
            g=3.721;
            fuel = 500;
            frame = 0;
            level=0;
            angle = 0;
            isLanded = false;
            isCrashed = false;
            N=50,M=50;
            size=16;
            w = size*N;
            h = size*M;
            window.close();
            game();
        }
        window.display();
        counter++;
         //sending
        if (counter%60 == 0)
        {
            data[0] =  v.x; data[1] = v.y;
            data[2] = a.x; data[3] = a.y;
            data[4] = (float)angle; data[5] = fuel;
            data[6]=0.0; data[7]=0.0;
            if(isCrashed)data[6] = 1.0;
            if(isLanded)data[7] = 1.0;
            // TCP socket:
            if (tcpSscket.send(data, 8*32) != sf::Socket::Done)
            {
                // error...
                std::cout << "Connection lost!"<< std::endl;
            }
        }
    }
}


int main()
{
    int choise;
    std::cout<<"enter 1 to be server, any number else to play"<<std::endl;
    std::cin>> choise;
    if(choise==1)
    {

        sf::TcpListener listener;

        // bind the listener to a port
        if (listener.listen(53000) != sf::Socket::Done)
        {
            // error...
            std::cout << "Could not start listening"<< std::endl;

        }else{
            std::cout << "Server is listening!"<< std::endl;
        }

        sf::TcpSocket client;
        if (listener.accept(client) != sf::Socket::Done)
        {
            // error...
            std::cout << "error in 2"<< std::endl;
        }

        //receiving
        float data[8];
        std::size_t received;
        float x=0;
        while (true)
        {
            if(client.receive(data, 8*4, received) == sf::Socket::Done)
            {
                system("CLS");
                // TCP socket:
                if (client.receive(data, 8*4, received) != sf::Socket::Done)
                {
                    // error...
                    std::cout << "error in 4"<< std::endl;
                }
                std::cout << "velocity x:" <<data[0]<<std::endl;
                std::cout << "velocity y:" <<data[1]<<std::endl;
                std::cout << "acceleration x:" <<data[2]<<std::endl;
                std::cout << "acceleration y:" <<data[3]<<std::endl;
                std::cout << "angle:" <<data[4]<<std::endl;
                std::cout << "fuel:" <<data[5]<<std::endl;
                std::cout << "crashed:" <<data[6]<<std::endl;
                std::cout << "landed:" <<data[7]<<std::endl;
            }
        }
    }else{
        game();
    }
    return 0;
}
