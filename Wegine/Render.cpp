#include <SDL.h>
#include <iostream>
#include "Render.h"
#include <thread>

using namespace std;


SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;
SDL_Surface* surf = NULL;


bool init() {
    bool ok = true;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0) {
        cout << "Can't init SDL: " << SDL_GetError() << endl;
    }

    win = SDL_CreateWindow("Hueta", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN| SDL_WINDOW_VULKAN);
    if (win == NULL) {
        cout << "Can't create window: " << SDL_GetError() << endl;
        ok = false;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == NULL) {
        cout << "Can't create renderer: " << SDL_GetError() << endl;
        ok = false;
    }

    surf=SDL_GetWindowSurface(win);
    if (surf == NULL) {
        cout << "Can't create renderer: " << SDL_GetError() << endl;
        ok = false;
    }
    SDL_RenderClear(ren);
    return ok;
}

void quit() {
    SDL_DestroyWindow(win);
    win = NULL;

    SDL_DestroyRenderer(ren);
    ren = NULL;
    
    SDL_Quit();
}

void set_pixel(int x, int y, SDL_Color cl)
{
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
        return;
    Uint32* const target_pixel = (Uint32*)((Uint8*)surf->pixels
        + (y) * surf->pitch
        + x * surf->format->BytesPerPixel);
    Uint32 pixel = (cl.r << 16) | (cl.g << 8) | cl.b;
    *target_pixel = pixel;
}



Cam::Cam(int x, int y,float ang,int max_v)
{
    Res_x = x;
    Res_y = y;
    angle = ang*M_PI/180;
    max_view = max_v;
    const int h = y;
    
    dX = 2 * tan(angle / 2);
    sX = dX / (x-1);
    dY = sX* (y-1); 
    rays = new cl_ray[(long long)x*y];
}

Cam::Cam()
{
    *this = Cam(SCREEN_HEIGHT, SCREEN_WIDTH, 70,1000);
}

Scene::Scene()
{
    kern_pr = NULL;
    rets = NULL;

}


Scene::Scene(Kernel_program& pr)
{
    kern_pr = &pr;
    long long resol = (long long)cam.Res_x * cam.Res_y;
    rets = new Raycast_ret[resol];
    cl_int cl_error;
    ray_buff = cl::Buffer(kern_pr->context, (CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR), resol * sizeof(cl_ray), cam.rays, &cl_error);
    rets_buff = cl::Buffer(kern_pr->context, (CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR), resol * sizeof(Raycast_ret),rets, &cl_error);
    Kernel_program::cl_throw_error(cl_error);
}

void Scene::set_program(Kernel_program& pr)
{
    kern_pr = &pr;//TODO 
    long long resol = (long long)cam.Res_x * cam.Res_y;
    rets = new Raycast_ret[resol];
    cl_int cl_error;
    ray_buff = cl::Buffer(kern_pr->context, (CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR), resol * sizeof(cl_ray), cam.rays, &cl_error);
    rets_buff = cl::Buffer(kern_pr->context, (CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR), resol * sizeof(Raycast_ret), rets, &cl_error);
    Kernel_program::cl_throw_error(cl_error);
}

void Scene::render()
{


    std::thread threads[THREADS];
    float resize = cam.Res_x / THREADS;
    for (int i = 0; i < THREADS; i++)
    {

        int x_from = (int)(i * resize);
        int x_to = (int)((i + 1) * resize);
        threads[i] = std::thread(&Scene::new_render_thread,this,x_from,x_to);
    }

    for (int i = 0; i < THREADS; i++)
        threads[i].join();
        



    /*for (int i =0;i<cam.Res_x;i++)
        for (int j = 0; j < cam.Res_y; j++)
        {
            cam.rays[i][j].len = cam.max_view;
            std::thread t;
            set_pixel(i, j, Raytrace(cam.rays[i][j],Render_depth));
        }*/

    
}

SDL_Color Scene::Raytrace(Ray r,int rd_depth,float lenght,Material* now_mat)
{
    if (rd_depth == 0)
        return SDL_Color{ 0,0,0 };
    Raycast_ret p;
    float len = lenght;
    Point res;
    SDL_Color cl;
    Object* now_obj = NULL;
    float blen;
    Vector norm;
    for (Object* obj : objs)
    {
        p = obj->Raycast(r);
        if(p.dist < lenght)
        {
            //blen = (p.pt - r.pt).Length();
            if (blen >= COMPUTING_ERROR && blen < len)
            {
                //res = p.pt;
                len = blen;
                now_obj = obj;
                //norm = p.norm;
            }
        }
    }
   
    if (len == lenght)
        return SDL_Color{ 0,0,0 };

    cl = now_obj->mat.color;

   

     if (now_obj->mat.mirroring != 0)
    {
        Ray rh;
        rh.pt = res;
        rh.dir = r.dir - (norm * (2 * (r.dir ^ norm)));
        SDL_Color next_cl = Raytrace(rh, rd_depth - 1, lenght - len,now_mat);
        float h = cl.r*(1- now_obj->mat.mirroring) + next_cl.r* now_obj->mat.mirroring;
        cl.r = (int)h;
        h = cl.g * (1 - now_obj->mat.mirroring) + next_cl.g * now_obj->mat.mirroring;
        cl.g = (int)h;
        h = cl.b * (1 - now_obj->mat.mirroring) + next_cl.b * now_obj->mat.mirroring;
        cl.b = (int)h;
    }

    if (now_obj->mat.transparency != 0)
    {
        Vector buff_norm = norm;
        Material* obj_mat = &now_obj->mat;
        if (&(now_obj->mat) == now_mat)
            obj_mat = &void_mat;
        if ((buff_norm ^ r.dir) > 0)
            buff_norm *= -1;
        float ref = now_mat->refraction / obj_mat->refraction;
        float c = (buff_norm ^ r.dir) * (-1);
        float sq = sqrt(1-ref*ref*(1-c*c));

        Ray new_ray;
        new_ray.pt = res;
        new_ray.dir = buff_norm * (c * ref - sq);
        new_ray.dir = new_ray.dir + r.dir * ref;
        new_ray.dir.normalize();
        SDL_Color next_cl = Raytrace(new_ray, rd_depth - 1, lenght - len, obj_mat);
        cl.r = cl.r * (1 - obj_mat->transparency) + next_cl.r * obj_mat->transparency;
        cl.g = cl.g * (1 - obj_mat->transparency) + next_cl.g * obj_mat->transparency;
        cl.b = cl.b * (1 - obj_mat->transparency) + next_cl.b * obj_mat->transparency;
    }
    Light all_light;  all_light.cl = { 0,0,0 };
    all_light.brightness = LightCompute(res, norm,lenght, r.dir, now_obj->mat.mirroring);
    cl.r = (now_obj->mat.light_glaring * all_light.cl.r + (cl.r) * (1 - now_obj->mat.light_glaring)) * all_light.brightness;
    cl.g = (now_obj->mat.light_glaring * all_light.cl.g + (cl.g) * (1 - now_obj->mat.light_glaring)) * all_light.brightness;
    cl.b = (now_obj->mat.light_glaring * all_light.cl.b + (cl.b) * (1 - now_obj->mat.light_glaring)) * all_light.brightness;
    
    return cl;
}

void Scene::new_render_thread(int from,int to)
{
    int dd = 0;
    for (int i = from; i < to; i++)
    {
        for (int j = 0; j < cam.Res_y; j++)
        {
            Ray r;
            r.pt = cam.pt;
            r.dir = Point{ i * cam.sX - (cam.dX / 2),(-j * cam.sX) + (cam.dY / 2),1 };
            
            r.dir.normalize();
            if (i == 150 && j == 100)
                dd = 1;
            set_pixel(i, j, Raytrace(r, Render_depth, cam.max_view,&void_mat));
        }
    }
    
}



float Scene::LightCompute(Point pt, Vector norm,float lenght, Vector r, float mir)
{
    float l = 0;
    Vector v;
    for (Light* lig : lights){
        if (lig->type == ambient)
            l += lig->brightness;
        else 
        {
            if (lig->type == point)
            {
                v = lig->pt - pt;
                v.normalize();
            }
            else
                v = lig->dir;
            Ray now; now.dir = v; now.pt = pt;
            bool shadow=false;
            float blen;
            Raycast_ret p;
            for (Object* obj : objs)
            {
                p = obj->Raycast(now);
                if (p.dist<lenght)
                {
                    //blen = (cl_float3_to_Point(p.pt) - now.pt).Length();
                    if (blen >= COMPUTING_ERROR)
                    {
                        shadow = true;
                        break;
                    }
                }
                if (shadow)
                    break;
            }
            if (shadow)
                continue;

            float n_dot_l = norm ^ v;
            if (n_dot_l > 0)
                l += lig->brightness * n_dot_l / (norm.Length() * v.Length());
            if (mir != 0) {
                Vector R = v- norm * n_dot_l * 2;
                R.normalize();
                float r_dot_v = (R ^ r);
                if (r_dot_v > 0)
                    l += lig->brightness * SDL_pow(r_dot_v,10*mir);
            }
        }
    }
    return l;
}




static void render_thread(Scene* sc,SDL_Color** out)
{

    
    for (int i = 0; i < sc->cam.Res_x; i++)
    {
        for (int j = 0; j < sc->cam.Res_y; j++)
        {
            Ray r;
            r.pt = sc->cam.pt;
            r.dir = Point{ i * sc->cam.sX - (sc->cam.dX / 2),(-j * sc->cam.sX) + (sc->cam.dY / 2),1 };

            r.dir.normalize();
            out[i][j] = sc->Raytrace(r, sc->Render_depth,sc->cam.max_view, &sc->void_mat);
        }
    }
}

#ifdef NDEBUG
void Scene::kernel_raytrace(int rd_depth, float lenght, Material* now_mat)
{
    // Заполнение массива лучей
    cl_float3  CamCHR = { cam.sX,cam.dY,cam.dX };
    cl_int cl_error;
    cl::Kernel kern(kern_pr->program, "setRays", &cl_error);
    Kernel_program::cl_throw_error(cl_error);
    Kernel_program::cl_throw_error(kern.setArg(0, ray_buff));
    Kernel_program::cl_throw_error(kern.setArg(1, (cl_float3)cam.pt));
    Kernel_program::cl_throw_error(kern.setArg(2, CamCHR));
    Kernel_program::cl_throw_error(kern.setArg(3, rets_buff));
    Kernel_program::cl_throw_error(kern.setArg(4, cam.max_view));
    cl::NDRange range(cam.Res_y, cam.Res_x);

    cl::CommandQueue queue(kern_pr->context, kern_pr->devices[0]);
    Kernel_program::cl_throw_error(queue.enqueueNDRangeKernel(kern, cl::NullRange, range));
    Kernel_program::cl_throw_error(queue.enqueueReadBuffer(ray_buff, CL_TRUE, 0, sizeof(cl_ray) * (long long)cam.Res_x*cam.Res_y, cam.rays));
    Kernel_program::cl_throw_error(queue.enqueueReadBuffer(rets_buff, CL_TRUE, 0, sizeof(Raycast_ret) * (long long)cam.Res_x*cam.Res_y, rets));




    // Рейкаст со всеми обьектами
    for (auto& obj : objs)
        obj->Raycast(cam.rays, (long long)cam.Res_x * cam.Res_y, *kern_pr,rets,ray_buff,rets_buff);
    ////////////////

    // Свет
    
    for (int i =0;i<SCREEN_HEIGHT;i++)
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            if (rets[(long long)i * SCREEN_WIDTH + j].dist < lenght)
                set_pixel(j, i, SDL_Color{ 255, 0, 0 });
        }

}


#endif // NDEBUG
