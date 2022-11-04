#include "threading.h"
#ifdef NDEBUG


std::ostream& operator<<(std::ostream& out, const cl_ray& r)
{
    out << "Ray(direction=(" << r.dir.x << ',' << r.dir.y << ',' << r.dir.z << "),point = (" << r.pt.x << ',' << r.pt.y << ',' << r.pt.z << "))";
    return out;
}


void Kernel_program::cl_throw_error(cl_int err)
{
    if (err != 0)
    {
        std::cout << err << '\n';
        throw err;
    }
        
}

Kernel_program::Kernel_program(std::string filename)
{
    std::ifstream kern_file(filename);
    if (!kern_file.is_open())
        throw std::exception("File not found");

    std::string kern_prog, buff;

    while (!kern_file.eof()) {
        getline(kern_file, buff);
        kern_prog += buff;
        if (!kern_file.eof()) {
            kern_prog += '\n';
        }
    }
    cl_int cl_error;

    program= cl::Program(kern_prog, true, &cl_error);
    cl_throw_error(cl_error);
    context = program.getInfo<CL_PROGRAM_CONTEXT>(&cl_error);
    cl_throw_error(cl_error);
    devices = context.getInfo<CL_CONTEXT_DEVICES>(&cl_error);
    cl_throw_error(cl_error);
}
#endif