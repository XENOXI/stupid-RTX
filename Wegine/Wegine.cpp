#include "Render.h"
#include "STL_parser.h"
#include <iostream>

using namespace std;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern SDL_Window* win;
extern SDL_Surface* surf;
int TimePerTick;

int SDL_main(int arhc, char** argv) {
    if (!init()) {
        quit();
        system("pause");
        return 1;
    }



    Poly_object po1 = stl_read("./lowpoly.stl");

    

    Sphere sph;
    sph.rad = 5;
    sph.mat.color = SDL_Color{ 255,0,0 };
    sph.pt = Point{ 0,20,-90 };
    sph.mat.mirroring = 0;
    sph.mat.light_glaring = 0;
    sph.mat.refraction = 1.5;
   
    sph.mat.transparency = 0.5;
    /*
    

    Sphere sph1;
    sph1.rad = 5;
    sph1.mat.color = SDL_Color{ 0,255,0 };
    sph1.pt = Point{ -5,5,20 };
    sph1.mat.mirroring = 0.5;
    sph1.mat.light_glaring = 0;

    Poly_object po;
    po.polys.push_back(Polygon{ Point{0,0,20},Point{0,-3,15},Point{5,-5,20} });
    po.polys.push_back(Polygon{ Point{0,0,20},Point{0,-3,15},Point{-5,-5,20} });
    po.polys.push_back(Polygon{ Point{0,-3,15},Point{5,-5,20},Point{-5,-5,20} });
    po.polys.push_back(Polygon{ Point{0,0,20},Point{5,-5,20},Point{-5,-5,20} });
    po.mat.color = SDL_Color{ 0,0,255 };
    po.mat.mirroring = 0;
    po.mat.light_glaring = 0;

    */

    Light l1; 
    l1.brightness = 1;
    l1.cl = SDL_Color{ 255,255,255 };
    l1.pt = Point{ 0,20,-100 };
    l1.type = point;
    l1.dir = Point{ 0,0,1 };

    Scene sc1; 
    sc1.cam = Cam();
    sc1.cam.pt = { 0,20,-100 };
    /*
        
    sc1.objs.push_back(&sph1);
    sc1.objs.push_back(&po);
    */
    sc1.objs.push_back(&po1);
    sc1.objs.push_back(&sph);
    sc1.lights.push_back(&l1);
 
    int FPS = 10000;
    TimePerTick = 1000/FPS;
    int start = SDL_GetTicks();
    bool right = true;

    //po1.Rotate(0, -1.57, 0);
    while (true)
    {
        SDL_FillRect(surf, NULL, 0x000000);
        sc1.render();
        po1.Rotate(0.1, 0.1, 0.1);
        SDL_UpdateWindowSurface(win);
        int end = SDL_GetTicks();
        cout << end - start<<endl;
        if (end - start < TimePerTick)
            SDL_Delay(TimePerTick-end+start);
        start = end;
    }





    SDL_Delay(5000);

    quit();
    return 0;
}