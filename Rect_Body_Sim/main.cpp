#include"RigidBody.cpp"
#include<SFML/Graphics.hpp>
#include<vector>
#include<deque>

float G = 6.67 * std::pow(10,-1);

struct Verts
{
  Vec2 vertex[4];  
};

float min(float dp[4])
{
    float min = dp[0];
    for(int m = 0; m<4; m++ )
    {
        if(min>dp[m])
        min = dp[m];
    }
    return min;
}

float max(float dp[4])
{
    float max = dp[0];
    for(int n = 0; n<4; n++)
    {
        if(max<dp[n])
        max = dp[n];
    }
    return max;
}


void RectangleSetup(sf::RectangleShape& x, Vec2 pos, Vec2 size, sf::Color fill, sf::Color out, float thick, unsigned screenY)
{
    x.setFillColor(fill);
    x.setOutlineColor(out);
    x.setOutlineThickness(thick);
    x.setSize({size.x,size.y});
    x.setPosition({pos.x,screenY-pos.y});
}

void CircleSetup(sf::CircleShape& x, Vec2 pos, float radius, sf::Color fill, sf::Color out, float thick, unsigned screenY)
{
    x.setFillColor(fill);
    x.setOutlineColor(out);
    x.setOutlineThickness(thick);
    x.setRadius(radius);
    x.setPosition({pos.x-radius,screenY-pos.y-radius});
}

void ConvexRectangleSetup(sf::ConvexShape& x, float& hw, float& hh, sf::Color fill, sf::Color out, float thick)
{
    x.setPointCount(4);
    x.setFillColor(fill);
    x.setOutlineColor(out);
    x.setOutlineThickness(thick);

    //Set Point
    x.setPoint(0, {hw,hh});
    x.setPoint(1, {-hw,hh});
    x.setPoint(2, {-hw,-hh});
    x.setPoint(3, {hw,-hh});
}

void SpawnRect(std::vector<RigidBody>& rects, std::vector<sf::ConvexShape>& gr_rects, RigidBody& sun, std::vector<Verts>& vertixes, std::vector<bool>& isDestroyed, Vec2& mbpos, float screenY)
{
    RigidBody rect(mbpos.x, screenY-mbpos.y, 30.0f, 1.0f, 0.7f, 0.0f);
    bool destroyed = false;
    isDestroyed.push_back(destroyed);
    sf::ConvexShape gr_rect;
    float hw = 10.0f ;
    float hh = 10.0f ;
    ConvexRectangleSetup(gr_rect,hw,hh,sf::Color(std::min(255-rand()%200, 255), std::min(255-rand()%200, 255), std::min(255-rand()%200, 255)),sf::Color::White, 0.7f);
    gr_rect.setPosition({rect.position.x,screenY-rect.position.y});
    
    Verts vertices;
    vertices.vertex[0] = Vec2(hw,hh);
    vertices.vertex[1] = Vec2(-hw,hh);
    vertices.vertex[2] = Vec2(-hw,-hh);
    vertices.vertex[3] = Vec2(hw,-hh);

    Vec2 dis = (sun.position-rect.position);
    
    float vel = std::sqrt(G*sun.mass/dis.length());

    rect.velocity = Vec2(-dis.getdirection().y,dis.getdirection().x)*vel; 

    rect.ang_vel = vel/dis.length();
    rects.push_back(rect);
    vertixes.push_back(vertices);
    gr_rects.push_back(gr_rect);
}

bool SAT(std::vector<RigidBody>& rect, std::vector<Verts>& vertices, Vec2& norm, int i, int j, float& overlap )
{
   RigidBody& recA = rect[i];
   RigidBody& recB = rect[j];

   Verts& vertA = vertices[i];
   Verts& vertB = vertices[j];

   //World vertices
   Vec2 worldA[4] , worldB[4];

   for(int k = 0; k<4; k++)
   {
    worldA[k] = recA.position + rotate(vertA.vertex[k], recA.theta);
    worldB[k] = recB.position + rotate(vertB.vertex[k], recB.theta);
   }

   float dotA[4], dotB[4];
   overlap = 9999999.0f;

   Vec2 axes[4] = {(rotate(Vec2(0.0f,1.0f),recA.theta)), (rotate(Vec2(1.0f,0.0f), recA.theta)), (rotate(Vec2(0.0f,1.0f), recB.theta)), (rotate(Vec2(1.0f,0.0f), recB.theta))};

   for(int k = 0; k<4; k++)
   {
        Vec2& axis = axes[k];

        for(int l =0; l<4 ; l++)
        {
            Vec2& vertexA = worldA[l];
            Vec2& vertexB = worldB[l];

            dotA[l] = vertexA.dot(axis);
            dotB[l] = vertexB.dot(axis);
        }

        float minA = min(dotA);
        float maxA = max(dotA);
        float minB = min(dotB);
        float maxB = max(dotB);

        if(!(minA<=maxB && minB<=maxA))
        return false;

        float depth = std::min(maxA,maxB) - std::max(minA, minB);

        if(overlap > depth)
        {
            overlap = depth;
            norm = axis;            
        }
    }

    //normal should lie from A to B
    Vec2 diff = recB.position - recA.position;
    if(diff.dot(norm)<0)
    norm = Vec2(-norm.x,-norm.y);

    return true;
}

void RecCollision(std::vector<RigidBody>& rect, std::vector<Verts>& vertices, Vec2& norm, float& overlap, int i, int j)
{
    
    RigidBody& rectA = rect[i];
    RigidBody& rectB = rect[j];

    //Position correction
    rectA.position += norm*(overlap*0.5f);
    rectB.position -= norm*(overlap*0.5f);

    //Find contact points
     //World position
    Vec2 worldA[4], worldB[4];
    for(int k = 0 ;k<4; k++)
    {
        worldA[k] = rectA.position + rotate(vertices[i].vertex[k], rectA.theta);
        worldB[k] = rectB.position + rotate(vertices[j].vertex[k], rectB.theta);
    }

    int indA =0;
    float maxA = -9999999.0f;
    int indB = 0;
    float minB = 9999999.9f;

    for(int k = 0; k<4; k++)
    {
        if(maxA<worldA[k].dot(norm))
        {
            maxA = worldA[k].dot(norm);
            indA = k;
        }

        if(minB>worldB[k].dot(norm))
        {
            minB = worldB[k].dot(norm);
            indB = k;
        }
    }

     Vec2 contactA = rotate(vertices[i].vertex[indA], rectA.theta);
     Vec2 contactB = rotate(vertices[j].vertex[indB], rectB.theta);

     Vec2 spinvelA = Vec2(-rectA.ang_vel*contactA.y, rectA.ang_vel*contactA.x);
     Vec2 spinvelB = Vec2(-rectB.ang_vel*contactB.y, rectB.ang_vel*contactB.x);

     Vec2 totA = rectA.velocity + spinvelA;
     Vec2 totB =  rectB.velocity +spinvelB;
     
     Vec2 relvel = totB - totA; //from a to b

     float collvel = relvel.dot(norm);

     float crossA = contactA.cross(norm);
   float crossB = contactB.cross(norm);

   float e = std::min(rectA.restitution, rectB.restitution);

   if(collvel<0)
   {
    float rot_inertia_A = (crossA*crossA)/rectA.inertia;
    float rot_inertia_B = (crossB*crossB)/rectB.inertia;

    float lin_inertia_A = 1.0f/rectA.mass;
    float lin_inertia_B = 1.0f/rectB.mass;
     
    float tot_in_A = rot_inertia_A+lin_inertia_A;
    float tot_in_B = rot_inertia_B+lin_inertia_B;

    float tot_inertia = tot_in_A+tot_in_B;

    float impulse = (-(e+1)*collvel)/tot_inertia;

    //Impulse Application
    rectA.velocity += norm*(impulse/rectA.mass);
    rectB.velocity -= norm*(impulse/rectB.mass);

    rectA.ang_vel += (contactA.cross(norm*impulse))/rectA.inertia;
    rectB.ang_vel -= (contactB.cross(norm*impulse))/rectB.inertia;
   
}

}

void SpawnFragment(std::vector<RigidBody>& rects,
                   std::vector<sf::ConvexShape>& gr_rects,
                   std::vector<Verts>& vertices,
                   std::vector<bool>& isDestroyed,
                   std::vector<float>& destroytimer,
                   Vec2 pos, Vec2 vel, float hw, float hh,
                   unsigned screenY)
{
    RigidBody frag(pos.x, pos.y, 5.0f, 1.0f, 0.7f, 0.0f);
    frag.velocity = vel;
    frag.ang_vel = (rand() % 200 - 100) / 20.0f;

    sf::ConvexShape gr_frag;
    ConvexRectangleSetup(gr_frag, hw, hh,
        sf::Color(rand()%255, rand()%255, rand()%255),
        sf::Color::White, 0.7f);
    gr_frag.setPosition({pos.x, (float)screenY - pos.y});

    Verts v;
    v.vertex[0] = Vec2(hw, hh);
    v.vertex[1] = Vec2(-hw, hh);
    v.vertex[2] = Vec2(-hw, -hh);
    v.vertex[3] = Vec2(hw, -hh);

    float timer = 0.0f;
    destroytimer.push_back(timer);

    rects.push_back(frag);
    gr_rects.push_back(gr_frag);
    vertices.push_back(v);
    isDestroyed.push_back(false);
}
int main()
{
    //Screen
    unsigned screenX = 1080;
    unsigned screenY = 720;
    float frame = 120.0f;
    float dt= 1.0f/frame;

    //Rectangles
    std::vector<RigidBody> rects;
    RigidBody sun(1080/2.0f, 720/2.0f, 500000.0f, 20.0f, 0.7f,0.0f);
    std::vector<Verts> vertices;
    std::vector<bool> isDestroyed;

    //Formula var
    Vec2 normal;
    float overlap;

    //Graphics
    std::vector<sf::ConvexShape> gr_rects;
    sf::CircleShape gr_sun;

    //Trails
    std::vector<RigidBody> frags;
    std::vector<sf::ConvexShape> gr_frags;
    std::vector<Verts> frag_vertices;
    std::vector<bool> frag_isDestroyed;
    std::vector<float> destroytimer;

    //Text
    sf::Font f;
    if(!(f.openFromFile("C://Windows//Fonts//GARA.ttf")))
    return -1;

    sf::Text txt_rmb(f,"Right Click To Spawn ", 12);
    txt_rmb.setPosition({10.0f,10.0f});


    //Setup
    CircleSetup(gr_sun, sun.position, sun.radius, sf::Color::White, sf::Color::Yellow, 0.7f, screenY);

    //Window
    sf::RenderWindow window(sf::VideoMode({screenX,screenY}), "Rect Body Sim");
    window.setFramerateLimit(frame);

    while(window.isOpen())
    {
        window.clear();
        while(std::optional event = window.pollEvent())
        {
            if(event->is<sf::Event::Closed>())
            window.close();

            else if(event->is<sf::Event::MouseButtonPressed>())
            {
                auto mb = event->getIf<sf::Event::MouseButtonPressed>();

                //Spawn
                if(mb->button == sf::Mouse::Button::Left)
                {
                    Vec2 mpos = Vec2(mb->position.x,mb->position.y);
                    SpawnRect(rects, gr_rects,sun,vertices, isDestroyed,mpos, screenY);
                }

            }
        }

    
            //Apply Gravity
            for(int i =0 ; i<rects.size(); i++)
            {
                RigidBody& rectA = rects[i];
                if(!isDestroyed[i])
                {               
                
                for(int j = i+1; j<rects.size(); j++)
                {
                    RigidBody& rectB = rects[j];

                    Vec2 diff = rectB.position - rectA.position;
                    Vec2 dir = diff.getdirection();
                    float dist = diff.length();

                    float g = (G*rectA.mass*rectB.mass)/(dist*dist);
                    Vec2 gforce = dir*g;

                    //Rec Rec collision
                    if(SAT(rects,vertices,normal,i,j,overlap))
                    {
                        RecCollision(rects,vertices,normal,overlap,i,j);
                    }
                    rectA.applyforce(gforce);
                    rectB.applyforce(gforce*-1);
                }                

                //Appply sun gravity
                Vec2 diff = sun.position - rectA.position;
                Vec2 dir = diff.getdirection();
                float dist = diff.length();

                if(dist<=sun.radius + rectA.radius)
                {
                    isDestroyed[i] = true;

                 int numFrags = i+5.0f;
                for(int f = 0; f < numFrags; f++)
                {
                    float angle = (2.0f * 3.14159f * f) / numFrags;
                    Vec2 offset(std::cos(angle), std::sin(angle));
                    Vec2 fragPos = rectA.position + offset * 15.0f;
                    Vec2 fragVel = rectA.velocity + offset * 80.0f; // inherit + explode outward

                    float size = 2.0f + rand() % 4;
                    SpawnFragment(frags, gr_frags, frag_vertices, frag_isDestroyed, destroytimer,
                                  fragPos, fragVel, size, size, screenY);
                                
                }
                continue;
            }

                float g = (G*rectA.mass*sun.mass)/(dist*dist);
                Vec2 gforce = dir*g;
                rectA.applyforce(gforce);
                sun.applyforce(gforce*-1);
                rectA.update(dt);
            
            }

            }
            
            sun.update(dt);

            //Fragment control
            for(int i = 0; i < frags.size(); i++)
        {
            if(frag_isDestroyed[i]) continue;
        
            destroytimer[i] += dt;
        
            if(destroytimer[i] >= 3.0f)
            {
                frag_isDestroyed[i] = true;
                continue;
            }
        
            Vec2 diff = sun.position - frags[i].position;
            Vec2 dir = diff.getdirection();
            float dist = diff.length();
        
            if(dist <= sun.radius + frags[i].radius)
            {
                frag_isDestroyed[i] = true;
                continue;
            }
        
            float g = (G * frags[i].mass * sun.mass) / (dist * dist);
            frags[i].applyforce(dir * g);
            frags[i].update(dt);
        }
            

            for(int i = 0; i<frags.size(); i++)
            {
                if(!frag_isDestroyed[i])
                {
                    gr_frags[i].setPosition({frags[i].position.x, screenY - frags[i].position.y});
                    window.draw(gr_frags[i]);
                }
            }
           

            //Draw
            gr_sun.setPosition({sun.position.x-sun.radius, screenY-sun.position.y-sun.radius});
            window.draw(gr_sun);

            for(int i = 0; i<rects.size(); i++)
            {
                if(!isDestroyed[i])
                {
                gr_rects[i].setPosition({rects[i].position.x , screenY-rects[i].position.y});
                gr_rects[i].setRotation(sf::radians(-rects[i].theta));
                window.draw(gr_rects[i]);
                }
            }
            window.draw(txt_rmb);
        
           window.display();
           
    }
}

