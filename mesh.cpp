#include "mesh.h"
#include <sstream>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <math.h>
#include <array>
#include <vector>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <gl/GL.h>
#endif

My_Mesh::My_Mesh()
{
	vTranslation[0] = Theta[0] = 0;
	vTranslation[1] = Theta[1] = 0;
	vTranslation[2] = Theta[2] = 0;
	Theta[0] = 45;
}

My_Mesh::~My_Mesh()
{
}

void My_Mesh::
load_obj(std::string obj_File)
{
	this->clear_data();
	float minx, miny, minz, maxx, maxy, maxz;
	minx = 0;
	miny = 0;
	minz = 0;
	maxx = 0;
	maxy = 0;
	maxz = 0;
	// @TODO: 请在此添加代码实现对含有UV坐标的obj文件的读取
	// 将读取的数据存储为My_Mesh对象的属性值，可参考圆柱体的生成
	
	// 如果传入的文件名为空
	if (obj_File.empty())
		return;
	
	// 创建文件输入流
	std::ifstream in;
	in.open(obj_File.c_str(), std::ios::in);

	// 读取开头的字符串字段
	std::string begin;
	for(int i=0; i<9; i++){
		std::getline(in,begin);
	}
	
	// 读取点坐标
	std::string type;
	while(1){
		in >> type;
		if(type != "v")
			break;
		
		float x,y,z;
		in >> x >> y >> z;
		if(x<minx)
			minx = x;
		if(y<miny)
			miny = y;
		if(z<minz)
			minz = z;
		if(x>maxx)
			maxx = x;
		if(y>maxy)
			maxy = y;
		if(z>maxz)
			maxz = z;
		// 使用容器把点坐标的数据存起来
		v.push_back(vec3f(x, y, z));
		// 向m_vertices_压入数据
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);
	}
	// 读取数据间隔的无用字符串
	std::getline(in,begin);
	std::getline(in,begin);
	
	this->m_center_ = point3f((maxx+minx)/2, (maxy+miny)/2, (maxz+minz)/2);
	this->m_min_box_ = point3f(minx, miny, minz);
	this->m_max_box_ = point3f(maxx, maxy, maxz);

	float d = m_min_box_.distance(m_max_box_);
	up_to_level = -(minz - m_center_.z)/d;

	// 读取法向量
	while(1){
		in >> type;
		if(type != "vn")
			break;
		float x,y,z;
		in >> x >> y >> z;
		// 使用容器把法向的数据存起来
		vn.push_back(vec3f(x,y,z));
	}
	// 读取数据间隔的无用字符串
	std::getline(in,begin);
	std::getline(in,begin);

	// 读取点的贴图坐标
	while(1){
		in >> type;
		if(type != "vt")
			break;
		float x,y,z;
		in >> x >> y >> z;
		// 使用容器把点的贴图坐标数据存起来
		vt.push_back(vec3f(x,y,z));
	}

	
	for(int i=0; i<5; i++)
		std::getline(in,begin);
	
	// 为法向数据空间开辟空间
	m_normals_.resize(m_vertices_.size(),0);
	// 读取面片信息
	while(1){
		in >> type;
		if(type != "f")
			break;
		
		std::string a,b,c;
		int x,y,z;
		

		for(int i=0; i<3; i++){
			// 以字符串形式读取数据
			std::getline(in,a,'/');
			std::getline(in,b,'/');
			in >> c;
			//将字符串转化为整形
			x=std::stoi(a);
			y=std::stoi(b);
			z=std::stoi(c);

			// 面片(下标从1开始)
			m_faces_.push_back(x-1);

			// vn
			m_normals_[(x-1)*3] = vn[z-1].x;
			m_normals_[(x-1)*3+1] = vn[z-1].y;
			m_normals_[(x-1)*3+2] = vn[z-1].z;

			// vt
			m_vt_list_.push_back(vt[y-1].x);
			m_vt_list_.push_back(vt[y-1].y);
		}
	}
	

	for(int i=0; i<m_normals_.size(); i=i+3){
		float r,g,b;
		normal_to_color(m_normals_[i], m_normals_[i+1], m_normals_[i+2], r, g, b);
		// 这里采用法线来生成颜色
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}

};

void My_Mesh::normal_to_color(float nx, float ny, float nz, float& r, float& g, float& b)
{
	r = float(std::min(std::max(0.5 * (nx + 1.0), 0.0), 1.0));
	g = float(std::min(std::max(0.5 * (ny + 1.0), 0.0), 1.0));
	b = float(std::min(std::max(0.5 * (nz + 1.0), 0.0), 1.0));
};



void My_Mesh::clear_data()
{
	m_vertices_.clear();
	m_normals_.clear();
	m_faces_.clear();
	m_color_list_.clear();
	m_vt_list_.clear();
	Ka.clear();
	Ks.clear();
	Kd.clear();
	m_vertice_list_2.clear();
	m_normal_list_2.clear();
	m_color_list_2.clear();
	m_vt_list_2.clear();

	m_vertice_index_2.clear();
	m_normal_index_2.clear();
	m_color_index_2.clear();
	m_vt_index_2.clear();



};

void My_Mesh::get_boundingbox(point3f& min_p, point3f& max_p) const
{
	min_p = this->m_min_box_;
	max_p = this->m_max_box_;
};

const STLVectorf&  My_Mesh::get_colors()
{
	return this->m_color_list_;
};

const VertexList& My_Mesh::get_vertices()
{
	return this->m_vertices_;
};

const NormalList& My_Mesh::get_normals()
{
	return this->m_normals_;
};

const FaceList&   My_Mesh::get_faces()
{
	return this->m_faces_;
};
const VertexList& My_Mesh::get_vertices2()
{
	return this->m_vertice_list_;
};

const NormalList& My_Mesh::get_normals2()
{
	return this->m_normal_list_;
};

const FaceList&   My_Mesh::get_faces2()
{
	return this->m_faces_;
};

const VtList&  My_Mesh::get_vts()
{
	return this->m_vt_list_;
};

const std::vector<unsigned int>& My_Mesh::get_vertice_index_()
{
	return this->m_vertice_index_;
};

const std::vector<unsigned int>& My_Mesh::get_normal_index_()
{
	return this->m_normal_index_;
};

const std::vector<unsigned int>&  My_Mesh::get_vt_index_()
{
	return this->m_vt_index_;
};

int My_Mesh::num_faces()
{
	return this->m_faces_.size()/3;
};

int My_Mesh::num_vertices()
{
	return this->m_vertices_.size()/3;
};

const point3f& My_Mesh::get_center()
{
	return this->m_center_;
};

const bool My_Mesh::whether_to_normalize()
{
	return this->toNormalize;
};

void My_Mesh::set_normalization	(bool  toNormalize)
{
	this->toNormalize = toNormalize;
};

const bool My_Mesh::whether_to_moveup()
{
	return this->toMoveup;
};

void My_Mesh::set_moveup (bool  tomoveup)
{
	this->toMoveup = tomoveup;
};

/*
// 生成正方体
void My_Mesh::generate_cube()
{
	// @TODO: 请在此添加代码生成圆盘
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, 0);
	this->m_max_box_ = point3f(1, 1, 0);

	// 设置表面顶点坐标
	float z = 0;
	m_vertices_.push_back(1);
	m_vertices_.push_back(1);
	m_vertices_.push_back(z);

	
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		
		
		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		// 法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		// 这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_color_list_.push_back(0);
	m_color_list_.push_back(0);
	m_color_list_.push_back(0);

	// 生成三角面片，每个矩形由两个三角形面片构成
	for (int i = 0; i < num_samples; i++)
	{
		// 面片1
		m_faces_.push_back(i);
		m_faces_.push_back((i+1)%num_samples);
		m_faces_.push_back(num_samples);
		
		// 映射到UV坐标的0-1
		// 纹理坐标
		m_vt_list_.push_back(m_vertices_[i*3]*0.5+0.5);
		m_vt_list_.push_back(m_vertices_[i*3+1]*0.5+0.5);
		// 纹理坐标
		m_vt_list_.push_back(m_vertices_[(i+1)*3]*0.5+0.5);
		m_vt_list_.push_back(m_vertices_[(i+1)*3+1]*0.5+0.5);
		// 纹理坐标
		m_vt_list_.push_back(0.5);
		m_vt_list_.push_back(0.5);
	}
	
};
*/

// 生成圆盘
void My_Mesh::generate_disk(int num_division)
{
	// @TODO: 请在此添加代码生成圆盘
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, 0);
	this->m_max_box_ = point3f(1, 1, 0);

	int num_samples = num_division;
	float step = 2 * M_PI / num_samples; // 每个切片的弧度

	// 按cos和sin生成x，y坐标，z为0，即得到表面顶点坐标
	float z = 0;
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);
		
		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		// 法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		// 这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_vertices_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_normals_.push_back(0);
	m_color_list_.push_back(0);
	m_color_list_.push_back(0);
	m_color_list_.push_back(0);

	// 生成三角面片，每个矩形由两个三角形面片构成
	for (int i = 0; i < num_samples; i++)
	{
		// 面片1
		m_faces_.push_back(i);
		m_faces_.push_back((i+1)%num_samples);
		m_faces_.push_back(num_samples);
		
		// 映射到UV坐标的0-1
		// 纹理坐标
		m_vt_list_.push_back(m_vertices_[i*3]*0.5+0.5);
		m_vt_list_.push_back(m_vertices_[i*3+1]*0.5+0.5);
		// 纹理坐标
		m_vt_list_.push_back(m_vertices_[(i+1)*3]*0.5+0.5);
		m_vt_list_.push_back(m_vertices_[(i+1)*3+1]*0.5+0.5);
		// 纹理坐标
		m_vt_list_.push_back(0.5);
		m_vt_list_.push_back(0.5);
	}
	
};

void My_Mesh::generate_cylinder(int num_division, float height)
{
	this->clear_data();
	this->m_center_ = point3f(0, 0, 0);
	this->m_min_box_ = point3f(-1, -1, -height);
	this->m_max_box_ = point3f(1, 1, height);

	int num_samples = num_division;
	float z = -height;
	float pi = 3.14159265;
	float step = 1.0 * 360 / num_samples;
	float rr = pi / 180;
	//圆柱体Z轴向上，按cos和sin生成x，y坐标
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		//这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
	}

	z = height;
	//圆柱体Z轴向上，按cos和sin生成x，y坐标，
	for (int i = 0; i < num_samples; i++)
	{
		float r_r_r = i * step * rr;
		float x = cos(r_r_r);
		float y = sin(r_r_r);
		m_vertices_.push_back(x);
		m_vertices_.push_back(y);
		m_vertices_.push_back(z);

		m_normals_.push_back(x);
		m_normals_.push_back(y);
		m_normals_.push_back(0);
		//法线由里向外
		float r;
		float g;
		float b;
		My_Mesh::normal_to_color(x, y, z, r, g, b);
		m_color_list_.push_back(r);
		m_color_list_.push_back(g);
		m_color_list_.push_back(b);
		//这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
	}

	for (int i = 0; i < num_samples; i++)
	{
		m_faces_.push_back(i);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples) % (num_samples)+num_samples);

		m_faces_.push_back((i + num_samples) % (num_samples)+num_samples);
		m_faces_.push_back((i + 1) % num_samples);
		m_faces_.push_back((i + num_samples + 1) % (num_samples)+num_samples);
		//生成三角面片

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标

		m_vt_list_.push_back(1.0 * i / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(0.0);
		//纹理坐标
		m_vt_list_.push_back(1.0 * ((i + 1)) / num_samples);
		m_vt_list_.push_back(1.0);
		//纹理坐标
	}

};

void My_Mesh::set_texture_file2(std::string s)
{
	this->texture_file_name2.push_back(s);
};

std::string My_Mesh::get_texture_file2(int i)
{
	return this->texture_file_name2[i];
};

void My_Mesh::set_texture_file(std::string s)
{
	this->texture_file_name = s;
};

std::string My_Mesh::get_texture_file()
{
	return this->texture_file_name;
};

void My_Mesh::set_rotate(float x, float y, float z)
{
	vRotate[0] = x;
	vRotate[1] = y;
	vRotate[2] = z;

};
void My_Mesh::get_rotate(float& x, float& y, float& z)
{
	x = vRotate[0];
	y = vRotate[1];
	z = vRotate[2];
};

void My_Mesh::set_translate(float x, float y, float z)
{
	vTranslation[0] = x;
	vTranslation[1] = y;
	vTranslation[2] = z;

};
void My_Mesh::get_translate(float& x, float& y, float& z)
{
	x = vTranslation[0];
	y = vTranslation[1];
	z = vTranslation[2];
};

void My_Mesh::set_scale(float x, float y, float z)
{
	scale[0] = x;
	scale[1] = y;
	scale[2] = z;

};
void My_Mesh::get_scale(float& x, float& y, float& z)
{
	x = scale[0];
	y = scale[1];
	z = scale[2];
};

void My_Mesh::set_theta(float x, float y, float z)
{
	Theta[0] = x;
	Theta[1] = y;
	Theta[2] = z;
};
void My_Mesh::get_theta(float& x, float& y, float& z)
{
	x = Theta[0];
	y = Theta[1];
	z = Theta[2];
};

void My_Mesh::set_theta_step(float x, float y, float z)
{
	Theta_step[0] = x;
	Theta_step[1] = y;
	Theta_step[2] = z;
};

void My_Mesh::add_theta_step()
{
	Theta[0] = Theta[0] + Theta_step[0];
	Theta[1] = Theta[1] + Theta_step[1];
	Theta[2] = Theta[2] + Theta_step[2];
};

float My_Mesh::get_uptolevel()
{
	return up_to_level;
}