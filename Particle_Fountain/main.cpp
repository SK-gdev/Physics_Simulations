#include"RigidBody.cpp"
#include<SFML/Graphics.hpp>
#include<vector>
#include<deque>

int main()
{
    std::vector<RigidBody> balls;
    std::vector<std::deque<Vec2>> trails;
    sf::VertexArray trailsVA(sf::PrimitiveType::Points);
    int count = 0;  

    //Graphics
    std::vector<sf::CircleShape> gr_balls;
    std::vector<sf::RectangleShape> gr_swall;
    std::vector<sf::RectangleShape> gr_ywall;

    //Windows
    unsigned screenX = 840;
    unsigned screenY = 720;

    //Wall
    float leftWall   = 10.0f;
    float rightWall  = screenX - 10.0f;
    float bottomWall = 10.0f;
    float topWall    = screenY - 10.0f;    

    sf::RenderWindow window(sf::VideoMode({screenX,screenY}), "Particle Fountain");
    window.setFramerateLimit(90);
    float dt = 1.0f/90.0f;

    while(window.isOpen())
    {
        window.clear();
        
        //Draw side walls
        for(int i = 0; i<2; i++)
        {
            sf::RectangleShape w;
            w.setSize({10.0f,screenY});
            w.setFillColor(sf::Color::Cyan);
            w.setOutlineColor(sf::Color::Yellow);
            w.setOutlineThickness(0.7f);
            if(i ==1)
            w.setPosition({screenX-10.0f,0.0f});
            else
            w.setPosition({0.0f,0.0f}); 
            
            Vec2 wall(w.getPosition().x, screenY - w.getPosition().y);
            window.draw(w);
            

        }
        //Draw y wall
        for(int i = 0; i<2; i++)
        {
            sf::RectangleShape w;
            w.setSize({screenX,10.0f});
            w.setFillColor(sf::Color::Cyan);
            w.setOutlineColor(sf::Color::Yellow);
            w.setOutlineThickness(0.7f);
            if(i ==1)
            w.setPosition({0.0f,screenY-10.0f});
            else
            w.setPosition({0.0f,0.0f}); 
            
            Vec2 wall(w.getPosition().x, screenY - w.getPosition().y);
            window.draw(w);
        }

        while(std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            window.close();
            
            //Mouse Event 
            else if(event->is<sf::Event::MouseButtonPressed>())
            {
                auto mb = event->getIf<sf::Event::MouseButtonPressed>();

                //Spawn particles
                if(mb->button== sf::Mouse::Button::Left)
                {
                    Vec2 mpos = Vec2(mb->position.x,mb->position.y);
                    for(int i = 0; i<10; i++)
                    {
                        RigidBody ball(mpos.x+(std::rand()%10)- 10.0f,screenY-mpos.y,10.0f,10.0f,0.88f,0.0f);
                        sf::CircleShape gr_ball;

                        ball.velocity = Vec2((std::rand() % 200) - 100.0f, (std::rand() % 200) + 50.0f);

                        gr_ball.setPosition({mpos.x,mpos.y});
                        gr_ball.setFillColor(sf::Color::Blue);
                        gr_ball.setRadius(ball.radius);

                        balls.push_back(ball);
                        gr_balls.push_back(gr_ball);

                        //Trails
                        
                        Vec2 ball_pos = ball.position;
                        trails.push_back(std::deque<Vec2>{ball_pos});

                    } 
                }
            }

        }

        //Collision
        for(int i = 0; i<balls.size(); i++)
        {
            RigidBody& b = balls[i];
            // Apply gravity
            b.applyforce(Vec2(0.0f, -9.8f * b.mass));
        
            // Left wall
            if(b.position.x - b.radius <= 10.0f)
            {
                b.position.x = 10.0f + b.radius;
                b.velocity.x = std::abs(b.velocity.x) * b.restitution;
            }
        
            // Right wall
            if(b.position.x + b.radius >= screenX - 10.0f)
            {
                b.position.x = screenX - 10.0f - b.radius;
                b.velocity.x = -std::abs(b.velocity.x) * b.restitution;
            }
        
            // Bottom wall
            if(b.position.y - b.radius <= 10.0f)
            {
                b.position.y = 10.0f + b.radius;
                b.velocity.y = std::abs(b.velocity.y) * b.restitution;
            }
        
            // Top wall
            if(b.position.y + b.radius >= screenY - 10.0f)
            {
                b.position.y = screenY - 10.0f - b.radius;
                b.velocity.y = -std::abs(b.velocity.y) * b.restitution;
            }
            b.linear_update(dt);
            b.total_life = 5.0f + (rand()%10);
            b.life += dt;
            if(trails[i].size()>40)
            trails[i].pop_front();
            trails[i].push_back(b.position);
            
        }

        for(int i = 0; i< trails.size(); i++)
        {
            for(int j = 0; j< trails[i].size();j++)
            {
                float t = (float)j/trails[i].size();
                float a = 255.0f * t;
                float r = balls[i].radius*t;

                sf::Vertex v;
                v.position = {trails[i][j].x-r,screenY- trails[i][j].y-r};
                v.color = sf::Color(100.0f,100.0f,200.0f,a);
                trailsVA.append(v);

            }
        }
        for(int i = balls.size()-1; i>=0; i--)
    {
            RigidBody& ball = balls[i];
        if(ball.life > ball.total_life)
        {
            balls.erase(balls.begin() + i);
            gr_balls.erase(gr_balls.begin() + i);
            trails.erase(trails.begin() + i);
        }
    }

        for(int i =0; i<balls.size(); i++)
        {
            gr_balls[i].setPosition({balls[i].position.x-balls[i].radius,screenY-balls[i].position.y-balls[i].radius});
            window.draw(gr_balls[i]);
        }

        window.draw(trailsVA);

        window.display();
        count = count +1;
        trailsVA.clear();
    }
}