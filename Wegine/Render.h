#pragma once
#include <SDL.h>
#include "geometry.h"
#include <vector>
#define THREADS 1
bool init();
void quit();
void set_pixel(int x, int y, SDL_Color cl);

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern SDL_Window* win;
extern SDL_Surface* surf;




class Cam
{
public:
	int max_view;
	Point pt;
	int Res_x, Res_y;
	float angle;
	Vector dir;
	Cam(int, int,float,int);
	Cam();
	float sX, dX, dY;
	Ray** rays;	
};

typedef struct interval
{
	int from;
	int to;
};

class Scene
{
public:
	Cam cam;
	int Render_depth = 10;
	Material void_mat = {{0,0,0},0,1,0,0};
	std::vector<Object*> objs;
	std::vector<Light*> lights;
	void render();
	SDL_Color Raytrace(Ray,int,Material*);
	void new_render_thread(int,int);
	float LightCompute(Point,Vector,Vector,float);
};