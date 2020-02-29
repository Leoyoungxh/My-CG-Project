#pragma once
#include "mesh.h"
#include "callback.h"
#include "camera.h"
#include "robot.h"
#include <sstream>
#include <fstream>
#include <iosfwd>
#include "material.h"
using namespace Robot;
using namespace Material;

class Mesh_Painter
{
public:
	Mesh_Painter();
	~Mesh_Painter();

	void load(std::string obj_File, float scalex, float scaley, float scalez,
				float translatex, float translatey, float translatez,
				float rotatex, float rotatey, float rotatez, bool need_normalize);
	void draw_meshes();
	void draw_meshes2_material();
	void draw_speedup();
	void draw_speedup_shadow();
	void draw_meshes2_shadow();
	void draw_meshestest();
	void draw_meshescar();
	void draw_shadowmeshes();
	void draw_carshadow2();
	void draw_robot();
	void draw_robot_shadow();
	void draw_robot_detail(mat4 model_view, int i, bool is_shadow);
	void update_vertex_buffer();
	void update_texture();
	void update_vertex_buffer4();
	void update_texture2();
	void set_light(bool x);
	bool get_light();

	void init_shaders(std::string vs, std::string fs);
	void init_shaders2(std::string vs, std::string fs);
	void add_mesh(My_Mesh*);
	void clear_mehs();

private:

	void load_texture_STBImage(std::string file_name, GLuint& m_texName);

	std::vector<GLuint> textures_all;
	std::vector<GLuint> program_all;
	std::vector<GLuint> vao_all;
	std::vector<GLuint> buffer_all;
	std::vector<GLuint> vPosition_all;
	std::vector<GLuint> vColor_all;
	std::vector<GLuint> vTexCoord_all;
	std::vector<GLuint> vNormal_all;
	std::vector<GLuint>  modelMatrix_all;
	std::vector<GLuint>  modelView_all;
	std::vector<GLuint>  projection_all;
	std::vector<GLuint>  AmbientProduct_all;
	std::vector<GLuint>  DiffuseProduct_all;
	std::vector<GLuint>  SpecularProduct_all;
	std::vector<GLuint>  Shininess_all;
public:
	std::vector<My_Mesh*> m_my_meshes_;
	
	bool light_reflex;
};

