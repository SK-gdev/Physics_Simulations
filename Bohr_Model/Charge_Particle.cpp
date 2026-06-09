#include<iostream>
#include<math.h>

struct Vec2
{
    float x,y;

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

struct Particle
{
    Vec2 position;
    Vec2 force;
    Vec2 velocity;
    float mass;
    float radius;
    float restitution;
    float charge;

    Particle(float x, float y, float m, float r, float rest, float ch): position(x,y), force(0.0f,0.0f), velocity(0.0f,0.0f), mass(m), radius(r), restitution(rest), charge(ch){}

    void applyforce(const Vec2& f)
    {
        force += f;
    }

    

    Vec2 MagneticForce(const float Bz)
    {
        return Vec2(charge*velocity.y*Bz, charge*velocity.x*Bz);
    }
    
    void update(float dt)
    {
        Vec2 acceleration = force *(1.0f/mass); //Acc too is a vector in phy
        //Euler Integration

        velocity += acceleration*dt;
        position += velocity*dt;
        force = Vec2(0.0f,0.0f);
    }

    void print(float tick) const
    {
        std :: cout<<"Tick = "<<tick<<" | Velocity = ("<<velocity.x<<","<<velocity.y<<") |"<<"Position = ("<<position.x<<","<<position.y<<") \n";

    }


};
  
Vec2 rebound(const Particle& v, const Vec2& n)
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

    float MagneticField(const Particle& source, const Particle& p, const float I, const float k)
    {
        Vec2 diff = p.position - source.position;
        float r = diff.length();
        Vec2 diff_dir = diff.getdirection();

        float B = (k*I)/r; //Magnitude
        Vec2 B_dir = Vec2(-diff_dir.y,diff_dir.x);
        float sign = diff_dir.cross(B_dir);
        return B*sign; // if sign is positive, the field is out of plane, else in plane. So, multiplying with sign will give the correct direction to the field

    }
