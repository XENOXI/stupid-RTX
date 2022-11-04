#define COMPUTING_ERROR 0.0001

typedef struct raycast_return_
{
    float3 pt;
    float3 norm;
    float dist;
}raycast_return;

typedef struct polygon_
{
    float3 pts[3];
    float3 norm;
}polygon;

typedef struct ray_
{
    float3 dir;
    float3 pt;
}ray;

typedef struct Shpere_
{
    float rad;
    float3 pt;
}sphere;

float float_abs(float e)
{
    if (e<0)
        return (-e);
    return e;
}

void print_float3(float3 data)
{
    printf("%f %f %f",data.x,data.y,data.z);
}

float3 vecMult(float3 f,float3 s)
{
    float3 res;
    res.x = f.y*s.z-f.z*s.y;
    res.y = f.z*s.x-f.x*s.z;
    res.z = f.x*s.y-f.y*s.x;
    return res;
}

kernel void setRays(global ray* rays,float3 point,float3 CamCHR,global raycast_return* rets,float lenght)
{
    //CamCHR[0]=sX, CamCHR[1]=dY, CamCHR[2]=dX,
    size_t i = get_global_id(1);
    size_t j = get_global_id(0);
    size_t size = get_global_size(0);  
    rays[i*size+j].pt=point;
    rays[i*size+j].dir = (float3)(((float)i*CamCHR[0]-(CamCHR[2]/2)),((-(float)j*CamCHR[0])+(CamCHR[1]/2)),1);
    rays[i*size+j].dir = normalize(rays[i*size+j].dir);
    rets[i*size+j].dist = lenght;
}

kernel void polygon_raycast(global const ray* rays,global const polygon* polys,global raycast_return* ret)
{
    long long int ray_id = get_global_id(0);
    int pol_id = get_global_id(1);
    float3 e1 = polys[pol_id].pts[1]-polys[pol_id].pts[0];
    float3 e2 = polys[pol_id].pts[2]-polys[pol_id].pts[0];
    float3 normi = vecMult(rays[ray_id].dir,e2);
    float det = dot(e1,normi);
    if (float_abs(det)<COMPUTING_ERROR)
        return;
    float inv_det = 1/det;
    float3 tvec = rays[ray_id].pt-polys[pol_id].pts[0];
    float u =dot(tvec,normi)*inv_det;
    if (u<0 || u>1)
        return;
    float3 qvec = vecMult(tvec,e1);
    float v = dot(rays[ray_id].dir,qvec)*inv_det;
    if (v < 0 || u+v>1)
        return;
    float t = dot(e2,qvec)*inv_det;
    if (t>COMPUTING_ERROR && t<ret[ray_id].dist)
    {
        ret[ray_id].pt = rays[ray_id].pt + (rays[ray_id].dir*t);
        ret[ray_id].norm = polys[pol_id].norm;  
        ret[ray_id].dist = t; 
    }
}

kernel void sphere_raycast(global const ray* rays,global raycast_return* rets,const sphere sph)
{   
    size_t id = get_global_id(0);
    float3 cent_vec = rays[id].pt - sph.pt;
    float b = 2 * dot(cent_vec,rays[id].dir);
    float c = dot(cent_vec,cent_vec)-(sph.rad*sph.rad);
    float disc = (b*b) - (4*c);
    float3 h;
    if (disc<0)
        return;
    float k1 = (sqrt(disc)-b)/2;
    float k2 = -(sqrt(disc)+b)/2;
    if (k1<COMPUTING_ERROR && k2<COMPUTING_ERROR)
        return;
    if(k1<COMPUTING_ERROR)
    {
        if (k2<rets[id].dist)
        {
            h = rays[id].pt + (rays[id].dir*k2);
            rets[id].pt = h;
            rets[id].dist = k2;
            rets[id].norm =normalize(h-sph.pt);
        }
        return;
    }
    if (k2<COMPUTING_ERROR)
    {
        if (k1<rets[id].dist)
        {
            h = rays[id].pt + (rays[id].dir*k1);
            rets[id].pt = h;
            rets[id].dist = k1;
            rets[id].norm =normalize(h-sph.pt);
        }
        return;
    }
    float k = min(k1,k2);
    float3 p = rays[id].pt+(rays[id].dir*k);
    if (k<rets[id].dist)
    {
        rets[id].dist = k;
        rets[id].norm =normalize(p-sph.pt);
        rets[id].pt = p;
    }
    
}


