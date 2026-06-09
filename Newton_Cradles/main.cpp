#include"RigidBody.cpp"
#include"SFML/Graphics.hpp"
#include<vector>

int main()
{
    //Particles
    std::vector<RigidBody> balls;
    std::vector<Vec2> anchors;
    float s_length = 400.0f;

    //Graphics
    std::vector<sf::CircleShape> gr_anchors;
    std::vector<sf::CircleShape> gr_balls;
    std::vector<sf::RectangleShape> gr_strings;
    sf::RectangleShape gr_Ceil;
    sf::VertexArray gr_ropes(sf::PrimitiveType::Lines,10);

    //Set Ceil
    gr_Ceil.setFillColor(sf::Color(100.0f,200.0f,200.0f,150.0f));
    gr_Ceil.setSize({720.0f,20.0f});
    gr_Ceil.setPosition({0.0f,110.0f});

    //Text
    sf::Font f;
    if(!f.openFromFile("C://Windows//Fonts//GARA.ttf"))
    return -1;

    sf::Text lmb(f, "To Move A Bob - LMB", 12);
    lmb.setPosition({20.0f, 20.0f});
    lmb.setFillColor(sf::Color::White);

    //Window
    unsigned screenX = 720;
    unsigned screenY = 720;
    sf::RenderWindow window(sf::VideoMode({screenX,screenY}), "Cradles");
    window.setFramerateLimit(90);
    float dt = 1.0f/90.0f;

    //Initialize
        for(int i = 0; i<5; i++)
        {
        
            Vec2 anchor(300.0f + i * 40.0f, 600.0f);
            RigidBody ball(anchor.x,anchor.y-s_length,20.0f,10.0f,0.9f,0.0f);
            
            sf::CircleShape gr_anchor;
            gr_anchor.setFillColor(sf::Color::Cyan);
            gr_anchor.setOutlineColor(sf::Color::White);
            gr_anchor.setOutlineThickness(1.0f);
            gr_anchor.setRadius(10.0f);
            gr_anchor.setPosition({anchor.x-10.0f, screenY - anchor.y-10.0f});

            sf::CircleShape gr_ball;
            gr_ball.setFillColor(sf::Color::White);
            gr_ball.setOutlineColor(sf::Color::Blue);
            gr_ball.setOutlineThickness(1.0f);
            gr_ball.setRadius(ball.radius);
            gr_ball.setPosition({ball.position.x-ball.radius, screenY - ball.position.y-ball.radius});           
            
            gr_ropes[i*2].position = {gr_anchor.getPosition().x + 10.0f, gr_anchor.getPosition().y + 10.0f};
            gr_ropes[i*2+1].position = {gr_ball.getPosition().x + gr_ball.getRadius(), gr_ball.getPosition().y + gr_ball.getRadius()};
            
            gr_ropes[i*2].color = sf::Color(100.0,200.0,200.0,220.0);
            gr_ropes[i*2+1].color = sf::Color(100.0,200.0,200.0,220.0);

            balls.push_back(ball);
            anchors.push_back(anchor);
            gr_balls.push_back(gr_ball);
            gr_anchors.push_back(gr_anchor);
            

        }

    //Physics loop
    while(window.isOpen())
    {
        window.clear();
        while(std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }
            
            //Adding position
            else if(event->is<sf::Event::MouseButtonPressed>())
            {
                auto mb = event->getIf<sf::Event::MouseButtonPressed>();

                if(mb->button == sf::Mouse::Button::Left)
                {
                    Vec2 mpos = Vec2(mb->position.x,mb->position.y);
                    balls[balls.size()-1].position = Vec2(mpos.x, screenY - mpos.y);
                }
            }
        }

        //Logic
        for(int i = 0; i<balls.size();i++)
        {
            RigidBody& ball = balls[i];
            Vec2& anchor = anchors[i];
            ball.applyforce(Vec2(0.0f,-9.8*ball.mass));

            //Rope Logic
            Vec2 diff = anchor - ball.position;
            float len = diff.length();

            if(len>=s_length)
            {
                Vec2 dir = diff.getdirection();
                ball.position = anchor-dir*s_length;
                float pull = ball.velocity.dot(dir);

                if(pull<0)
                {
                    ball.velocity -= dir*pull;
                }
            }

            //Ceiling coll
             if(ball.position.y + ball.radius >= screenY - 10.0f)
              {
                    ball.position.y = screenY - 10.0f - ball.radius;
                    ball.velocity.y = -std::abs(ball.velocity.y)*ball.restitution;
              }
                

            //Ball Collision logic
            for(int j = i+1; j<balls.size(); j++)
            {
                RigidBody& ball2 = balls[j];
                float radii = ball.radius + ball2.radius;
                Vec2 dist = ball2.position - ball.position;
                Vec2 dist_dir = dist.getdirection();
                float dist_l = dist.length();
                Vec2 rel_vel = ball.velocity-ball2.velocity;
                float coll_vel = rel_vel.dot(dist_dir);
                float e = std::min(ball.restitution,ball2.restitution);

                if(dist_l<= radii)
                {
                    float overlap = radii - dist_l;
                    ball.position -= dist_dir*(overlap*0.5f);
                    ball2.position += dist_dir*(overlap*0.5f);

                    if(coll_vel>0)
                    {
                        float impulse = ((e+1)*coll_vel)/((1.0f/ball.mass) +(1.0f/ball2.mass));
                        ball.velocity -= dist_dir*(impulse/ball.mass);
                        ball2.velocity += dist_dir*(impulse/ball2.mass);
                    }
                }

               
            }
            ball.linear_update(dt); 
            gr_balls[i].setPosition({ball.position.x-ball.radius, screenY - ball.position.y-ball.radius});
            gr_ropes[i*2].position = {gr_anchors[i].getPosition().x + 10.0f, gr_anchors[i].getPosition().y +10.0f};
            gr_ropes[i*2+1].position = {gr_balls[i].getPosition().x + ball.radius, gr_balls[i].getPosition().y + ball.radius};
            
                
            
            window.draw(gr_balls[i]);
            window.draw(gr_anchors[i]);
        }

        window.draw(gr_ropes);
        window.draw(gr_Ceil);
        window.draw(lmb);
        window.display();
    }

}