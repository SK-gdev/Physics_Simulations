#include"Charge_Particle.cpp"
#include<SFML/Graphics.hpp>

int main()
{
    Particle nucleus(450.0f,300.0f, 3.34f*std::pow(10,-27), 10.0f,1.0f,1.6f*std::pow(10,-19));
    Particle electron(200.0f,200.0f, 9.1f*std::pow(10,-31), 10.0f,1.0,-1.6f*std::pow(10,-19));
    int n = 1;
    float r0 = 5.29f * std::pow(10,-11);
    float k = 9.0 * std::pow(10,9);

    //Graphics

    sf::CircleShape gr_nucleus;
    sf::CircleShape gr_electron;

    gr_nucleus.setRadius(nucleus.radius);
    gr_nucleus.setFillColor(sf::Color::Blue);
    gr_nucleus.setPosition({nucleus.position.x-nucleus.radius,720.0f - nucleus.position.y-nucleus.radius});

    gr_electron.setRadius(electron.radius);
    gr_electron.setFillColor(sf::Color::Green);
    gr_electron.setPosition({electron.position.x-electron.radius,720.0f - electron.position.y-electron.radius});

    sf::RenderWindow window(sf::VideoMode({1080u,720u}), "Bohr's Model");
    window.setFramerateLimit(90);
    float dt = 1.0f;

    //Imp phy
                  
        float r = n*n*r0;
        float SCALE = 1e12f;
        float r_scaled = r * SCALE;

        float vel = std::abs(electron.charge) * std::sqrt(k/(electron.mass*r_scaled));
        
        electron.position.x = nucleus.position.x + r_scaled;
        electron.position.y = nucleus.position.y;

        electron.velocity = Vec2(0.0f,vel);

    while (window.isOpen())
    {
        window.clear();
        while (std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        Vec2 pos = nucleus.position-electron.position;
        float dist = pos.length();
        Vec2 dist_dir = pos.getdirection();       
        
        float C_force = (k* std::abs(electron.charge)* std::abs(nucleus.charge))/(dist*dist);
        Vec2 force = dist_dir * C_force;
        electron.applyforce(force);
        electron.update(dt);

        gr_electron.setPosition({electron.position.x-electron.radius,720.0f - electron.position.y-electron.radius});
        window.draw(gr_nucleus);
        window.draw(gr_electron);
        window.display();
        


        
    }
    
}