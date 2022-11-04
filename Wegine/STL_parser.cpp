#include "STL_parser.h"

Poly_object stl_read(std::string filename)
{
    Poly_object res;
    res.mat.color = SDL_Color{ 0,255,0 };
    res.mat.mirroring = 0;
    res.mat.light_glaring = 0;
    res.mat.refraction = 0;
    res.mat.transparency = 0;
    std::ifstream in(filename, std::ios::binary | std::ios::in);
    if (in.fail())
        throw "ee";
    char header;
    Uint32 nums;
    Uint16 buff;
    for (int i = 0;i<80;i++)
        in.read(&header,1);
    in.read((char*)&nums, sizeof(Uint32));
    Polygon pol;
#ifdef NDEBUG
    cl_polygon cl_pol;
#endif // NDEBUG


    for (Uint32 i = 0; i < nums; i++)
    {
        in.read((char*)&pol.norm.z , sizeof(float));
        in.read((char*)&pol.norm.x, sizeof(float));
        in.read((char*)&pol.norm.y, sizeof(float));
#ifdef NDEBUG
        cl_pol.norm=(cl_float3)pol.norm;
#endif // NDEBUG
        for (int j = 0; j < 3; j++)
        {
            in.read((char*)&pol.points[j].z, sizeof(float));
            in.read((char*)&pol.points[j].x, sizeof(float));
            in.read((char*)&pol.points[j].y, sizeof(float));
#ifdef NDEBUG
            cl_pol.pts[j] = (cl_float3)pol.points[j];
#endif // NDEBUG
        }
        in.read((char*)&buff, sizeof(Uint16));
        res.viewed.push_back(pol);
        res.orig.push_back(pol);
#ifdef NDEBUG
        res.cl_orig.push_back(cl_pol);
        res.cl_viewed.push_back(cl_pol);
#endif // NDEBUG
    }
#ifdef NDEBUG
    cl_pol.norm = (cl_float3)pol.norm;
#endif // NDEBUG
    in.close();
    return res;
}
