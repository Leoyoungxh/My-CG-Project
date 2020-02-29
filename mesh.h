#pragma once
#include <vector>
#include <string>
#include "Angel.h"

typedef struct Index
{
	unsigned int a, b, c;

	Index(int ia, int ib, int ic) : a(ia), b(ib), c(ic) {}
} vec3i;
typedef struct vec3 vec3f;

struct  point3f
{
	float x;
	float y;
	float z;
	point3f()
	{
		x = 0;
		y = 0;
		z = 0;
	};
	point3f(float xx, float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	};

	float distance(const point3f& p)
	{
		float d = (x - p.x)*(x - p.x);
		d += (y - p.y)*(y - p.y);
		d += (z - p.z)*(z - p.z);

		return sqrt(d);
	};
};

typedef std::vector<float> VertexList;
typedef std::vector<float> NormalList;
typedef std::vector<float> VtList;

typedef std::vector<float> STLVectorf;
typedef std::vector<int> STLVectori;

typedef std::vector<std::pair<int, int> > Edges;
typedef std::vector<unsigned int> FaceList;


class My_Mesh
{
public:
	My_Mesh();
	~My_Mesh();

	void load_obj(std::string obj_File);
	void set_texture_file(std::string s);
	std::string get_texture_file();

	void set_texture_file2(std::string s);
	std::string get_texture_file2(int i);

	const VertexList& get_vertices();
	const NormalList& get_normals();
	const FaceList&   get_faces();
	const VertexList& get_vertices2();
	const NormalList& get_normals2();
	const FaceList&   get_faces2();
	const VtList&   get_vts();
	const std::vector<unsigned int>& get_vertice_index_();
	const std::vector<unsigned int>& get_normal_index_();
	const std::vector<unsigned int>&   get_vt_index_();
	const STLVectorf&   get_colors();
	

	int num_faces();
	int num_vertices();

	const point3f& get_center();

	const bool whether_to_normalize();
	void set_normalization	(bool toNormalize);

	void set_moveup(bool tomoveup);
	const bool whether_to_moveup();

	void get_boundingbox(point3f& min_p, point3f& max_p) const;

	void generate_disk(int num_division);
	void generate_cube();
	void generate_cylinder(int num_division = 100, float height = 2);

	static void normal_to_color(float, float, float, float&, float&, float&);
	void set_translate(float, float, float);
	void get_translate(float& x, float& y, float& z);
	void set_rotate(float, float, float);
	void get_rotate(float& x, float& y, float& z);
	void set_scale(float, float, float);
	void get_scale(float& x, float& y, float& z);
public:
	void clear_data();
public:
	//-----new-------------------
	std::vector<float>			m_vertice_list_2;
	std::vector<float>			m_normal_list_2;
	std::vector<float>			m_color_list_2;
	std::vector<float>			m_vt_list_2;
	std::vector<unsigned int>	m_vertice_index_2;
	std::vector<unsigned int>	m_normal_index_2;
	std::vector<unsigned int>	m_color_index_2;
	std::vector<unsigned int>	m_vt_index_2;

	VertexList m_vertices_;
	NormalList m_normals_;
	FaceList   m_faces_;
	STLVectorf m_color_list_;
	VtList	m_vt_list_;

	std::vector<float>			m_vertice_list_;
	std::vector<float>			m_normal_list_;
	std::vector<unsigned int>	m_vertice_index_;
	std::vector<unsigned int>	m_normal_index_;
	std::vector<unsigned int>	m_color_index_;
	std::vector<unsigned int>	m_vt_index_;

	point3f m_center_;
	point3f m_min_box_;
	point3f m_max_box_;
	std::vector<vec3f> v;
	std::vector<vec3f> vn;
	std::vector<vec3f> vt;

	std::vector<int> fnum;
	std::vector<std::string> mtlname;
	std::vector<int> mtlindex;
	std::vector<std::string> texture_file_name2;
	std::string texture_file_name;
	std::string mtl;

	std::vector<vec4> Ka;	//环境光系数
	std::vector<vec4> Kd;	//漫反射系数
	std::vector<vec4> Ks;	//镜面反射系数
	std::vector<vec4> Em;	//自发光系数
	std::vector<float> Ns;	//高光系数
	
public:
	void add_theta_step();
	void set_theta_step(float x, float y, float z);
	void set_theta(float x, float y, float z);
	void get_theta(float& x, float& y, float& z);
	float get_uptolevel();

private:
	
	

	enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
	int      Axis = Xaxis;
	float up_to_level;
	GLfloat  Theta[3];
	GLfloat  Theta_step[3];
	GLfloat vTranslation[3];
	GLfloat scale[3];
	GLfloat vRotate[3];
	
	bool toNormalize;
	bool toMoveup;

	
};

