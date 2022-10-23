#pragma once
#include <vector> 
#include <SDL.h>
#define err 0.0001
//typedef struct Color
class Vector;

class Point
{
public:
	float x, y,z;
	Point();
	Point(float,float,float);
	Point operator+(const Vector v) const;
	Vector operator-(const Point p) const;
};

class Vector:public Point
{
public:
	Vector();
	Vector(Point);
	float Length();
	void normalize();
	Vector Rotate(float aplha, float beta, float gamma) const;
	Vector operator+(const Vector v) const;
	Vector operator-(const Vector v) const;
	Vector& operator+=(const Vector v);
	Vector& operator-=(const Vector v);
	Vector operator*(const float num) const;
	Vector operator*(const Vector v) const;
	Vector operator/(const float num) const;
	Vector& operator*=(const float num);
	float operator^(const Vector v) const;
};


class Ray
{
public:
	Point pt;
	Vector dir;
	float len;
	Ray();
	Ray(float);
};

typedef struct Polygon
{
	Point points[3];
	Vector norm;
};

class Material
{
public:
	SDL_Color color;
	float mirroring;
	float refraction;
	float light_glaring;
	float transparency;
};

typedef struct Raycast_ret
{
	std::vector<Point> pts;
	Vector norm;
};

class Object
{
public:
	float rotation_x=0, rotation_y=0, rotation_z=0;
	Material mat;
	Point pt;
	virtual Raycast_ret Raycast(Ray) { return Raycast_ret();}
	virtual void Rotate(float, float, float) {}
	virtual void Resize(float, float, float) {}
	virtual Ray Refract(Point, Ray) { return Ray(); }
};
enum Xtype { ambient, point, directional };

class Light
{
public:
	float brightness;
	
	Xtype type;
	Vector dir;
	SDL_Color cl;
	Point pt;
};

class Sphere : public Object
{	
public:
	Sphere() {
		rad = 0;
	}
	Sphere(float r) {
		rad = r;
	}
	float rad;
	Raycast_ret Raycast(Ray);
	void Rotate(float, float, float) {};
	void Resize(float, float, float);

};



class Poly_object : public Object
{
private:
	Vector norm = Point{ 0,0,0 };
	Point center;
	
public:
	std::vector<Polygon> orig;
	std::vector<Polygon> viewed;
	Raycast_ret Raycast(Ray);
	void Rotate(float, float, float);
	void Resize(float, float, float){};

};