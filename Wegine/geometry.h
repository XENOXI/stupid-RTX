#pragma once
#include <vector> 
#include <SDL.h>
#include "threading.h"
#define COMPUTING_ERROR 0.0001
//typedef struct Color
class Vector;



class Point
{
public:
	float x, y,z;
	Point();
	Point(float,float,float);
	operator cl_float3() const;
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



typedef struct Ray
{
	Vector dir;
	Point pt;
};

std::ostream& operator<<(std::ostream& out, const Ray& r);

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
	cl_float3 pt;
	cl_float3 norm;
	float dist;
};

class Object
{
public:
	float rotation_x=0, rotation_y=0, rotation_z=0;
	Material mat;
	Point pt;
#ifdef NDEBUG
	virtual void Raycast(cl_ray*, long long, Kernel_program&, Raycast_ret*,cl::Buffer& buff, cl::Buffer& rets_buff)=0;
#endif
	virtual Raycast_ret Raycast(Ray) = 0;
	virtual void Rotate(float, float, float) {}
	virtual void Resize(float, float, float) {}

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
private:
#ifdef NDEBUG
	typedef struct cl_sphere
	{
		cl_float rad;
		cl_float3 pt;
	};
#endif // NDEBUG
public:


	Sphere() {
		rad = 0;
	}
	Sphere(float r) {
		rad = r;
	}
	float rad;
	Raycast_ret Raycast(Ray);
#ifdef NDEBUG
	void Raycast(cl_ray*, long long, Kernel_program&, Raycast_ret*, cl::Buffer& buff, cl::Buffer& rets_buff);
#endif
	void Rotate(float, float, float) {};
	void Resize(float, float, float);

};



class Poly_object : public Object
{
private:
	Vector norm = Point{ 0,0,0 };
	Point center;
public:
	std::vector<cl_polygon> cl_viewed;
	std::vector<cl_polygon> cl_orig;
	std::vector<Polygon> orig;
	std::vector<Polygon> viewed;
	Raycast_ret Raycast(Ray);
#ifdef NDEBUG
	void Raycast(cl_ray*, long long, Kernel_program&,Raycast_ret*, cl::Buffer& buff, cl::Buffer& rets_buff);
#endif
	void Rotate(float, float, float);
	void Resize(float, float, float){};

};