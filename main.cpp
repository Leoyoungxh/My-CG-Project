// rotating cube with two texture objects
// change textures with 1 and 2 keys

#include "callback.h"
#include "camera.h"
#include "Angel.h"
#include "mesh.h"
#include "robot.h"
#include "material.h"
#include "Mesh_Painter.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iosfwd>
using namespace Robot;

// #pragma comment(lib, "glew32.lib")
// #pragma comment(lib, "FreeImage.lib")

/*----------------------------------实现功能---------------------------- ----
		
		#1 搭建出了一个室外场景，包括天空和草地
		#2 创建了很多个模型，包括层级建模的人、建筑、树木
		#3 对所有的模型都添加了纹理或者材质效果
		#4 对所有的模型都添加了阴影和光照效果
		#5 用户可以实现视角切换：1-全局 2-人 3-车（按键1、2、3）
		#5 驾车模式，用户按f键上车，f键下车。
		#6 用户可以实现相应视角下的物体移动
			人模式： w：前进  a：左转  d：右转
			车模式： w：前进  a：左转  d：右转
		#7 用户可以实现全局视角下的相机移动
			全局模式：
			上：相机上旋  下：相机下旋  
			左: 相机左旋  右：相机右旋
		#8 用户可以实现鼠标拖拽改变光源位置




---------------------------------------------------------------------------*/
// 窗口大小
struct Window
{
	int width = 1200;
	int height = 1000;
};
Window window;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// 存储要贴纹理的物体
std::vector<My_Mesh*>	my_meshs;
// 存储纹理图片
Mesh_Painter*			ground;
Mesh_Painter*			shadow;
Mesh_Painter*			robot;
Mesh_Painter*			robot_shadow;
Mesh_Painter*			article;
Mesh_Painter*			newhouse;
Mesh_Painter*			tree;
Mesh_Painter*			treeshadow;
Mesh_Painter*			car;
Mesh_Painter*			carshadow;
Mesh_Painter*			oldhouse;
Mesh_Painter*			speedup;
Mesh_Painter*			speedup_shadow;


int startsWith(std::string s, std::string sub){
        return s.find(sub)==0?1:0;
}

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)

{

    std::string::size_type pos1, pos2;

    pos2 = s.find(c);

    pos1 = 0;

    while(std::string::npos != pos2)

    {

        v.push_back(s.substr(pos1, pos2-pos1));

         

        pos1 = pos2 + c.size();

        pos2 = s.find(c, pos1);

    }

    if(pos1 != s.length())

        v.push_back(s.substr(pos1));

}

void load_obj(std::string obj_File, Mesh_Painter* mp, Mesh_Painter* mp_shadow,
				float scalex, float scaley, float scalez,
				float translatex, float translatey, float translatez,
				float rotatex, float rotatey, float rotatez, bool need_normalize, bool need_shadow)
{
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
	std::string mtl_File;
	std::ifstream in_obj, in_mtl;
	in_obj.open(obj_File.c_str(), std::ios::in);
	
	bool need_push = false; //判断是否需要推入
	int index_v=1;
	int index_vn=1; //表示新的元素的下标
	int index_vt=1;
	int index_face=1;
	int num_v=0;
	int num_vn=0;
	int num_vt=0;
	int num_face=0;
	int fnum=0;

	std::string line;
	My_Mesh* mesh1;
	mesh1 = new My_Mesh;
	mesh1->clear_data();

	while(std::getline(in_obj,line))
	{
		if(startsWith(line, "#"))
			continue;
		if(startsWith(line, "mtllib"))
		{
			std::string str1, str2;
			std::istringstream is(line);
			is>>str1>>str2;
			mtl_File = str2;
		}
		if(startsWith(line, "o"))
		{
			if(need_push)
			{
				mesh1->set_normalization(need_normalize);
				mesh1->set_scale(scalex, scaley, scalez);
				mesh1->set_translate(translatex, translatey, translatez);	// 平移
				mesh1->set_rotate(rotatex, rotatey, rotatez);
				mesh1->fnum.push_back(fnum);
				mesh1->mtlindex.push_back(num_face);
				need_push = false;
				mp->add_mesh(mesh1);
				if(need_shadow)
					mp_shadow->add_mesh(mesh1);
				
				mesh1 = new My_Mesh;
				mesh1->clear_data();
				index_v += num_v;
				index_vn += num_vn;
				index_vt += num_vt;
				index_face += num_face;
				num_v = 0;
				num_vn = 0;
				num_vt = 0;
				num_face = 0;
			}
		}
		if(startsWith(line, "vn"))
		{	
			std::string str1, str2, str3, str4;
			std::istringstream is(line);
			is>>str1>>str2>>str3>>str4;
			float x, y, z;
			x = atof(str2.c_str());
			y = atof(str3.c_str());
			z = atof(str4.c_str());

			// 使用容器把点坐标的数据存起来
			mesh1->m_normal_list_2.push_back(x);	
			mesh1->m_normal_list_2.push_back(y);	
			mesh1->m_normal_list_2.push_back(z);

			// 
			mesh1->m_color_list_2.push_back(0);
			mesh1->m_color_list_2.push_back(0);
			mesh1->m_color_list_2.push_back(0);

			num_vn++;		
		}
		else if(startsWith(line, "vt"))
		{
			std::string str1, str2, str3, str4;
			std::istringstream is(line);
			is>>str1>>str2>>str3>>str4;
			float x, y, z;
			x = atof(str2.c_str());
			y = atof(str3.c_str());

			// 使用容器把点坐标的数据存起来
			mesh1->m_vt_list_2.push_back(x);
			mesh1->m_vt_list_2.push_back(y);
			num_vt++;
		}
		else if(startsWith(line, "v"))
		{

			need_push = true;
			std::string str1, str2, str3, str4;
			std::istringstream is(line);
			is>>str1>>str2>>str3>>str4;
			float x, y, z;
			x = atof(str2.c_str());
			y = atof(str3.c_str());
			z = atof(str4.c_str());
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
			mesh1->m_vertice_list_2.push_back(x);
			mesh1->m_vertice_list_2.push_back(y);
			mesh1->m_vertice_list_2.push_back(z);
			num_v++;	
		}
		if(startsWith(line, "f"))
		{
			
			std::string str[5];
			std::istringstream is(line);
			is>>str[0]>>str[1]>>str[2]>>str[3]>>str[4];

			if(str[4]=="")
				fnum = 3;
			else
				fnum = 4;
			for(int i=1; i<fnum+1; i++)
			{
				std::vector<std::string> v;

				SplitString(str[i], v, "/"); //可按多个字符来分隔;

				
				//将字符串转化为整形
				int x, y, z;
				x=std::stoi(v[0]);
				y=std::stoi(v[1]);
				z=std::stoi(v[2]);

				// 面片(下标从1开始)
				mesh1->m_vertice_index_2.push_back(x-index_v);

				// vn and color
				mesh1->m_normal_index_2.push_back(z-index_vn);
				mesh1->m_color_index_2.push_back(z-index_vn);

				// vt
				mesh1->m_vt_index_2.push_back(y-index_vt);
				
			}
			num_face++;
		}
		if(startsWith(line, "usemtl"))
		{
			std::string str1, str2;
			std::istringstream is(line);
			is>>str1>>str2;
			mesh1->mtlname.push_back(str2);
			mesh1->mtlindex.push_back(num_face);
			mesh1->fnum.push_back(fnum);
		}
		if(startsWith(line, "s"))
			continue;
	}

	if(need_push)
	{
		mesh1->set_normalization(false);
		mesh1->set_scale(scalex, scaley, scalez);
		mesh1->set_translate(translatex, translatey, translatez);	// 平移
		mesh1->set_rotate(rotatex, rotatey, rotatez);
		mesh1->mtlindex.push_back(num_face);
		mesh1->fnum.push_back(fnum);
		mp->add_mesh(mesh1);
		if(need_shadow)
			mp_shadow->add_mesh(mesh1);
		num_vn = 0;
		num_vt = 0;
		num_face = 0;
	}

	for(int i=0; i<mp->m_my_meshes_.size(); i++)
	{
		mp->m_my_meshes_[i]->m_min_box_ = point3f(minx, miny, minz);
		mp->m_my_meshes_[i]->m_max_box_ = point3f(minx, miny, minz);
		mp->m_my_meshes_[i]->m_center_ = point3f((maxx+minx)/2, (maxy+miny)/2, (maxz+minz)/2);
	}

	
			
	std::string mtl_file_dir = "texture/" + mtl_File;
	in_mtl.open(mtl_file_dir.c_str(), std::ios::in);

	while(getline(in_mtl, line))
	{
		if(startsWith(line, "#"))
			continue;
		if(startsWith(line, "newmtl"))
		{
			std::string str1, str2;
			std::istringstream is(line);
			is>>str1>>str2;
			std::string textname = str2;
			std::string textdir = "";
			vec4 Ka,Kd,Ks;
			float Ns;
			while(1)
			{
				getline(in_mtl, line);
				if(line=="")
				{
					for(int i=0; i<mp->m_my_meshes_.size(); i++)
					{
						for(int j=0; j<mp->m_my_meshes_[i]->mtlname.size(); j++)
						{
							if(mp->m_my_meshes_[i]->mtlname[j]==textname)
							{
								mp->m_my_meshes_[i]->set_texture_file2(textdir);
								mp->m_my_meshes_[i]->Ka.push_back(Ka);
								mp->m_my_meshes_[i]->Kd.push_back(Kd);
								mp->m_my_meshes_[i]->Ks.push_back(Ks);
								mp->m_my_meshes_[i]->Ns.push_back(Ns);
							}
							
						}
					}
					break;
				}
				if(startsWith(line, "map_Kd"))
				{
					std::string str3, str4;
					std::istringstream is(line);
					is>>str3>>str4;
					
					textdir = "texture/" + str4 ;
				}
				if(startsWith(line, "Ns"))
				{
					std::string str3, str4;
					std::istringstream is(line);
					is>>str3>>str4;
					float x;
					x = atof(str4.c_str());

					Ns = x;
				}
				if(startsWith(line, "Ka"))
				{
					std::string str3, str4, str5, str6;
					std::istringstream is(line);
					is>>str3>>str4>>str5>>str6;
					float x, y, z;
					x = atof(str4.c_str());
					y = atof(str5.c_str());
					z = atof(str6.c_str());

					Ka = vec4(x,y,z,0);
				}
				if(startsWith(line, "Kd"))
				{
					std::string str3, str4, str5, str6;
					std::istringstream is(line);
					is>>str3>>str4>>str5>>str6;
					float x, y, z;
					x = atof(str4.c_str());
					y = atof(str5.c_str());
					z = atof(str6.c_str());

					Kd = vec4(x,y,z,0);
				}
				if(startsWith(line, "Ks"))
				{
					std::string str3, str4, str5, str6;
					std::istringstream is(line);
					is>>str3>>str4>>str5>>str6;
					float x, y, z;
					x = atof(str4.c_str());
					y = atof(str5.c_str());
					z = atof(str6.c_str());

					Ks = vec4(x,y,z,0);
				}
				if(startsWith(line, "Ni"))
				{
					continue;
				}
				if(startsWith(line, "d"))
				{
					continue;
				}
				if(startsWith(line, "illum"))
				{
					continue;
				}
			}
		}
	}
};

void init()
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
    glClearColor( 1.0, 1.0, 1.0, 1.0 );
}

void display(void)
{
#ifdef __APPLE__ // 解决 macOS 10.15 显示画面缩小问题
	glViewport(0, 0, window.width * 2, window.height * 2);
#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	newhouse->draw_meshes2_material();
	tree->draw_meshes2_material();
	treeshadow->draw_meshes2_shadow();
	oldhouse->draw_meshes2_material();
	shadow->draw_shadowmeshes();
	article->draw_meshes();
	ground->draw_meshes();
	car->draw_meshescar();
	carshadow->draw_carshadow2();
	if(viewtype != 3)
	{
		robot->draw_robot();
		robot_shadow->draw_robot_shadow();
	}
	else
	{
		positionx_robot = tx+0.7*sin(carturn_right*M_PI/180);
		positionz_robot = tz+0.7*cos(carturn_right*M_PI/180);
		robot_movex = 0;
		robot_movez = 0;
		robotturn_right = carturn_right+90;
	}
	speedup->draw_speedup();
	speedup_shadow->draw_speedup_shadow();
	glutSwapBuffers();
};

void draw_robot()
{

	// 机器人
	// 躯干
	My_Mesh* torso = new My_Mesh;
	torso->load_obj("texture/robot_cube.obj");
	torso->set_texture_file("texture/torso.png");	// 指定纹理图像文件
	torso->set_normalization(false);
	torso->set_scale(1, 1, 1);
	torso->set_translate(0.0, UPPER_LEG_HEIGHT + LOWER_LEG_HEIGHT, 0.0);	// 平移
	robot->add_mesh(torso);
	my_meshs.push_back(torso);
	robot_shadow->add_mesh(torso);

	// 头部
	My_Mesh* head = new My_Mesh;
	head->load_obj("texture/robot_cube.obj");
	head->set_texture_file("texture/head.png");	// 指定纹理图像文件
	head->set_normalization(false);
	head->set_scale(1, 1, 1);
	head->set_translate(0.0, TORSO_HEIGHT, 0.0);	// 平移
	robot->add_mesh(head);
	my_meshs.push_back(head);
	robot_shadow->add_mesh(head);

	// 左大臂
	My_Mesh* left_upper_arm = new My_Mesh;
	left_upper_arm->load_obj("texture/robot_cube.obj");
	left_upper_arm->set_texture_file("texture/upper_arm.png");	// 指定纹理图像文件
	left_upper_arm->set_normalization(false);
	left_upper_arm->set_scale(1, 1, 1);
	left_upper_arm->set_translate(-(0.5*TORSO_WIDTH + 0.5*UPPER_ARM_WIDTH)  , TORSO_HEIGHT, 0.0);	// 平移
	robot->add_mesh(left_upper_arm);
	my_meshs.push_back(left_upper_arm);
	robot_shadow->add_mesh(left_upper_arm);

	// 左小臂
	My_Mesh* left_lower_arm = new My_Mesh;
	left_lower_arm->load_obj("texture/robot_cube.obj");
	left_lower_arm->set_texture_file("texture/lower_arm.png");	// 指定纹理图像文件
	left_lower_arm->set_normalization(false);
	left_lower_arm->set_scale(1, 1, 1);
	left_lower_arm->set_translate(0.0 ,-UPPER_ARM_HEIGHT, 0.0);	// 平移
	robot->add_mesh(left_lower_arm);
	my_meshs.push_back(left_lower_arm);
	robot_shadow->add_mesh(left_lower_arm);

	// 右大臂
	My_Mesh* right_upper_arm = new My_Mesh;
	right_upper_arm->load_obj("texture/robot_cube.obj");
	right_upper_arm->set_texture_file("texture/upper_arm.png");	// 指定纹理图像文件
	right_upper_arm->set_normalization(false);
	right_upper_arm->set_scale(1, 1, 1);
	right_upper_arm->set_translate((0.5*TORSO_WIDTH + 0.5*UPPER_ARM_WIDTH)  , TORSO_HEIGHT, 0.0);	// 平移
	robot->add_mesh(right_upper_arm);
	my_meshs.push_back(right_upper_arm);
	robot_shadow->add_mesh(right_upper_arm);

	// 右小臂
	My_Mesh* right_lower_arm = new My_Mesh;
	right_lower_arm->load_obj("texture/robot_cube.obj");
	right_lower_arm->set_texture_file("texture/lower_arm.png");	// 指定纹理图像文件
	right_lower_arm->set_normalization(false);
	right_lower_arm->set_scale(1, 1, 1);
	right_lower_arm->set_translate(0.0 ,-UPPER_ARM_HEIGHT, 0.0);	// 平移
	robot->add_mesh(right_lower_arm);
	my_meshs.push_back(right_lower_arm);
	robot_shadow->add_mesh(right_lower_arm);

	// 左大腿
	My_Mesh* left_upper_leg = new My_Mesh;
	left_upper_leg->load_obj("texture/robot_cube.obj");
	left_upper_leg->set_texture_file("texture/upper_leg.png");	// 指定纹理图像文件
	left_upper_leg->set_normalization(false);
	left_upper_leg->set_scale(1, 1, 1);
	left_upper_leg->set_translate(-(0.5*TORSO_WIDTH-0.5*UPPER_LEG_WIDTH) , 0.0, 0.0);	// 平移
	robot->add_mesh(left_upper_leg);
	my_meshs.push_back(left_upper_leg);
	robot_shadow->add_mesh(left_upper_leg);

	// 左小腿
	My_Mesh* left_lower_leg = new My_Mesh;
	left_lower_leg->load_obj("texture/robot_cube.obj");
	left_lower_leg->set_texture_file("texture/lower_leg.png");	// 指定纹理图像文件
	left_lower_leg->set_normalization(false);
	left_lower_leg->set_scale(1, 1, 1);
	left_lower_leg->set_translate(0.0 , -UPPER_LEG_HEIGHT, 0.0);	// 平移
	robot->add_mesh(left_lower_leg);
	my_meshs.push_back(left_lower_leg);
	robot_shadow->add_mesh(left_lower_leg);

	// 右大腿
	My_Mesh* right_upper_leg = new My_Mesh;
	right_upper_leg->load_obj("texture/robot_cube.obj");
	right_upper_leg->set_texture_file("texture/upper_leg.png");	// 指定纹理图像文件
	right_upper_leg->set_normalization(false);
	right_upper_leg->set_scale(1, 1, 1);
	right_upper_leg->set_translate((0.5*TORSO_WIDTH-0.5*UPPER_LEG_WIDTH) , 0.0, 0.0);	// 平移
	robot->add_mesh(right_upper_leg);
	my_meshs.push_back(right_upper_leg);
	robot_shadow->add_mesh(right_upper_leg);

	// 右小腿
	My_Mesh* right_lower_leg = new My_Mesh;
	right_lower_leg->load_obj("texture/robot_cube.obj");
	right_lower_leg->set_texture_file("texture/lower_leg.png");	// 指定纹理图像文件
	right_lower_leg->set_normalization(false);
	right_lower_leg->set_scale(1, 1, 1);
	right_lower_leg->set_translate(0.0 , -UPPER_LEG_HEIGHT, 0.0);	// 平移
	robot->add_mesh(right_lower_leg);
	my_meshs.push_back(right_lower_leg);
	robot_shadow->add_mesh(right_lower_leg);
	
}

void draw_world()
{
	
	My_Mesh* ground_piece;
	float row = 25;
	float column = 25;
	for(int i=0; i<row; i++)
	{
		
		for(int j=0; j<column; j++)
		{
			ground_piece = new My_Mesh;
			ground_piece->load_obj("texture/cube.obj");
			ground_piece->set_texture_file("texture/grass_real.jpg");	// 指定纹理图像文件
			ground_piece->set_normalization(false);
			ground_piece->set_scale(1,1,1);
			ground_piece->set_translate(-row+i*2+1, -0.0001, -column+j*2+1);	// 平移
			ground_piece->set_rotate(0, 0, 0);	// 旋转
			ground->add_mesh(ground_piece);
			my_meshs.push_back(ground_piece);
		}
		
	}

	My_Mesh* sky;
	sky = new My_Mesh;
	sky->load_obj("texture/sky_box2.obj");
	sky->set_texture_file("texture/sky.jpg");	// 指定纹理图像文件
	sky->set_normalization(false);
	sky->set_scale(row*2, 10, column*2);
	sky->set_translate(0, 0.5, 0);	// 平移
	sky->set_rotate(0, 0, 0);	// 旋转
	ground->add_mesh(sky);
	my_meshs.push_back(sky);
	
}

void draw_article()
{
	// 桌子
	My_Mesh* table = new My_Mesh;
	table->load_obj("texture/table.obj");
	table->set_texture_file("texture/table.png");	// 指定纹理图像文件
	table->set_normalization(true);
	table->set_scale(1, 1, 1);
	table->set_translate(1, table->get_uptolevel(), 0);	// 平移
	table->set_rotate(-90, 0, 0);	// 旋转
	article->add_mesh(table);
	my_meshs.push_back(table);
	// 桌子的阴影
	shadow->add_mesh(table);

}

int main( int argc, char **argv )
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window.width, window.height);
	glutCreateWindow("2017153042_杨翔闳_大作业");

	#ifdef WIN32
		glutInitContextVersion(3, 2);
		glutInitContextProfile(GLUT_CORE_PROFILE);
		glewExperimental = GL_TRUE;
		glewInit();
	#endif

	init();
	
	std::string vshader, fshader, vshadowshader, fshadowshader, frobotshader,
						vlightshader, flightshader;
	#ifdef __APPLE__
		vshader = "shaders/vshader_mac.glsl";
		fshader = "shaders/fshader_mac.glsl";
	#else
		vshader = "shaders/vshader.glsl";
		fshader = "shaders/fshader.glsl";
		vshadowshader = "shaders/vshadow.glsl";
		fshadowshader = "shaders/fshadow.glsl";
		frobotshader = "shaders/frobot.glsl";
		vlightshader = "shaders/vshader_light.glsl";
		flightshader = "shaders/fshader_light.glsl";
	#endif

	// @TODO: 参考实验4.1，读取wawa.obj和table.obj模型并贴图、设置绕y轴旋转
	ground = new Mesh_Painter;
	shadow = new Mesh_Painter;
	article = new Mesh_Painter;
	robot = new Mesh_Painter;
	robot_shadow = new Mesh_Painter;
	newhouse = new Mesh_Painter;
	car = new Mesh_Painter;
	carshadow = new Mesh_Painter;
	tree = new Mesh_Painter;
	treeshadow = new Mesh_Painter;
	oldhouse = new Mesh_Painter;
	speedup = new Mesh_Painter;
	speedup_shadow = new Mesh_Painter;
	
	load_obj("texture/tree.obj", tree, treeshadow,
			0.00015, 0.00015, 0.00015,
			15900, 0, 0,
			0, 0, 0, false, true);

	load_obj("texture/chair.obj", tree, treeshadow,
			0.01, 0.01, 0.01,
			120, -5, 55,
			0, -170, 0, true, true);
	/*
	load_obj("texture/house.obj", newhouse, treeshadow,
			0.01, 0.01, 0.01,
			-600, 0, 20,
			0, 0, 0, false, false);
	
	load_obj("texture/medieval-house-2.obj", oldhouse, carshadow, 
			0.5, 0.5, 0.5,
			0.5, 1, -14,
			0, 0, 0, false, false);
	
	load_obj("texture/police-car.obj", car, carshadow,
			0.6, 0.6, 0.6 ,
			positionx_car, positiony_car, positionz_car,
			0, 0, 0,
			false, true);
	*/
	for(int i=0; i<13; i++)
	{
		load_obj("texture/speedup.obj", speedup, speedup_shadow,
			1, 1, 1 ,
			speedpo[i][0], 0.4, speedpo[i][1],
			0, 0, 0,
			false, true);
	}
	
	
	draw_world();
	draw_robot();
	draw_article();

	speedup->set_light(true);
	speedup->init_shaders2(vlightshader.c_str(), flightshader.c_str());
	speedup->update_vertex_buffer4();
	speedup->update_texture2();

	speedup_shadow->set_light(true);
	speedup_shadow->init_shaders2(vlightshader.c_str(), flightshader.c_str());
	speedup_shadow->update_vertex_buffer4();
	speedup_shadow->update_texture2();
	
	oldhouse->set_light(true);
	oldhouse->init_shaders2(vlightshader.c_str(), flightshader.c_str());
	oldhouse->update_vertex_buffer4();
	oldhouse->update_texture2();

	newhouse->set_light(true);
	newhouse->init_shaders2(vlightshader.c_str(), flightshader.c_str());
	newhouse->update_vertex_buffer4();
	newhouse->update_texture2();

	
	tree->set_light(true);
	tree->init_shaders2(vlightshader.c_str(), flightshader.c_str());
	tree->update_vertex_buffer4();
	tree->update_texture2();
	

	ground->set_light(false);
	ground->init_shaders(vshader.c_str(), fshader.c_str());
	ground->update_vertex_buffer();
	ground->update_texture();

	article->set_light(true);
	article->init_shaders(vlightshader.c_str(), flightshader.c_str());
	article->update_vertex_buffer();
	article->update_texture();
	
	shadow->set_light(false);
	shadow->init_shaders(vshadowshader.c_str(), fshadowshader.c_str());
	shadow->update_vertex_buffer();
	shadow->update_texture();

	treeshadow->set_light(false);
	treeshadow->init_shaders2(vshadowshader.c_str(), fshadowshader.c_str());
	treeshadow->update_vertex_buffer4();
	treeshadow->update_texture2();

	robot->set_light(false);
	robot->init_shaders(vshader.c_str(), fshader.c_str());
	robot->update_vertex_buffer();
	robot->update_texture();
	
	robot_shadow->set_light(false);
	robot_shadow->init_shaders(vshadowshader.c_str(), fshadowshader.c_str());
	robot_shadow->update_vertex_buffer();
	robot_shadow->update_texture();
	

	car->set_light(true);
	car->init_shaders2(vlightshader.c_str(), flightshader.c_str());
	car->update_vertex_buffer4();
	car->update_texture2();

	carshadow->set_light(false);
	carshadow->init_shaders2(vshadowshader.c_str(), fshadowshader.c_str());
	carshadow->update_vertex_buffer4();
	carshadow->update_texture2();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
#ifdef WIN32
	glutMouseWheelFunc(mouseWheel);
#endif
	glutSpecialFunc(special);
	glutSpecialUpFunc(keyup);
	glutIdleFunc(idle);

	glutMainLoop();

	for (unsigned int i = 0; i < my_meshs.size(); i++)
	{
		delete my_meshs[i];
	}
	delete ground;
	delete article;
	delete shadow;
	delete robot;
	delete robot_shadow;
	delete newhouse;
	delete oldhouse;
	delete car;
	delete tree;
	delete carshadow;

	return 0;
}
