#include"Vec_Particle_Collision.cpp"
#include<SFML/Graphics.hpp>
#include<vector>
#include<deque>

int main()
{ 

    //Particles in the system

    std::vector<Particle> Bodies;
    Particle UFO(100.0,200.0,500.0,10.0,1.0);
    float G = 6.67 * std::pow(10,0);
    Particle sun(540.0f,360.0f,1000000.0f,20.0f,1.0f);
    float maxVel = 0.0f;

    //Graphics

    std::vector<sf::CircleShape> gr_Bodies;
    sf::CircleShape gr_UFO;
    sf::CircleShape gr_sun;

    //Setup UFO

    gr_UFO.setRadius(UFO.radius);
    gr_UFO.setFillColor(sf::Color::Yellow);
    gr_UFO.setOutlineColor(sf::Color::Red);
    gr_UFO.setOutlineThickness(0.9);

    //Setup Sun

    gr_sun.setFillColor(sf::Color::White);
    gr_sun.setRadius(sun.radius);
    gr_sun.setPosition({sun.position.x-sun.radius, 720.0f- sun.position.y-sun.radius});

    //Setup Text
    sf::Font f;
    if(!f.openFromFile("C://Windows//Fonts//GARA.ttf"))
    return -1;

    sf::Text count(f, "", 12);
    sf::Text move(f, "To move - Arrow Keys", 12);
    sf::Text incW(f,"Increase Weight - W ",12);
    sf::Text decW(f,"Decrease Weight - S ", 12);
    sf::Text maxV(f, "", 12);
    std::ostringstream ss;

    //Initialize Text
    count.setFillColor(sf::Color::White);
    count.setPosition({20.0f,30.0f});

    move.setFillColor(sf::Color::White);
    move.setPosition({20.0f,50.0f});

    incW.setFillColor(sf::Color::White);
    incW.setPosition({20.0f,70.0f});

    decW.setFillColor(sf::Color::White);
    decW.setPosition({20.0f,90.0f});

    maxV.setFillColor(sf::Color::White);
    maxV.setPosition({20.0f,110.0f});


    //Window open

    unsigned int screenX = 1080u;
    unsigned int screenY = 720u;
    sf::RenderWindow window(sf::VideoMode({screenX,screenY}),"Rocket Sim");
    window.setFramerateLimit(90);
    float dt = 1.0f/90.0f;

    while(window.isOpen())
    {
        window.clear();
        while(std::optional event = window.pollEvent())
        {
            //Close Event
            if(event->is<sf::Event::Closed>())
            {
                window.close();
            }

            //Mouse Event - Add Planets
            else if(event->is<sf::Event::MouseButtonPressed>())
            {
                auto mb = event->getIf<sf::Event::MouseButtonPressed>();

                //Add Planets
                if(mb->button == sf::Mouse::Button::Left)
                {
                    Vec2 mpos = Vec2(mb->position.x,screenY-mb->position.y);
                    Particle planet(mpos.x,mpos.y,50.0f,12.0f,1.0f);
                    sf::CircleShape gr_planet;
                    gr_planet.setRadius(planet.radius);
                    gr_planet.setFillColor(sf::Color::Blue);
                    gr_planet.setPosition({mpos.x,screenY-mpos.y});
                    
                    Vec2 dis = (sun.position-planet.position);
                    float vel = std::sqrt(G*sun.mass/dis.length());
                    planet.velocity = Vec2(-dis.getdirection().y,dis.getdirection().x)*vel;
                    
                    Bodies.push_back(planet);
                    gr_Bodies.push_back(gr_planet);
                }

            }

            //Keyboard Events

            else if(event->is<sf::Event::KeyPressed>())
            {
              auto kb = event->getIf<sf::Event::KeyPressed>();

              //Rocket Jump
              if(kb->code == sf::Keyboard::Key::Up)
              {
                UFO.velocity = Vec2(0.0f,50.0f);
              }

              //Rocket Dodge left
              if(kb->code == sf::Keyboard::Key::Left)
              {
                UFO.velocity = Vec2(-50.0f,0.0f);
              }

              //Rocket Dodge right
              if(kb->code == sf::Keyboard::Key::Right)
              {
                UFO.velocity = Vec2(50.0f,0.0f);
              }

              //Rocket Down
              if(kb->code == sf::Keyboard::Key::Down)
              {
                UFO.velocity = Vec2(0.0f,-50.0f);
              }

            }

        }
        
        //Increse mass of latest planet
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            if(Bodies.size()>0)
            Bodies[Bodies.size()-1].mass += 50.0f;
        }

        //Decrease mass of latest planet
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            if(Bodies.size()>0)
            Bodies[Bodies.size()-1].mass -= 10.0f;
        }
        
        //Sun to UFO gravity
        Vec2 diff_sufo = sun.position-UFO.position;
        float dist_sufo = diff_sufo.length();
        Vec2 sufo_dir = diff_sufo.getdirection();
        float sufo_f = (G*UFO.mass*sun.mass)/(dist_sufo*dist_sufo);
        Vec2 sufo_gf = sufo_dir * sufo_f;
        UFO.applyforce(sufo_gf);


        Vec2 diff(0.0f,0.0f);
        float dist = 0.0f;
        Vec2 UFO_pl(0.0f,0.0f);
        float dist_UFO = 0.0f;
        
        for(int i=0; i<Bodies.size(); i++)
        {

            //Max Velo
                if(Bodies[i].velocity.length()>maxVel)
                maxVel = Bodies[i].velocity.length();
                
            for(int j = i+1; j< Bodies.size(); j++)
            {
                diff = Bodies[j].position - Bodies[i].position;
                dist = diff.length();
                Vec2 dist_dir = diff.getdirection();

                float force = (G*Bodies[j].mass*Bodies[i].mass)/(dist*dist);
                Vec2 gforce = dist_dir * force;

                Bodies[i].applyforce(gforce);
                Bodies[j].applyforce(gforce*-1);
         

            }

            diff = sun.position - Bodies[i].position;
            dist = diff.length();
            Vec2 dist_dir = diff.getdirection();

            float force = (G*Bodies[i].mass*sun.mass)/(dist*dist);
            Vec2 sforce = dist_dir*force;

            Bodies[i].applyforce(sforce);
            
            //UFO gravity

            UFO_pl = Bodies[i].position - UFO.position;
            dist_UFO = UFO_pl.length();
            Vec2 UFO_Dir = UFO_pl.getdirection();

            float ufo_force = (G*Bodies[i].mass*UFO.mass)/(dist_UFO*dist_UFO);
            Vec2 ufo_gforce = UFO_Dir * ufo_force;

            UFO.applyforce(ufo_gforce);
            
            Bodies[i].update(dt);
            
            gr_Bodies[i].setPosition({Bodies[i].position.x-Bodies[i].radius, screenY-Bodies[i].position.y-Bodies[i].radius});
            window.draw(gr_Bodies[i]);
        }

        UFO.update(dt);

        gr_UFO.setPosition({UFO.position.x - UFO.radius, screenY - UFO.position.y-UFO.radius});

        //Set Text Rendering
        count.setString("Planets = " + std::to_string(Bodies.size()));
        ss<<std::fixed<<std::setprecision(2)<<maxVel;
        maxV.setString("Max Speed = "+ ss.str());
        ss.str("");
        ss.clear();

        window.draw(gr_sun);
        window.draw(gr_UFO);
        window.draw(count);
        window.draw(move);
        window.draw(incW);
        window.draw(decW);
        window.draw(maxV);
        window.display();
        

    }


}