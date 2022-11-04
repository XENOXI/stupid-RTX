#pragma once
#include <fstream>

#ifdef NDEBUG
#include <CL/cl2.hpp>
#include <iostream>



typedef struct cl_ray
{
	cl_float3 dir;
	cl_float3 pt;
};

typedef struct cl_polygon
{
	cl_float3 pts[3];
	cl_float3 norm;
};


std::ostream& operator<<(std::ostream& out, const cl_ray& r);
class Kernel_program
{
public:
	static void cl_throw_error(cl_int err);
	cl_int cl_error;
	cl::Program program;
	
	cl::Context context;
	cl::vector<cl::Device> devices;
	std::vector<cl::Kernel> kernels;
	cl::CommandQueue queue;
	Kernel_program(std::string);
};

#endif