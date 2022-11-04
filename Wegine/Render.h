#pragma once
#include <SDL.h>
#ifdef NDEBUG

#endif
#include "geometry.h"
#include <vector>

#define THREADS 1

const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 320;
bool init();
void quit();
void set_pixel(int x, int y, SDL_Color cl);

extern SDL_Window* win;
extern SDL_Surface* surf;



class Cam
{
public:
	float max_view;
	Point pt;
	int Res_x, Res_y;
	float angle;
	Vector dir;
	cl_ray* rays;
	Cam(int, int,float,int);
	Cam();
	float sX, dX, dY;
};




class Scene
{
private:
#ifdef NDEBUG
	Raycast_ret* rets;
	cl::Buffer ray_buff;
	cl::Buffer rets_buff;
	Kernel_program* kern_pr;
#endif
public:
	Scene();
	Scene(Kernel_program&);
	void set_program(Kernel_program& pr);
	Cam cam;
	int Render_depth = 10;
	Material void_mat = {{0,0,0},0,1,0,0};
	std::vector<Object*> objs;
	std::vector<Light*> lights;
	void render();
	SDL_Color Raytrace(Ray,int,float,Material*);
	void new_render_thread(int,int);
	float LightCompute(Point,Vector, float,Vector,float);

#ifdef NDEBUG
	void kernel_raytrace(int,float,Material*);
#endif // NDEBUG

};
