#include<iostream>
#include<math.h>
#include<sstream>
#include<iomanip>

struct Vec2
{
    float x,y;

    Vec2(){}

    Vec2 (float x, float y): x(x), y(y){}

    Vec2 operator+(const Vec2& v)const
    {
        return Vec2(x+v.x,y+v.y);
    }

    Vec2 operator-(const Vec2& v)const
    {
        return Vec2(x-v.x, y-v.y);
    }

    Vec2 operator*(float a)const
    {
        return Vec2(x*a,y*a);
    }

    Vec2& operator+=(const Vec2& v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2 operator/(float a)const
    {
        if(a!=0)return Vec2(x/a,y/a);
        else return Vec2(0.0f,0.0f);
    }

    float length() const
    {
        return std::sqrt(x*x+y*y);
    }

    Vec2 getdirection() const
    {
        float len = length();
        if(len ==0)
        {
            return Vec2(0,0);
        }      
        return Vec2(x/len, y/len);
    }

    float dot(const Vec2& v) const
    {
        return x*v.x +y*v.y;
    }

    float cross(const Vec2& v)const
    {
        return x*v.y - y*v.x;
    }

    void print()const
    {
        std :: cout<<"("<<x<<","<<y<<")"<<"\n";
    }

};

struct RigidBody
{
    //Linear
    Vec2 position;
    Vec2 force;
    Vec2 velocity;
    float mass;
    float radius;
    float restitution;

    //Angular
    float theta;
    float ang_vel;
    float torque;
    float inertia;

    //Life
    float life = 0.0f;
    float total_life = 0.0f;

    RigidBody(){}


    RigidBody(float x, float y, float m, float r, float rest, float θ): position(x,y), force(0.0f,0.0f), velocity(0.0f,0.0f), 
    mass(m), radius(r), restitution(rest), theta(θ), ang_vel(0.0f), torque(0.0f), inertia(0.5f * m*r*r) {}

    void applyforce(const Vec2& f)
    {
        force += f;
    }


    void applytorque(const Vec2& d, const Vec2& f)
    {
        torque += d.cross(f);
    }

    void angular_update(float dt)
    {
        float ang_acc = torque/inertia;
        
        ang_vel += ang_acc*dt;
        theta += ang_vel*dt;
        torque = 0.0f;
    }
    
    void linear_update(float dt)
    {
        Vec2 acceleration = force *(1.0f/mass); //Acc too is a vector in phy
        //Euler Integration

        velocity += acceleration*dt;
        position += velocity*dt;
        force = Vec2(0.0f,0.0f);
    }

    void update(float dt)
    {
        linear_update(dt);
        angular_update(dt);
    }

    bool lifeCheck()
    {
        if(life>=total_life)
        return 1;
        else
        return 0;
    }

    void print(float tick) const
    {
        std :: cout<<"Tick = "<<tick<<" | Velocity = ("<<velocity.x<<","<<velocity.y<<") |"<<"Position = ("<<position.x<<","<<position.y<<") \n";

    }


};
  
Vec2 rebound(const RigidBody& v, const Vec2& n)
{
    Vec2 n_dir = n.getdirection();
    float dp = v.velocity.dot(n_dir);
    
    if(dp<0)
    {
        return Vec2((v.velocity.x-n_dir.x*2*dp)*v.restitution, (v.velocity.y-n_dir.y*2*dp)*v.restitution); // if n_dir replaced by n, the velocity will get boosted by its magnitude. So, using only direction will be the best
    }
    else
    {
        return v.velocity;
    }
    
}

void linear_collision( RigidBody& a , RigidBody& b)
{
    Vec2 diff = b.position - a.position;
    Vec2 dir = diff.getdirection();
    float dist = diff.length();
    Vec2 rel_vel = a.velocity - b.velocity;
    float coll_vel = rel_vel.dot(dir);
    float radii = a.radius + b.radius;
    float e = std::min(a.restitution,b.restitution);

    if(dist<=radii)
    {
        float overlap = radii-dist;
        a.position -= dir*(overlap*0.5f);
        b.position += dir*(overlap*0.5f);
        if(coll_vel>0)
        {
            float impulse = ((e+1)*coll_vel)/((1.0f/a.mass)+(1.0f/b.mass));
            a.velocity -= dir*(impulse/a.mass);
            b.velocity += dir*(impulse/b.mass);
        }
    }
}
Vec2 setGraphicsPosition(Vec2& a, const float& r, const unsigned& y)
{
    return Vec2(a.x-r, y - a.y-r);
}

void wall_collision(RigidBody& b, const Vec2& norm, Vec2& contact)
{
    Vec2 contact_vel = b.velocity + Vec2(-b.ang_vel*contact.y,b.ang_vel*contact.x);
    float coll_vel = contact_vel.dot(norm);            
                
    if(coll_vel<0)
    {
    float  rot_inertia = ((contact.cross(norm))*(contact.cross(norm)))/b.inertia;
    float lin_inertia = 1.0f/b.mass;
    float tot_inertia = rot_inertia+lin_inertia;
    
    float impulse = (-(b.restitution+1)*coll_vel)/tot_inertia;
    //apply impulse

    b.velocity += norm*(impulse/b.mass);
    b.ang_vel += (contact.cross(norm*impulse)/b.inertia);  //Formula for angular acc
    }
}

Vec2 rotate(const Vec2& v,const float theta)
{
    float x = v.x*std::cos(theta) - v.y*std::sin(theta);
    float y = v.x*std::sin(theta) + v.y*std::cos(theta);
    return Vec2(x,y);
}
