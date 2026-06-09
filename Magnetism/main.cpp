#include"Charge_Particle.cpp"
#include<SFML/Graphics.hpp>



int main()
{
    Particle wire(450.0f,350.0f,10.0f,10.0f,1.0f,100.0f);
    Particle obj(400.0f,400.0f,9.1*(std::pow(10,-1)),10.0f,1.0f,-1.6*(std::pow(10,-1)));
    float I = 2.0f*std::pow(10,2);
    float k = 8.7f *3.14f* std::pow(10,-1); 

    //GR

    sf::CircleShape gr_wire;
    sf::CircleShape gr_obj;

    //obj
    gr_obj.setFillColor(sf::Color::Red);
    gr_obj.setOutlineColor(sf::Color::Blue);
    gr_obj.setOutlineThickness(0.7f);
    gr_obj.setRadius(obj.radius);
    gr_obj.setPosition({obj.position.x-obj.radius, 720.0f - obj.position.y-obj.radius});

    //obj gr
    gr_wire.setFillColor(sf::Color::Yellow);
    gr_wire.setOutlineColor(sf::Color::Blue);
    gr_wire.setOutlineThickness(0.7f);
    gr_wire.setRadius(wire.radius);
    gr_wire.setPosition({wire.position.x-wire.radius, 720.0f - wire.position.y-wire.radius});

    sf::RenderWindow window(sf::VideoMode({1080u,720u}),"Magnetism");
    window.setFramerateLimit(90);
    float dt = 1/90.0f;

    while(window.isOpen())
    {
        window.clear();
        while(std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }

           }
           // Replace your KeyPressed event block with this, outside the event loop:
         

         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
             obj.velocity = Vec2(0.0f, 50.0f);
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
             obj.velocity = Vec2(0.0f, -50.0f);
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
             obj.velocity = Vec2(-50.0f, 0.0f);
         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
             obj.velocity = Vec2(50.0f, 0.0f);

           float Bz = MagneticField(wire,obj,I,k);
           Vec2 force = obj.MagneticForce(Bz);
           
           obj.applyforce(force);
           obj.update(dt);

           gr_obj.setPosition({obj.position.x-obj.radius, 720.0f - obj.position.y-obj.radius});

           window.draw(gr_wire);
           window.draw(gr_obj);
           window.display();
    }
}