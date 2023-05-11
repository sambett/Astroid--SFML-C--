#include<iostream>
#include<SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include <vector>
#include <sstream>

using namespace std;
using namespace sf;

const int W = 1700;
const int H = 956;
const int MAX_NUMBER_OF_ITEMS = 2;

float DEGTORAD = 0.017453f;

class Menu {
public:
    Menu(float width, float height) {
        if (!font.loadFromFile("font.ttf")) {
            std::cout << "Error loading font file!" << std::endl;
        }
        menu[0].setFont(font);
        menu[0].setFillColor(sf::Color::Green);
        menu[0].setString("Start Game");
        menu[0].setPosition(sf::Vector2f(width / 1.5, height / (MAX_NUMBER_OF_ITEMS + 1) * 1));

        menu[1].setFont(font);
        menu[1].setFillColor(sf::Color::Red);
        menu[1].setString("Exit");
        menu[1].setPosition(sf::Vector2f(width / 1.5, height / (MAX_NUMBER_OF_ITEMS + 1) * 2));

        selectedItemIndex = 0;

        // Load the background texture
        if (!bgTexture.loadFromFile("images/background.jpg")) {
            std::cout << "Error loading background texture!" << std::endl;
        }
        bgSprite.setTexture(bgTexture);

        // Load the background music
        if (!bgMusic.openFromFile("images/music/cool.ogg")) {
            std::cout << "Error loading background music!" << std::endl;
        }
        bgMusic.setLoop(true);
        bgMusic.play();
    }


    void draw(sf::RenderWindow& window) {
        window.draw(bgSprite);

        for (int i = 0; i < MAX_NUMBER_OF_ITEMS; i++) {
            window.draw(menu[i]);
        }
    }

    void moveUp() {
        if (selectedItemIndex - 1 >= 0) {
            menu[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex--;
            menu[selectedItemIndex].setFillColor(sf::Color::Green);
        }
    }

    void moveDown() {
        if (selectedItemIndex + 1 < MAX_NUMBER_OF_ITEMS) {
            menu[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex++;
            menu[selectedItemIndex].setFillColor(sf::Color::Red);
        }
    }
    void stopBackgroundMusic() {
        bgMusic.stop();
    }
    void playBackgroundMusic() {
        bgMusic.play();
    }

    int getSelectedItemIndex() {
        return selectedItemIndex;
    }

private:
    sf::Font font;
    sf::Text menu[MAX_NUMBER_OF_ITEMS];
    int selectedItemIndex;
    sf::Texture bgTexture;
    sf::Sprite bgSprite;
    sf::Music bgMusic;
};
class Animation
{
public:
    float Frame, speed;
    Sprite sprite;
    std::vector<IntRect> frames;

    Animation() {}

    Animation(Texture& t, int x, int y, int w, int h, int count, float Speed)
    {
        Frame = 0;
        speed = Speed;

        for (int i = 0; i < count; i++)
            frames.push_back(IntRect(x + i * w, y, w, h));

        sprite.setTexture(t);
        sprite.setOrigin(w / 2, h / 2);
        sprite.setTextureRect(frames[0]);
    }


    void update()
    {
        Frame += speed;
        int n = frames.size();
        if (Frame >= n) Frame -= n;
        if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
    }

    bool isEnd()
    {
        return Frame + speed >= frames.size();
    }

};


class Entity
{
public:
    float x, y, dx, dy, R, angle;
    bool life;
    std::string name;
    Animation anim;

    Entity()
    {
        life = 1;
    }

    void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 1)
    {
        anim = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
    }

    virtual void update() {};

    void draw(RenderWindow& app)
    {
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle + 90);
        app.draw(anim.sprite);

        CircleShape circle(R);
        circle.setFillColor(Color(255, 0, 0, 170));
        circle.setPosition(x, y);
        circle.setOrigin(R, R);
        //app.draw(circle);
    }

    virtual ~Entity() {};
};


class asteroid : public Entity
{
public:
    asteroid()
    {
        dx = rand() % 8 - 4;
        dy = rand() % 8 - 4;
        name = "asteroid";
    }

    void update()
    {
        x += dx;
        y += dy;

        if (x > W) x = 0;  if (x < 0) x = W;
        if (y > H) y = 0;  if (y < 0) y = H;
    }

};


class bullet : public Entity
{
public:
    bullet()
    {
        name = "bullet";
    }

    void  update()
    {
        dx = cos(angle * DEGTORAD) * 6;
        dy = sin(angle * DEGTORAD) * 6;
        // angle+=rand()%7-3;  /*try this*/
        x += dx;
        y += dy;

        if (x > W || x<0 || y>H || y < 0) life = 0;
    }

};


class gamer : public Entity
{
public:
    bool thrust;

    gamer()
    {
        name = "gamer";
    }   

    void update()
    {
        if (thrust)
        {
            dx += cos(angle * DEGTORAD) * 0.2;
            dy += sin(angle * DEGTORAD) * 0.2;
        }
        else
        {
            dx *= 0.99;
            dy *= 0.99;
        }

        int maxSpeed = 15;
        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed)
        {
            dx *= maxSpeed / speed;
            dy *= maxSpeed / speed;
        }

        x += dx;
        y += dy;

        if (x > W) x = 0; if (x < 0) x = W;
        if (y > H) y = 0; if (y < 0) y = H;
    }

};


bool isCollide(Entity* a, Entity* b)
{
    return (b->x - a->x) * (b->x - a->x) +
        (b->y - a->y) * (b->y - a->y) <
        (a->R + b->R) * (a->R + b->R);
}


int main()
{
    srand(time(0));

    Texture t1, t2, t3, t4, t5, t6, t7;
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");
    //-----------------------------------------------------------------------
    // Game Score -----------------------------------------------------------
    RectangleShape scoreZone(sf::Vector2f(500, 90)); // un Rectangle noir pour afficher le texte de l'heure
    
    scoreZone.setFillColor(Color::Black);
    scoreZone.setOutlineThickness(1.0f);
    scoreZone.setOutlineColor(Color::White);
    scoreZone.setPosition(0, 0);
    Font font; // Police pour le texte du temps écoulé
    font.loadFromFile("font.ttf"); 
    //------------------------------------------------------------------------
    sf::Text timeText; 
    timeText.setFont(font); 
    timeText.setCharacterSize(40); 
    timeText.setFillColor(sf::Color::Yellow); 
    timeText.setPosition(100, 10); 
    // Timer
    sf::Clock timer;
    timer.restart();
   
    //-----------------------------------------------------------------------------
    t1.setSmooth(true);
    t2.setSmooth(true);

    Sprite background(t2);

    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
    Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
    Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);




    std::list<Entity*> entities;

    for (int i = 0; i < 15; i++)
    {
        asteroid* a = new asteroid();
        a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25);
        entities.push_back(a);
    }

    gamer* p = new gamer();
    p->settings(sPlayer, 200, 200, 0, 20);
    entities.push_back(p);
    sf::RenderWindow window(sf::VideoMode(800, 600), "Menu");

    Menu menu(window.getSize().x, window.getSize().y);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::KeyReleased:
                switch (event.key.code)
                {
                case sf::Keyboard::Up:
                    menu.moveUp();
                    break;
                case sf::Keyboard::Down:
                    menu.moveDown();
                    break;
                case sf::Keyboard::Return:
                    switch (menu.getSelectedItemIndex())
                    {
                    case 0:
                    {
                        std::cout << "Start button pressed!" << std::endl;

                        RenderWindow app(VideoMode(W, H), "Asteroids!");
                        app.setFramerateLimit(60);
                        //main loop
                        int lives = 3;  // start with 3 lives
                        bool game_over = false;  // flag to indicate whether the game is over
                        menu.playBackgroundMusic();
                        timer.restart();
                        while (app.isOpen())
                        {
                            Event event;
                            while (app.pollEvent(event))
                            {
                                if (event.type == Event::Closed)
                                    app.close();

                                if (event.type == Event::KeyPressed)
                                    if (event.key.code == Keyboard::Space)
                                    {
                                        bullet* b = new bullet();
                                        b->settings(sBullet, p->x, p->y, p->angle, 10);
                                        entities.push_back(b);
                                    }
                                // counting the time (the score to my game)-------------------------------------------------------
                                int seconds = timer.getElapsedTime().asSeconds(); // counting number of seconds passed
                               
                                std::stringstream time;
                                time << "Time: " << seconds << "s";
                                timeText.setString(time.str()); // timeText  gets the value of seconds passes as string
                                
                            }
                            /*   alternative the change the game functionality to keyboard instead
                            in case i want my game to work with the keyboad instead
                            if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3;
                            if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle -= 3;
                            if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust = true;
                            else p->thrust = false;
                            */

                            if (event.type == Event::Closed)
                                app.close();

                            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
                                p->thrust = true;
                            else if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
                                p->thrust = false;

                            if (event.type == Event::MouseMoved)
                            {
                                // Get the current mouse position
                                Vector2i mousePos = Mouse::getPosition(app);

                                // Calculate the angle between the gamer and the mouse position
                                float dx = mousePos.x - p->x;
                                float dy = mousePos.y - p->y;
                                float angle = atan2(dy, dx) * 180;// M_PI;
                                p->angle = angle;
                            }


                            
                            for (auto a : entities)
                                for (auto b : entities)
                                {
                                    if (a->name == "asteroid" && b->name == "bullet")
                                        if (isCollide(a, b))
                                        {
                                            a->life = false;
                                            b->life = false;
                                            Entity* e = new Entity();
                                            e->settings(sExplosion, a->x, a->y);
                                            e->name = "explosion";
                                            entities.push_back(e);


                                            for (int i = 0; i < 2; i++)
                                            {
                                                if (a->R == 15) continue;
                                                Entity* e = new asteroid();
                                                e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
                                                entities.push_back(e);
                                            }

                                        }

                                    if (a->name == "gamer" && b->name == "asteroid")
                                        if (isCollide(a, b))
                                        {
                                            
                                            b->life = false;
                                            Entity* e = new Entity();
                                            e->settings(sExplosion_ship, a->x, a->y);
                                            e->name = "explosion";
                                            entities.push_back(e);

                                            // handle gamer respawn and life loss
                                            p->settings(sPlayer, W / 2, H / 2, 0, 20);
                                            p->dx = 0; p->dy = 0;
                                            lives--;  // decrement the gamer's lives
                                            if (lives <= 0) {
                                            game_over = true;  // set the game over flag
                                            }
                                            
                                        }
                                    
                                }


                            if (p->thrust)  p->anim = sPlayer_go;
                            else   p->anim = sPlayer;


                            for (auto e : entities)
                                if (e->name == "explosion")
                                    if (e->anim.isEnd()) e->life = 0;


                            if (rand() % 150 == 0)
                            {
                                asteroid* a = new asteroid();
                                a->settings(sRock, 0, rand() % H, rand() % 360, 25);
                                entities.push_back(a);
                            }

                            for (auto i = entities.begin(); i != entities.end();)
                            {
                                Entity* e = *i;

                                e->update();
                                e->anim.update();

                                if (e->life == false) { i = entities.erase(i); delete e; }
                                else i++;
                            }
                          
                            
                            // game over 
                            if (game_over) {
                                
                                menu.stopBackgroundMusic();
                                app.close();   
                                
                            }
                            else {
                                // draw game entities
                                app.draw(background);
                                for (auto i : entities) i->draw(app);
                                app.display();
                            }
                           
                        }
                        break;
                    }
                    case 1:
                        window.close();
                        break;
                    }
                    break;
                }
                break;
            case sf::Event::Closed:
                window.close();
                break;
            }
        }

        window.clear();
        menu.draw(window);
        window.draw(scoreZone);
        window.draw(timeText);
        window.display();
    }
    return 0;
}

