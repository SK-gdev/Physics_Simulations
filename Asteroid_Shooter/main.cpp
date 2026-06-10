#include "RigidBody.cpp"
#include <SFML/Graphics.hpp>
#include <vector>

int main()
{
    //Particles
    RigidBody ship(100.0f,50.0f,70.0f,20.0f,0.5f,0.0f);
    std::vector<RigidBody> bullets;
    std::vector<RigidBody> asteroids;

    //Ship check
    bool shipAlive = true;
    bool reloading = false;
    float reload_timer = 0.0f;
    float reload_time = 3.0f;
    int mag = 30;

    //Point
    int points = 0;

    //Graphics
    sf::CircleShape gr_ship;
    std::vector<sf::CircleShape> gr_bullets;
    std::vector<sf::CircleShape> gr_asteroids;

    //Asteroid Spawn
    float spawn_timer = 0.0f;
    float spawn_interval = 1.0f/90.0f;

    //Set Ship
    gr_ship.setFillColor(sf::Color::Cyan);
    gr_ship.setRadius(ship.radius);
    gr_ship.setPosition({ship.position.x-ship.radius, 720-ship.position.y-ship.radius});

    //Window
    unsigned screenX = 1020;
    unsigned screenY = 720;

    //Font
    sf::Font f;
    if(!f.openFromFile("C://Windows//Fonts//GARA.ttf"))
    return -1;

    //Text
    sf::Text txt_point(f,"",12u);
    sf::Text txt_reset(f,"To Reset - [R]", 12u);
    sf::Text txt_move(f, "To Move - [Arrow Keys]", 12u);
    sf::Text txt_reload(f, "To Reload - [A]", 12u);
    sf::Text txt_space(f, "To Shoot - [Space]", 12u);
    sf::Text txt_color(f,"Your Color - Cyan", 12u);
    sf::Text txt_mag(f,"",12u);

    //Text Setup
    txt_point.setFillColor(sf::Color::White);
    txt_point.setPosition({10.0f,40.0f});

    txt_move.setFillColor(sf::Color::White);
    txt_move.setPosition({10.0f,52.0f});

    txt_reset.setFillColor(sf::Color::White);
    txt_reset.setPosition({10.0f,64.0f});

    txt_reload.setFillColor(sf::Color::White);
    txt_reload.setPosition({10.0f,76.0f});

    txt_space.setFillColor(sf::Color::White);
    txt_space.setPosition({10.0f,88.0f});

    txt_mag.setFillColor(sf::Color::White);
    txt_mag.setPosition({10.0f,100.0f});

    txt_color.setFillColor(sf::Color::White);
    txt_color.setPosition({10.0f,112.0f});

    sf::RenderWindow window(sf::VideoMode({screenX,screenY}), "Asteroid Shooter 2D");
    window.setFramerateLimit(90);
    float dt = 1.0f/90.0f;

    while(window.isOpen())
    {
        window.clear();
        while(std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }
                   
        //Bullet Shoot
        if(event->is<sf::Event::KeyPressed>())
        {
            auto mb = event->getIf<sf::Event::KeyPressed>();
        if(mb->code == sf::Keyboard::Key::Space && shipAlive && mag>0 && !reloading)
        {
            RigidBody bullet(ship.position.x-ship.radius,ship.position.y-ship.radius,5.0f,5.0f,0.9f,0.0f);
            sf::CircleShape gr_bullet;
            bullet.total_life = 2.0f ;
            gr_bullet.setFillColor(sf::Color::Yellow);
            gr_bullet.setOutlineColor(sf::Color::White);
            gr_bullet.setOutlineThickness(0.8);
            gr_bullet.setRadius(bullet.radius);
            gr_bullet.setPosition({bullet.position.x,screenY-bullet.position.y});
            
            Vec2 dir = ship.position.getdirection();
            float speed = 300.0f;
            Vec2 normdir(0.0f,0.0f);
            if(sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Left))
                normdir = Vec2(-1.0f,0.0f);
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                normdir = Vec2(1.0f,0.0f);
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
                normdir = Vec2(0.0f,1.0f);
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
                normdir = Vec2(0.0f,-1.0f);
            else
                normdir = Vec2(0.0f,1.0f);
            bullet.velocity = normdir*speed;

            mag = mag-1;
            
            bullets.push_back(bullet);
            gr_bullets.push_back(gr_bullet);
            
       }

       //Reload
       if(mb->code == sf::Keyboard::Key::A && mag<30 && !reloading)
       {
            reloading = true;          
       }

       //Reset       
       if(mb->code == sf::Keyboard::Key::R)
       {
        ship.position = Vec2(100.0f,50.0f);
        ship.velocity = Vec2(0.0f,0.0f);
        shipAlive = true;
        points = 0;
        mag = 30;
        bullets.clear();
        gr_bullets.clear();
        asteroids.clear();
        gr_asteroids.clear();
       }
    }
    }

       if(shipAlive)
       {

        //Random asteroid spawn
        spawn_timer += dt;
            if(spawn_timer>=spawn_interval && asteroids.size()<5)
            {
                spawn_timer = 0.0f;
            float ax = (float)(rand() % (int)screenX);
            float ay = (float)(rand() % (int)screenY);
            RigidBody asteroid(ax, ay, rand()%40 + 10, 20, 0.8f, 0.0f);            
            sf::CircleShape gr_asteroid;

            asteroid.velocity = Vec2((rand()%100 - rand()%100), (rand()%100 - rand()%100));
            // negative X drifts left, positive Y drifts downward on screen

            gr_asteroid.setFillColor(sf::Color(std::min(255,rand()/60), std::min(255,rand()/70), std::min(255,rand()/80)));
            gr_asteroid.setOutlineColor(sf::Color(std::min(255,rand()/60), std::min(255,rand()/70), std::min(255,rand()/80)));
            gr_asteroid.setOutlineThickness(1.0);
            gr_asteroid.setPosition({asteroid.position.x-asteroid.radius,screenY-asteroid.position.y-asteroid.radius});
            gr_asteroid.setRadius(asteroid.radius);

            asteroids.push_back(asteroid);
            gr_asteroids.push_back(gr_asteroid);
            }
        
        //Arrow movement
        if(sf::Keyboard::isKeyPressed (sf::Keyboard::Key::Left))
        ship.velocity = Vec2(-90.0f,0.0f);
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
        ship.velocity = Vec2(90.0f,0.0f);
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        ship.velocity = Vec2(0.0f,90.0f);
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        ship.velocity = Vec2(0.0f,-90.0f);
        else
        ship.velocity = Vec2(0.0f, 0.0f);

        for(int i = bullets.size()-1; i>=0;i--)
        {
            bool bulletremoved = false;
            RigidBody& bullet = bullets[i];
            bullet.life += dt;

            //Asteroid Removal
        for(int j = asteroids.size()-1; j>=0; j--)
        {
            //Remove when out of window
            if(asteroids[j].position.x<= 0.0f || asteroids[j].position.x>=screenX || asteroids[j].position.y<= 0.0f || asteroids[j].position.y>= screenY)
            {
                asteroids.erase(asteroids.begin()+j);
                gr_asteroids.erase(gr_asteroids.begin()+j);
                continue;
            }

            float radii = bullet.radius + asteroids[j].radius;
            Vec2 diff = bullet.position-asteroids[j].position;
            Vec2 dir = diff.getdirection();
            float dist = diff.length();
            
            //Remove when struck by bullet
            if(dist<=radii)
            {
                asteroids.erase(asteroids.begin()+j);
                gr_asteroids.erase(gr_asteroids.begin()+j);

                bullets.erase(bullets.begin()+i);
                gr_bullets.erase(gr_bullets.begin()+i);
                bulletremoved = true;
                points = points+1;
                break;
                               
            }

        }
         //Check Life
            if(!bulletremoved && bullet.lifeCheck())
            {
                bullets.erase(bullets.begin()+i);
                gr_bullets.erase(gr_bullets.begin()+i);
            }

    }

        //Asteroid Collision with Asteroid
        
        for(int j = asteroids.size()-1; j>=0; j--)
        {
            bool asteroidremoved = false;
            //Remove on asteroid collision with asteroid
            for(int k = j-1; k>=0; k--)
            {
                Vec2 diff = asteroids[k].position - asteroids[j].position;
                float coll_dist = diff.length();
                float coll_cond = asteroids[k].radius + asteroids[j].radius;

                if(coll_dist<=coll_cond)
                {
                    asteroids.erase(asteroids.begin()+j);
                    asteroids.erase(asteroids.begin()+k);
                    gr_asteroids.erase(gr_asteroids.begin()+j);
                    gr_asteroids.erase(gr_asteroids.begin()+k);
                    asteroidremoved = true;
                    break;
                }

            }
            if(asteroidremoved)
                continue;
        }
        //Ship Collision with Asteroid
        for(int i = asteroids.size()-1; i>=0;i--)
        {
            float radii = ship.radius + asteroids[i].radius;
            Vec2 diff = ship.position-asteroids[i].position;
            Vec2 dir = diff.getdirection();
            float dist = diff.length();

            if(dist<=radii)
            {
                asteroids.erase(asteroids.begin()+i);
                gr_asteroids.erase(gr_asteroids.begin()+i);

                shipAlive = false;
            }
        }
        //Ship Update
        ship.linear_update(dt);
        ship.position.x = std::max(ship.radius, std::min((float)screenX - ship.radius, ship.position.x));
        ship.position.y = std::max(ship.radius, std::min((float)screenY - ship.radius, ship.position.y));
        gr_ship.setPosition({ship.position.x- ship.radius, screenY-ship.position.y-ship.radius});
    
        //Draw
        window.draw(gr_ship);
    }

        
        for(int i = 0; i<asteroids.size();i++)
        {
            asteroids[i].linear_update(dt);  
            gr_asteroids[i].setPosition({asteroids[i].position.x-asteroids[i].radius,screenY-asteroids[i].position.y-asteroids[i].radius});         
            window.draw(gr_asteroids[i]);
        }

        for(int i = 0; i< bullets.size();i++)
        {
            bullets[i].linear_update(dt);
            gr_bullets[i].setPosition({bullets[i].position.x,screenY-bullets[i].position.y});       
            window.draw(gr_bullets[i]);
        }

        //Text Rendering
        txt_point.setString("Point = " + std::to_string(points));

        if(mag<=0)
        {
            reloading = true;
        }

        if(reloading)
        {
            reload_timer += dt;

            if(reload_timer>=reload_time)
            {
                mag = 30;
                reloading = false;
                reload_timer = 0.0f;
            }
            
        }
        
        if(reloading)
        txt_reload.setString("Reloading... " + std::to_string((int)(reload_time - reload_timer) + 1) + "s");
        else
        txt_reload.setString("To Reload - [A]");

        txt_mag.setString("Magazine = " + std::to_string(mag));

        window.draw(txt_point);
        window.draw(txt_move);
        window.draw(txt_reset);
        window.draw(txt_reload);
        window.draw(txt_space);
        window.draw(txt_mag);
        window.draw(txt_color);
        window.display();
        
        
        
    }
 }
