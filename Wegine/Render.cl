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


