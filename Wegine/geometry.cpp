#include "geometry.h"
#include <SDL.h>


Vector Vector::operator/(const float num) const
{
	return (Vector)Point{x/num,y/num,z/num};
}

Vector::Vector()
{
	x = 0; y = 0; z = 0;
}

Vector::Vector(Point p)
{
	x = p.x;
	y = p.y;
	z = p.z;
}

float Vector::Length()
{
	return sqrt(x * x + y * y+z*z);
}

void Vector::normalize()
{
	float len = Length();
	x /= len;
	y /= len;
	z /= len;
}

Vector Vector::Rotate(float alpha, float beta, float gamma) const
{
	Vector res;
	double a_cos = cos(alpha);
	double a_sin = sin(alpha);
	double b_cos = cos(beta);
	double b_sin = sin(beta);
	double g_cos = cos(gamma);
	double g_sin = sin(gamma);

	
	double matrix[3][3] = {
		{b_cos * g_cos, a_sin * b_sin * g_cos - a_cos * g_sin, a_cos * b_sin * g_cos + a_sin * g_sin},
		{b_cos * g_sin, a_sin * b_sin * g_sin + a_cos * g_cos, a_cos * b_sin * g_sin - a_sin * g_cos},
		{-b_sin, a_sin * b_cos, a_cos * b_cos}
	};


	res.x = x * matrix[0][0] + y * matrix[0][1] + z * matrix[0][2];
	res.y = x * matrix[1][0] + y * matrix[1][1] + z * matrix[1][2];
	res.z = x * matrix[2][0] + y * matrix[2][1] + z * matrix[2][2];

	return res;
}

Vector Vector::operator+(const Vector v) const
{
	Vector res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

Vector Vector::operator-(const Vector v) const
{
	Vector res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}


Vector& Vector::operator+=(const Vector v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector& Vector::operator-=(const Vector v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector Vector::operator*(const float num) const
{
	Vector res;
	res.x = x * num;
	res.y = y * num;
	res.z = z * num;
	return res;
}

Vector Vector::operator*(const Vector v) const
{
	Vector res;
	res.x = y * v.z - z*v.y;
	res.y = z * v.x - x*v.z;
	res.z = x * v.y - y*v.x;
	return	res;
}

Vector& Vector::operator*=(const float num)
{
	x *= num;
	y *= num;
	z *= num;
	return *this;
}

float Vector::operator^(const Vector v) const
{
	return (x*v.x + y*v.y + z*v.z);
}

//Vector

Point::Point()
{
	x = 0; y = 0; z = 0;
}

Point::Point(float x_, float y_, float z_)
{
	x = x_;
	y = y_;
	z = z_;
}

Point::operator cl_float3() const
{
	return { x,y,z };
}

Point Point::operator+(const Vector v) const
{
	Point res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

Vector Point::operator-(const Point p) const
{
	Vector res;
	res.x = x - p.x;
	res.y = y - p.y;
	res.z = z - p.z;
	return res;
}




void Sphere::Resize(float x, float, float)
{
	rad *= x;
}


Raycast_ret Sphere::Raycast(Ray r)
{
	Raycast_ret res;
	Point h;
	float a = r.dir ^ r.dir;
	Vector centVec = r.pt - pt;
	float b = 2 * (centVec ^ r.dir);
	float c = (centVec ^ centVec) - (rad * rad);
	float disc = b * b - 4 * a * c;
	if (disc < 0)
		return res;
	float k1 = (sqrt(disc) - b) / (2 * a);
	float k2 = -(sqrt(disc) + b) / (2 * a);
	if (k1 < COMPUTING_ERROR && k2 < COMPUTING_ERROR)
		return res;


	if (k1 < COMPUTING_ERROR)
	{
		h = r.pt + (r.dir * k2);
		res.pt = h;
		res.norm = h - pt;
		res.dist = k1;
		//res.norm.normalize();
		return res;// TODO
	}
	if (k2 < COMPUTING_ERROR)
	{
		h = r.pt + (r.dir * k1);
		res.pt = h;
		res.norm = h - pt;
		//res.norm.normalize();
		return res;// TODO
	}

	Point p1 = r.pt + (r.dir * k1);
	Point p2 = r.pt + (r.dir * k2);
	if (k1 < k2)
	{
		res.norm = p1 - pt;
		//res.norm.normalize();
		res.pt = h;// TODO
	}
	else
	{
		res.norm = p2 - pt;
		//res.norm.normalize();
		res.pt=h;  // TODO
	}

	return res;
}




Raycast_ret Poly_object::Raycast(Ray r)
{


	Raycast_ret res;
	Point Pres;
	bool is_inter = false;
	Vector e1res;
	Vector e2res;
	float dist = INFINITY;
	Polygon now;
	Vector normal_res;
	for (int i =0;i<viewed.size();i++)
	//for (Polygon pol : viewed)
	{
		Polygon pol = viewed[i];
		if (i == 180)
			std::cout << "";
		Vector e1 = (pol.points[1] - pol.points[0]);
		Vector e2 = (pol.points[2] - pol.points[0]);
		Vector normi = r.dir * e2;
		float det = e1 ^ normi;
		if (abs(det) < COMPUTING_ERROR)
			continue;
		float inv_det = 1 / det;
		Vector tvec = r.pt - pol.points[0];
		float u = (tvec ^ normi) * inv_det;
		if (u < 0 || u > 1)
			continue;
		Vector qvec = tvec * e1;
		float v = (r.dir ^ qvec) * inv_det;
		if (v < 0 || u + v > 1)
			continue;

		float t = (e2 ^ qvec) * inv_det;
		if (t > COMPUTING_ERROR)
		{
			if (t < dist)
			{
				Pres = r.pt + (r.dir * t);
				e1res = e1;
				e2res = e2;
				is_inter = true;
				dist = t;
				res.dist = dist;
			}
		}
	}
	
	if (!is_inter)
		return res;
	res.norm = e1res * e2res;
	//res.norm.normalize();
	//if ((res.norm ^ r.dir) > 0)
		//res.norm *= -1;
	res.pt = Pres;
	return res;
}

void Poly_object::Rotate(float x, float y, float z)
{
	rotation_x += x;
	while (rotation_x > M_PI)
		rotation_x -= 2 * M_PI;
	while (rotation_x < -M_PI)
		rotation_x += 2 * M_PI;

	rotation_y += y;
	while (rotation_y > M_PI)
		rotation_y -= 2 * M_PI;
	while (rotation_y < -M_PI)
		rotation_y += 2 * M_PI;

	rotation_z += z;
	while (rotation_z > M_PI)
		rotation_z -= 2 * M_PI;
	while (rotation_z < -M_PI)
		rotation_z += 2 * M_PI;

	for (int j = 0; j < orig.size(); j++)
	{
		for (int i = 0; i < 3; i++)
		{
			Vector buff = (Vector)orig[j].points[i];

			viewed[j].points[i] = buff.Rotate(rotation_x, rotation_y, rotation_z);
		}
	}
}




#ifdef NDEBUG


void Sphere::Raycast(cl_ray* rays, long long rays_size, Kernel_program& kern_pr, Raycast_ret* rets, cl::Buffer& buff, cl::Buffer& rets_buff)
{
	
}


void Poly_object::Raycast(cl_ray* rays,long long rays_size, Kernel_program& kern_pr,Raycast_ret* rets, cl::Buffer& rays_buff,cl::Buffer& rets_buff)
{
	cl_int cl_error;

	
	cl::Buffer poly_buf(kern_pr.context, (CL_MEM_READ_WRITE | CL_MEM_HOST_READ_ONLY | CL_MEM_COPY_HOST_PTR), cl_viewed.size()*sizeof(cl_polygon), cl_viewed.data(), &cl_error);
	Kernel_program::cl_throw_error(cl_error);

	cl::Kernel kern(kern_pr.program, "polygon_raycast", &cl_error);
	Kernel_program::cl_throw_error(cl_error);

	Kernel_program::cl_throw_error(kern.setArg(0, rays_buff));
	Kernel_program::cl_throw_error(kern.setArg(1, poly_buf));
	Kernel_program::cl_throw_error(kern.setArg(2, rets_buff));

	Raycast_ret res;



	cl::CommandQueue queue(kern_pr.context, kern_pr.devices[0]);

	Kernel_program::cl_throw_error(queue.enqueueNDRangeKernel(kern, cl::NullRange, cl::NDRange(rays_size,viewed.size())));

	Kernel_program::cl_throw_error(queue.enqueueReadBuffer(rets_buff, CL_TRUE, 0, sizeof(Raycast_ret) * rays_size, rets));


}

#endif //  NDEBUG

std::ostream& operator<<(std::ostream& out, const Ray& r)
{
	out << "Ray(direction=(" << r.dir.x << ',' << r.dir.y << ',' << r.dir.z << "),point = (" << r.pt.x << ',' << r.pt.y << ',' << r.pt.z<<"))";
	return out;
}
