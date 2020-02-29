#include "Mesh_Painter.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


Mesh_Painter::Mesh_Painter()
{
}

Mesh_Painter::~Mesh_Painter()
{
}

void Mesh_Painter::draw_meshes()
{
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		// 指定使用渲染器，不同的模型可以指定不同的渲染器
		// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
		glUseProgram(this->program_all[i]);
		
		#ifdef __APPLE__
			glBindVertexArrayAPPLE(this->vao_all[i]);
		#else
			glBindVertexArray(this->vao_all[i]);
		#endif

 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_2D, this->textures_all[i]);// 该语句必须，否则将只使用同一个纹理进行绘制

		float x0,y0,z0,x1, y1, z1, x2, y2, z2;
		this->m_my_meshes_[i]->get_scale(x0,y0,z0);
		this->m_my_meshes_[i]->get_translate(x1, y1, z1);
		this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
		
		modelMatrix = mat4(1.0) * Scale(x0,y0,z0) * Translate(x1, y1, z1) * RotateZ(z2) * RotateY(y2) * RotateX(x2) ;	// 模型变换矩阵
		mat4 mm = modelMatrix;
		glUniformMatrix4fv(modelMatrix_all[i], 1, GL_TRUE, modelMatrix);

		viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
		glUniformMatrix4fv(modelView_all[i], 1, GL_TRUE, viewMatrix);

		projMatrix = perspective(fov,aspect,near,far);
		glUniformMatrix4fv(projection_all[i], 1, GL_TRUE, projMatrix);

		glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->num_faces() * 3);
		
		if(this->light_reflex)
		{
			GLint LightPosition = glGetUniformLocation(this->program_all[i], "LightPosition");
			// 将阴影矩阵传入点着色器
			// 根据材质反射率计算光源三个分量
			vec4 ambient_product = light_ambient * table_ambient;
			vec4 diffuse_product = light_diffuse * table_diffuse;
			vec4 specular_product = light_specular * table_specular;
			
			// 光源位置、材质高光系数、光源三种分量传入点着色器和片元着色器
			glUniform4fv(LightPosition, 1, light_position );
			glUniform1f(Shininess_all[i], table_shininess);
			glUniform4fv(AmbientProduct_all[i], 1, ambient_product );
			glUniform4fv(DiffuseProduct_all[i],  1, diffuse_product );
			glUniform4fv(SpecularProduct_all[i],  1, specular_product );
		
		}
		glUseProgram(0);
	}
};

void Mesh_Painter::draw_shadowmeshes()
{
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		// 指定使用渲染器，不同的模型可以指定不同的渲染器
		// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
		glUseProgram(this->program_all[i]);
		
		#ifdef __APPLE__
			glBindVertexArrayAPPLE(this->vao_all[i]);
		#else
			//激活一个顶点数组
			glBindVertexArray(this->vao_all[i]);
		#endif

 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_2D, this->textures_all[i]);// 该语句必须，否则将只使用同一个纹理进行绘制

		float x1, y1, z1, x2, y2, z2;
		this->m_my_meshes_[i]->get_translate(x1, y1, z1);
		this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
		
		modelMatrix = mat4(1.0) * Translate(x1, y1, z1) * RotateZ(z2) * RotateY(y2) * RotateX(x2) ;	// 模型变换矩阵
		mat4 shadowMatrix = getShadowMatrix() * modelMatrix;
		glUniformMatrix4fv(modelMatrix_all[i], 1, GL_TRUE, shadowMatrix);

		viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
		glUniformMatrix4fv(modelView_all[i], 1, GL_TRUE, viewMatrix);

		projMatrix = perspective(fov,aspect,near,far);
		glUniformMatrix4fv(projection_all[i], 1, GL_TRUE, projMatrix);

		glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->num_faces() * 3);
		
		glUseProgram(0);
	}
};

void Mesh_Painter::draw_robot_shadow()
{
    mat4 model_view = mat4(1.0);
	mat4 model_view_torso = mat4(1.0);
	mat4 instance = mat4(1.0);
	mat4 translateM;
	mat4 scaleM;
	float x1, y1, z1, x2, y2, z2;
	
	// 躯干0
	this->m_my_meshes_[0]->get_translate(x1, y1, z1);
	this->m_my_meshes_[0]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(positionx_robot+robot_movex, positiony_robot, positionz_robot+robot_movez) * RotateY(robotturn_right);	// 模型变换矩阵
	model_view_torso = model_view;
	// 本节点局部变换矩阵
	translateM = Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0);
	scaleM = Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_LENGTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 0, true);

    // 头部1
	this->m_my_meshes_[1]->get_translate(x1, y1, z1);
	this->m_my_meshes_[1]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateY(theta[1]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, 0.5 *  HEAD_HEIGHT, 0.0);
	scaleM = Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 1, true);

    //绘制四肢
    // 左大臂2
    this->m_my_meshes_[2]->get_translate(x1, y1, z1);
	this->m_my_meshes_[2]->get_rotate(x2, y2, z2);
	model_view = model_view_torso;
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[2]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * UPPER_ARM_HEIGHT, 0.0);
	scaleM = Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 2, true);
	
    // 左小臂3
    this->m_my_meshes_[3]->get_translate(x1, y1, z1);
	this->m_my_meshes_[3]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[3]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * LOWER_ARM_HEIGHT, 0.0);
	scaleM = Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 3, true);

    // 右大臂4
    this->m_my_meshes_[4]->get_translate(x1, y1, z1);
	this->m_my_meshes_[4]->get_rotate(x2, y2, z2);
	model_view = model_view_torso;
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[4]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * UPPER_ARM_HEIGHT, 0.0);
	scaleM = Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 4, true);
	
    // 右小臂5
    this->m_my_meshes_[5]->get_translate(x1, y1, z1);
	this->m_my_meshes_[5]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[5]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * LOWER_ARM_HEIGHT, 0.0);
	scaleM = Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 5, true);

    // 左大腿6
    this->m_my_meshes_[6]->get_translate(x1, y1, z1);
	this->m_my_meshes_[6]->get_rotate(x2, y2, z2);
	model_view = model_view_torso;
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[6]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * UPPER_LEG_HEIGHT, 0.0);
	scaleM = Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 6, true);
	
    // 左小腿7
    this->m_my_meshes_[7]->get_translate(x1, y1, z1);
	this->m_my_meshes_[7]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[7]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * LOWER_LEG_HEIGHT, 0.0);
	scaleM = Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 7, true);

    // 右大腿8
    this->m_my_meshes_[8]->get_translate(x1, y1, z1);
	this->m_my_meshes_[8]->get_rotate(x2, y2, z2);
	model_view = model_view_torso;
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[8]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * UPPER_LEG_HEIGHT, 0.0);
	scaleM = Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 8, true);
	
    // 右小腿9
    this->m_my_meshes_[9]->get_translate(x1, y1, z1);
	this->m_my_meshes_[9]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[9]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * LOWER_LEG_HEIGHT, 0.0);
	scaleM = Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 9, true);
}

void Mesh_Painter::draw_robot()
{
    mat4 model_view = mat4(1.0);
	mat4 model_view_torso = mat4(1.0);
	mat4 instance = mat4(1.0);
	mat4 translateM;
	mat4 scaleM;
	float x1, y1, z1, x2, y2, z2;
	
	// 躯干0
	this->m_my_meshes_[0]->get_translate(x1, y1, z1);
	this->m_my_meshes_[0]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(positionx_robot+robot_movex, positiony_robot, positionz_robot+robot_movez) * RotateY(robotturn_right);	// 模型变换矩阵
	model_view_torso = model_view;
	// 本节点局部变换矩阵
	translateM = Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0);
	scaleM = Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_LENGTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 0, false);

    // 头部1
	this->m_my_meshes_[1]->get_translate(x1, y1, z1);
	this->m_my_meshes_[1]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateY(theta[1]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, 0.5 *  HEAD_HEIGHT, 0.0);
	scaleM = Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 1, false);

    //绘制四肢
    // 左大臂2
    this->m_my_meshes_[2]->get_translate(x1, y1, z1);
	this->m_my_meshes_[2]->get_rotate(x2, y2, z2);
	model_view = model_view_torso;
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[2]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * UPPER_ARM_HEIGHT, 0.0);
	scaleM = Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 2, false);
	
    // 左小臂3
    this->m_my_meshes_[3]->get_translate(x1, y1, z1);
	this->m_my_meshes_[3]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[3]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * LOWER_ARM_HEIGHT, 0.0);
	scaleM = Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 3, false);

    // 右大臂4
    this->m_my_meshes_[4]->get_translate(x1, y1, z1);
	this->m_my_meshes_[4]->get_rotate(x2, y2, z2);
	model_view = model_view_torso;
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[4]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * UPPER_ARM_HEIGHT, 0.0);
	scaleM = Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 4, false);
	
    // 右小臂5
    this->m_my_meshes_[5]->get_translate(x1, y1, z1);
	this->m_my_meshes_[5]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[5]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * LOWER_ARM_HEIGHT, 0.0);
	scaleM = Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 5, false);

    // 左大腿6
    this->m_my_meshes_[6]->get_translate(x1, y1, z1);
	this->m_my_meshes_[6]->get_rotate(x2, y2, z2);
	model_view = model_view_torso;
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[6]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * UPPER_LEG_HEIGHT, 0.0);
	scaleM = Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 6, false);
	
    // 左小腿7
    this->m_my_meshes_[7]->get_translate(x1, y1, z1);
	this->m_my_meshes_[7]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[7]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * LOWER_LEG_HEIGHT, 0.0);
	scaleM = Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 7, false);

    // 右大腿8
    this->m_my_meshes_[8]->get_translate(x1, y1, z1);
	this->m_my_meshes_[8]->get_rotate(x2, y2, z2);
	model_view = model_view_torso;
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[8]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * UPPER_LEG_HEIGHT, 0.0);
	scaleM = Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 8, false);
	
    // 右小腿9
    this->m_my_meshes_[9]->get_translate(x1, y1, z1);
	this->m_my_meshes_[9]->get_rotate(x2, y2, z2);
	model_view = model_view * Translate(x1, y1, z1) * RotateX(theta[9]);	// 模型变换矩阵
	// 本节点局部变换矩阵
	translateM = Translate(0.0, -0.5 * LOWER_LEG_HEIGHT, 0.0);
	scaleM = Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH);
	instance = translateM * scaleM;
	draw_robot_detail(model_view*instance, 9, false);
}

void Mesh_Painter::draw_robot_detail(mat4 model_view, int i, bool is_shadow)
{
	// 指定使用渲染器，不同的模型可以指定不同的渲染器
	// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
	glUseProgram(this->program_all[i]);
	
	#ifdef __APPLE__
		glBindVertexArrayAPPLE(this->vao_all[i]);
	#else
		glBindVertexArray(this->vao_all[i]);
	#endif

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->textures_all[i]);// 该语句必须，否则将只使用同一个纹理进行绘制
	
	modelMatrix = model_view;
	if(is_shadow)
	{
		mat4 shadowMatrix = getShadowMatrix() * modelMatrix;
		glUniformMatrix4fv(modelMatrix_all[i], 1, GL_TRUE, shadowMatrix);
	}
	else
	{
		glUniformMatrix4fv(modelMatrix_all[i], 1, GL_TRUE, modelMatrix);
	}
	viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
	glUniformMatrix4fv(modelView_all[i], 1, GL_TRUE, viewMatrix);

	projMatrix = perspective(fov,aspect,near,far);
	glUniformMatrix4fv(projection_all[i], 1, GL_TRUE, projMatrix);

	glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->num_faces() * 3);
	
	glUseProgram(0);
};

void Mesh_Painter::update_texture()
{
	this->textures_all.clear();

	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint textures;

		// 调用stb_image生成纹理
		glGenTextures(1, &textures);
		load_texture_STBImage(this->m_my_meshes_[i]->get_texture_file(), textures);

		// 将生成的纹理传给shader
		glBindTexture(GL_TEXTURE_2D, textures);
		glUniform1i(glGetUniformLocation(this->program_all[i], "texture"), 0);
		this->textures_all.push_back(textures);
	}
};

void Mesh_Painter::load_texture_STBImage(std::string file_name, GLuint& m_texName)
{
	int width, height, channels = 0;
	unsigned char *pixels = NULL;
	stbi_set_flip_vertically_on_load(true);
	pixels = stbi_load(file_name.c_str(), &width, &height, &channels, 0);

	// 调整行对齐格式
	if(width*channels%4!=0) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLenum format = GL_RGB;
	// 设置通道格式
	switch (channels){
	case 1: format=GL_RED;break;
	case 3: format=GL_RGB;break;
	case 4: format=GL_RGBA;break;
	default: format=GL_RGB;break;
	}

	// 绑定纹理对象
	glBindTexture(GL_TEXTURE_2D, m_texName);

	// 指定纹理的放大，缩小滤波，使用线性方式，即当图片放大的时候插值方式
	// 将图片的rgb数据上传给opengl
	glTexImage2D(
		GL_TEXTURE_2D,	// 指定目标纹理，这个值必须是GL_TEXTURE_2D
		0,				// 执行细节级别，0是最基本的图像级别，n表示第N级贴图细化级别
		format,			// 纹理数据的颜色格式(GPU显存)
		width,			// 宽度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		height,			// 高度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		0,				// 指定边框的宽度。必须为0
		format,			// 像素数据的颜色格式(CPU内存)
		GL_UNSIGNED_BYTE,	// 指定像素数据的数据类型
		pixels			// 指定内存中指向图像数据的指针
	);
	
	// 生成多级渐远纹理，多消耗1/3的显存，较小分辨率时获得更好的效果
	glGenerateMipmap(GL_TEXTURE_2D);

	// 指定插值方法
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// 恢复初始对齐格式
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	// 释放图形内存
	stbi_image_free(pixels);
};

void Mesh_Painter::update_vertex_buffer()
{
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();

	// 顶点坐标，法线，颜色，纹理坐标到shader的映射
	for (unsigned int m_i = 0; m_i < this->m_my_meshes_.size(); m_i++)
	{
		int num_face = this->m_my_meshes_[m_i]->num_faces();
		int num_vertex = this->m_my_meshes_[m_i]->num_vertices();

		const VertexList& vertex_list = this->m_my_meshes_[m_i]->get_vertices();
		const NormalList& normal_list = this->m_my_meshes_[m_i]->get_normals();
		const FaceList&  face_list = this->m_my_meshes_[m_i]->get_faces();
		const STLVectorf& color_list = this->m_my_meshes_[m_i]->get_colors();
		const VtList& vt_list = this->m_my_meshes_[m_i]->get_vts();

		// 创建顶点数组对象
		GLuint vao;
		#ifdef __APPLE__
			glGenVertexArraysAPPLE(1, &vao);
			glBindVertexArrayAPPLE(vao);
		#else
			glGenVertexArrays(1, &vao); //声明一个顶点数组对象，告诉如何解析缓存对象中的数据
			glBindVertexArray(vao); //激活一个顶点数组对象
		#endif

		GLuint buffer;
		glGenBuffers(1, &buffer); //声明一个缓存对象ID
		glBindBuffer(GL_ARRAY_BUFFER, buffer); //激活(绑定)其中一个缓存
		//分配内存并拷贝数据到显存
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(vec3)*num_face * 3
			+ sizeof(vec3)*num_face * 3
			+ sizeof(vec3)*num_face * 3
			+ sizeof(vec2)*num_face * 3, NULL, GL_STATIC_DRAW);
	
		// ------------------------------------------------------------------------
		// 获得足够的空间存储坐标，颜色，法线以及纹理坐标等，并将它们映射给shader
		// Specify an offset to keep track of where we're placing data in our
		// vertex array buffer.  We'll use the same technique when we
		// associate the offsets with vertex attribute pointers.
		GLintptr offset = 0;
		point3f  p_center_;
		float d;
		// 判断是否进行归一化
		if (this->m_my_meshes_[m_i]->whether_to_normalize())
		{
			p_center_ = this->m_my_meshes_[m_i]->get_center();	// bounding box的中心点坐标
			point3f p_min_box_, p_max_box_;
			this->m_my_meshes_[m_i]->get_boundingbox(p_min_box_, p_max_box_);
			d = p_min_box_.distance(p_max_box_);				// bounding box的对角线长度
		}
		else
		{
			p_center_ = point3f(0.0, 0.0, 0.0);
			d = 1.0;
		}

		// -------------------- 实现顶点到shader的映射 ------------------------------
		vec3* points = new vec3[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			//实现顶点坐标到shader的映射
			int index = face_list[3 * i];
			points[3 * i] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
								(vertex_list[index * 3 + 1] - p_center_.y) / d,
								(vertex_list[index * 3 + 2] - p_center_.z) / d);
			index = face_list[3 * i + 1];
			points[3 * i + 1] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d);
			index = face_list[3 * i + 2];
			points[3 * i + 2] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d);
			
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3)*num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------
		
		// -------------------- 实现法线到shader的映射 ------------------------------
		points = new vec3[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			//@TODO: 参考实验4.1，在这里添加代码实现法线到shader的映射
			int index = face_list[3 * i];
			points[3 * i] = vec3((normal_list[index * 3 + 0]),
								(normal_list[index * 3 + 1]),
								(normal_list[index * 3 + 2]));
			index = face_list[3 * i + 1];
			points[3 * i + 1] = vec3((normal_list[index * 3 + 0]),
									(normal_list[index * 3 + 1]),
									(normal_list[index * 3 + 2]));
			index = face_list[3 * i + 2];
			points[3 * i + 2] = vec3((normal_list[index * 3 + 0]),
									(normal_list[index * 3 + 1]),
									(normal_list[index * 3 + 2]));
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------

		// -------------------- 实现颜色到shader的映射 ------------------------------
		points = new vec3[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			//@TODO: 参考实验4.1，在这里添加代码实现颜色到shader的映射
			int index = face_list[3 * i];
			points[3 * i] = vec3((color_list[index * 3 + 0]),
								(color_list[index * 3 + 1]),
								(color_list[index * 3 + 2]));
			index = face_list[3 * i + 1];
			points[3 * i + 1] = vec3((color_list[index * 3 + 0]),
									(color_list[index * 3 + 1]),
									(color_list[index * 3 + 2]));
			index = face_list[3 * i + 2];
			points[3 * i + 2] = vec3((color_list[index * 3 + 0]),
									(color_list[index * 3 + 1]),
									(color_list[index * 3 + 2]));
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------

		// ------------------- 实现纹理坐标到shader的映射 -----------------------------
		vec2* points_2 = new vec2[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			//@TODO: 参考实验4.1，在这里添加代码实现纹理坐标到shader的映射
			points_2[i * 3] = vec2(vt_list[i * 6 + 0], vt_list[i * 6 + 1]);
			points_2[i * 3 + 1] = vec2(vt_list[i * 6 + 2], vt_list[i * 6 + 3]);
			points_2[i * 3 + 2] = vec2(vt_list[i * 6 + 4], vt_list[i * 6 + 5]);
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * num_face * 3, points_2);
		offset += sizeof(vec2) * num_face * 3;
		delete[] points_2;
		// ------------------------------------------------------------------------

		// 加载着色器并使用生成的着色器程序
		offset = 0;
		// 指定vPosition在shader中使用时的位置
		GLuint vPosition;
		vPosition = glGetAttribLocation(this->program_all[m_i], "vPosition");
		//启用与“vPosition”相关联的顶点数组
		glEnableVertexAttribArray(vPosition);
		// 填充当前绑定的顶点数组对象
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;

		// 指定vNormal在shader中使用时的位置
		GLuint vNormal;
		vNormal = glGetAttribLocation(this->program_all[m_i], "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		
		// 指定vColor在shader中使用时的位置
		GLuint vColor;
		vColor = glGetAttribLocation(this->program_all[m_i], "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		
		//指定vTexCoord在shader中使用时的位置
		GLuint vTexCoord;
		vTexCoord = glGetAttribLocation(this->program_all[m_i], "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		
		this->vao_all.push_back(vao);
		this->buffer_all.push_back(buffer);
		this->vPosition_all.push_back(vPosition);
		this->vColor_all.push_back(vColor);
		this->vTexCoord_all.push_back(vTexCoord);
		this->vNormal_all.push_back(vNormal);
	}
};

void Mesh_Painter::update_vertex_buffer4()
{
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();

	int index_=0;
	// 顶点坐标，法线，颜色，纹理坐标到shader的映射
	for (unsigned int m_i = 0; m_i < this->m_my_meshes_.size(); m_i++)
	{
		for(int j=0; j<this->m_my_meshes_[m_i]->mtlname.size(); j++)
		{
			//int num_vertex = this->m_my_meshes_[m_i]->num_vertices();

			const std::vector<float>& vertex_list			= this->m_my_meshes_[m_i]->m_vertice_list_2;
			const std::vector<float>& normal_list			= this->m_my_meshes_[m_i]->m_normal_list_2;
			const std::vector<float>& color_list			= this->m_my_meshes_[m_i]->m_color_list_2;
			const std::vector<float>& vt_list				= this->m_my_meshes_[m_i]->m_vt_list_2;
			const std::vector<unsigned int>& vertex_index	= this->m_my_meshes_[m_i]->m_vertice_index_2;
			const std::vector<unsigned int>& normal_index	= this->m_my_meshes_[m_i]->m_normal_index_2;
			const std::vector<unsigned int>& color_index	= this->m_my_meshes_[m_i]->m_color_index_2;
			const std::vector<unsigned int>& vt_index		= this->m_my_meshes_[m_i]->m_vt_index_2;

			// 创建顶点数组对象
			GLuint vao;
			#ifdef __APPLE__
				glGenVertexArraysAPPLE(1, &vao);
				glBindVertexArrayAPPLE(vao);
			#else
				glGenVertexArrays(1, &vao); //声明一个顶点数组对象，告诉如何解析缓存对象中的数据
				glBindVertexArray(vao); //激活一个顶点数组对象
			#endif

			int num_face = this->m_my_meshes_[m_i]->mtlindex[j+1];
			int fnum = this->m_my_meshes_[m_i]->fnum[j+1];
			if(fnum==4)
			{
				GLuint buffer;
				glGenBuffers(1, &buffer); //声明一个缓存对象ID
				glBindBuffer(GL_ARRAY_BUFFER, buffer); //激活(绑定)其中一个缓存
				//分配内存并拷贝数据到显存
				glBufferData(GL_ARRAY_BUFFER,
					sizeof(vec3)*num_face * 6
					+ sizeof(vec3)*num_face * 6
					+ sizeof(vec3)*num_face * 6
					+ sizeof(vec2)*num_face * 6, NULL, GL_STATIC_DRAW);
			
				// ------------------------------------------------------------------------
				// 获得足够的空间存储坐标，颜色，法线以及纹理坐标等，并将它们映射给shader
				// Specify an offset to keep track of where we're placing data in our
				// vertex array buffer.  We'll use the same technique when we
				// associate the offsets with vertex attribute pointers.
				GLintptr offset = 0;
				point3f  p_center_;
				float d;
				// 判断是否进行归一化
				if (this->m_my_meshes_[m_i]->whether_to_normalize())
				{
					p_center_ = this->m_my_meshes_[m_i]->get_center();	// bounding box的中心点坐标
					point3f p_min_box_, p_max_box_;
					this->m_my_meshes_[m_i]->get_boundingbox(p_min_box_, p_max_box_);
					d = p_min_box_.distance(p_max_box_);				// bounding box的对角线长度
				}
				else
				{
					p_center_ = point3f(0.0, 0.0, 0.0);
					d = 1.0;
				}

				// -------------------- 实现顶点到shader的映射 ------------------------------
				vec3* points = new vec3[num_face * 6];
				for (int i = 0; i < num_face; i++)
				{
					//实现顶点坐标到shader的映射
					int index;
					index = vertex_index[4 * i];
					points[6 * i] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
										(vertex_list[index * 3 + 1] - p_center_.y) / d,
										(vertex_list[index * 3 + 2] - p_center_.z) / d);

					index = vertex_index[4 * i + 1];
					points[6 * i + 1] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
											(vertex_list[index * 3 + 1] - p_center_.y) / d,
											(vertex_list[index * 3 + 2] - p_center_.z) / d);
					index = vertex_index[4 * i + 2];
					points[6 * i + 2] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
											(vertex_list[index * 3 + 1] - p_center_.y) / d,
											(vertex_list[index * 3 + 2] - p_center_.z) / d);

					index = vertex_index[4 * i];						
					points[6 * i + 3] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
										(vertex_list[index * 3 + 1] - p_center_.y) / d,
										(vertex_list[index * 3 + 2] - p_center_.z) / d);

					index = vertex_index[4 * i + 2];
					points[6 * i + 4] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
											(vertex_list[index * 3 + 1] - p_center_.y) / d,
											(vertex_list[index * 3 + 2] - p_center_.z) / d);

					index = vertex_index[4 * i + 3];
					points[6 * i + 5] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
											(vertex_list[index * 3 + 1] - p_center_.y) / d,
											(vertex_list[index * 3 + 2] - p_center_.z) / d);

				}
				glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3)*num_face * 6, points);
				offset += sizeof(vec3) * num_face * 6;
				delete[] points;
				// ------------------------------------------------------------------------
				
				// -------------------- 实现法线到shader的映射 ------------------------------
				points = new vec3[num_face * 6];
				for (int i = 0; i < num_face; i++)
				{
					//@TODO: 参考实验4.1，在这里添加代码实现法线到shader的映射
					int index = normal_index[4 * i];
					points[6 * i] = vec3((normal_list[index * 3 + 0]),
										(normal_list[index * 3 + 1]),
										(normal_list[index * 3 + 2]));
					index = normal_index[4 * i + 1];
					points[6 * i + 1] = vec3((normal_list[index * 3 + 0]),
											(normal_list[index * 3 + 1]),
											(normal_list[index * 3 + 2]));
					index = normal_index[4 * i + 2];
					points[6 * i + 2] = vec3((normal_list[index * 3 + 0]),
											(normal_list[index * 3 + 1]),
											(normal_list[index * 3 + 2]));

					index = normal_index[4 * i];
					points[6 * i + 3] = vec3((normal_list[index * 3 + 0]),
										(normal_list[index * 3 + 1]),
										(normal_list[index * 3 + 2]));
					index = normal_index[4 * i + 2];
					points[6 * i + 4] = vec3((normal_list[index * 3 + 0]),
											(normal_list[index * 3 + 1]),
											(normal_list[index * 3 + 2]));
					index = normal_index[4 * i + 3];
					points[6 * i + 5] = vec3((normal_list[index * 3 + 0]),
											(normal_list[index * 3 + 1]),
											(normal_list[index * 3 + 2]));
				}
				glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 6, points);
				offset += sizeof(vec3) * num_face * 6;
				delete[] points;
				// ------------------------------------------------------------------------

				// -------------------- 实现颜色到shader的映射 ------------------------------
				points = new vec3[num_face * 6];
				for (int i = 0; i < num_face; i++)
				{
					//@TODO: 参考实验4.1，在这里添加代码实现颜色到shader的映射
					int index = color_index[4 * i];
					points[6 * i] = vec3((color_list[index * 3 + 0]),
										(color_list[index * 3 + 1]),
										(color_list[index * 3 + 2]));
					index = color_index[4 * i + 1];
					points[6 * i + 1] = vec3((color_list[index * 3 + 0]),
											(color_list[index * 3 + 1]),
											(color_list[index * 3 + 2]));
					index = color_index[4 * i + 2];
					points[6 * i + 2] = vec3((color_list[index * 3 + 0]),
											(color_list[index * 3 + 1]),
											(color_list[index * 3 + 2]));

					index = color_index[4 * i];
					points[6 * i + 3] = vec3((color_list[index * 3 + 0]),
										(color_list[index * 3 + 1]),
										(color_list[index * 3 + 2]));
					index = color_index[4 * i + 2];
					points[6 * i + 4] = vec3((color_list[index * 3 + 0]),
											(color_list[index * 3 + 1]),
											(color_list[index * 3 + 2]));
					index = color_index[4 * i + 3];
					points[6 * i + 5] = vec3((color_list[index * 3 + 0]),
											(color_list[index * 3 + 1]),
											(color_list[index * 3 + 2]));

				}
				glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 6, points);
				offset += sizeof(vec3) * num_face * 6;
				delete[] points;
				// ------------------------------------------------------------------------

				// ------------------- 实现纹理坐标到shader的映射 -----------------------------
				vec2* points_2 = new vec2[num_face * 6];
				for (int i = 0; i < num_face; i++)
				{
					int index = vt_index[4 * i];
					points_2[i * 6] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
					
					index = vt_index[4 * i + 1];
					points_2[i * 6 + 1] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);

					index = vt_index[4 * i + 2];
					points_2[i * 6 + 2] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);

					index = vt_index[4 * i];
					points_2[i * 6 + 3] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
					
					index = vt_index[4 * i + 2];
					points_2[i * 6 + 4] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);

					index = vt_index[4 * i + 3];
					points_2[i * 6 + 5] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
				}
				glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * num_face * 6, points_2);
				offset += sizeof(vec2) * num_face * 6;
				delete[] points_2;
				// ------------------------------------------------------------------------

				// 加载着色器并使用生成的着色器程序
				offset = 0;
				// 指定vPosition在shader中使用时的位置
				GLuint vPosition;
				vPosition = glGetAttribLocation(this->program_all[index_], "vPosition");
				//启用与“vPosition”相关联的顶点数组
				glEnableVertexAttribArray(vPosition);
				// 填充当前绑定的顶点数组对象
				glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
				offset += sizeof(vec3) * num_face * 6;

				// 指定vNormal在shader中使用时的位置
				GLuint vNormal;
				vNormal = glGetAttribLocation(this->program_all[index_], "vNormal");
				glEnableVertexAttribArray(vNormal);
				glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
				offset += sizeof(vec3) * num_face * 6;
				
				// 指定vColor在shader中使用时的位置
				GLuint vColor;
				vColor = glGetAttribLocation(this->program_all[index_], "vColor");
				glEnableVertexAttribArray(vColor);
				glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
				offset += sizeof(vec3) * num_face * 6;
				
				//指定vTexCoord在shader中使用时的位置
				GLuint vTexCoord;
				vTexCoord = glGetAttribLocation(this->program_all[index_], "vTexCoord");
				glEnableVertexAttribArray(vTexCoord);
				glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
				
				this->vao_all.push_back(vao);
				this->buffer_all.push_back(buffer);
				this->vPosition_all.push_back(vPosition);
				this->vColor_all.push_back(vColor);
				this->vTexCoord_all.push_back(vTexCoord);
				this->vNormal_all.push_back(vNormal);
				index_++;
			}
			else 
			{
				GLuint buffer;
				glGenBuffers(1, &buffer);
				glBindBuffer(GL_ARRAY_BUFFER, buffer);
				glBufferData(GL_ARRAY_BUFFER,
							sizeof(vec3)*num_face * 3		// 顶点坐标
							+ sizeof(vec3)*num_face * 3		// 法线
							+ sizeof(vec3)*num_face * 3		// 颜色
							+ sizeof(vec2)*num_face * 3,	// 贴图坐标
							NULL, GL_STATIC_DRAW);

				// ------------------------------------------------------------------------
				// 获得足够的空间存储坐标，颜色，法线以及纹理坐标等，并将它们映射给shader
				// Specify an offset to keep track of where we're placing data in our
				// vertex array buffer.  We'll use the same technique when we
				// associate the offsets with vertex attribute pointers.
				GLintptr offset = 0;
				point3f  p_center_;
				float d;
				// 判断是否进行归一化
				if (this->m_my_meshes_[m_i]->whether_to_normalize())
				{
					p_center_ = this->m_my_meshes_[m_i]->get_center();	// bounding box的中心点坐标
					point3f p_min_box_, p_max_box_;
					this->m_my_meshes_[m_i]->get_boundingbox(p_min_box_, p_max_box_);
					d = p_min_box_.distance(p_max_box_);				// bounding box的对角线长度
				}
				else
				{
					p_center_ = point3f(0.0, 0.0, 0.0);
					d = 1.0;
				}

				// -------------------- 实现顶点到shader的映射 ------------------------------
				vec3* points = new vec3[num_face * 3];
				// 逐面片存储每个面片的三个顶点坐标
				for (int i = 0; i < num_face; i++)
				{
					int index = vertex_index[3 * i]; // 数组标号从0开始
					points[3 * i] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,	// 平移到中心并缩放
										(vertex_list[index * 3 + 1] - p_center_.y) / d,
										(vertex_list[index * 3 + 2] - p_center_.z) / d );
					
					index = vertex_index[3 * i + 1];
					points[3 * i + 1] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
											(vertex_list[index * 3 + 1] - p_center_.y) / d,
											(vertex_list[index * 3 + 2] - p_center_.z) / d );
					
					index = vertex_index[3 * i + 2];
					points[3 * i + 2] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
											(vertex_list[index * 3 + 1] - p_center_.y) / d,
											(vertex_list[index * 3 + 2] - p_center_.z) / d );
					
				}
				glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
				offset += sizeof(vec3) * num_face * 3;
				delete[] points;
				// ------------------------------------------------------------------------
				
				// -------------------- 实现法线到shader的映射 ------------------------------
				points = new vec3[num_face * 3];
				// 逐面片存储每个面片的三个法线
				for (int i = 0; i < num_face; i++)
				{
					int index = normal_index[3 * i];
					points[3 * i] = vec3((normal_list[index * 3 + 0]),
										(normal_list[index * 3 + 1]),
										(normal_list[index * 3 + 2]) );

					index = normal_index[3 * i + 1];
					points[3 * i + 1] = vec3((normal_list[index * 3 + 0]),
											(normal_list[index * 3 + 1]),
											(normal_list[index * 3 + 2]) );
					
					index = normal_index[3 * i + 2];
					points[3 * i + 2] = vec3((normal_list[index * 3 + 0]),
											(normal_list[index * 3 + 1]),
											(normal_list[index * 3 + 2]) );
				}
				glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
				offset += sizeof(vec3) * num_face * 3;
				delete[] points;
				// ------------------------------------------------------------------------

				// -------------------- 实现颜色到shader的映射 ------------------------------
				points = new vec3[num_face * 3];
				for (int i = 0; i < num_face; i++)
				{
					//@TODO: 参考实验4.1，在这里添加代码实现颜色到shader的映射
					int index = color_index[3 * i];
					points[3 * i] = vec3((color_list[index * 3 + 0]),
										(color_list[index * 3 + 1]),
										(color_list[index * 3 + 2]));
					index = color_index[3 * i + 1];
					points[3 * i + 1] = vec3((color_list[index * 3 + 0]),
											(color_list[index * 3 + 1]),
											(color_list[index * 3 + 2]));
					index = color_index[3 * i + 2];
					points[3 * i + 2] = vec3((color_list[index * 3 + 0]),
											(color_list[index * 3 + 1]),
											(color_list[index * 3 + 2]));
				}
				glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
				offset += sizeof(vec3) * num_face * 3;
				delete[] points;
				// ------------------------------------------------------------------------

				// ------------------- 实现纹理坐标到shader的映射 -----------------------------
				vec2* points_2 = new vec2[num_face * 3];
				// 逐面片存储每个面片的三个纹理坐标
				for (int i = 0; i < num_face; i++)
				{
					int index = vt_index[3 * i];
					points_2[i * 3] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
					
					index = vt_index[3 * i + 1];
					points_2[i * 3 + 1] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);

					index = vt_index[3 * i + 2];
					points_2[i * 3 + 2] = vec2(vt_list[index * 2], vt_list[index * 2 + 1]);
				}
				glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * num_face * 3, points_2);
				offset += sizeof(vec2) * num_face * 3;
				delete[] points_2;
				// ------------------------------------------------------------------------

				// 加载着色器并使用生成的着色器程序
				offset = 0;
				// 指定vPosition在shader中使用时的位置
				GLuint vPosition;
				vPosition = glGetAttribLocation(this->program_all[index_], "vPosition");
				//启用与“vPosition”相关联的顶点数组
				glEnableVertexAttribArray(vPosition);
				// 填充当前绑定的顶点数组对象
				glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
				offset += sizeof(vec3) * num_face * 3;

				// 指定vNormal在shader中使用时的位置
				GLuint vNormal;
				vNormal = glGetAttribLocation(this->program_all[index_], "vNormal");
				glEnableVertexAttribArray(vNormal);
				glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
				offset += sizeof(vec3) * num_face * 3;
				
				// 指定vColor在shader中使用时的位置
				GLuint vColor;
				vColor = glGetAttribLocation(this->program_all[index_], "vColor");
				glEnableVertexAttribArray(vColor);
				glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
				offset += sizeof(vec3) * num_face * 3;
				
				//指定vTexCoord在shader中使用时的位置
				GLuint vTexCoord;
				vTexCoord = glGetAttribLocation(this->program_all[index_], "vTexCoord");
				glEnableVertexAttribArray(vTexCoord);
				glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
				
				this->vao_all.push_back(vao);
				this->buffer_all.push_back(buffer);
				this->vPosition_all.push_back(vPosition);
				this->vColor_all.push_back(vColor);
				this->vTexCoord_all.push_back(vTexCoord);
				this->vNormal_all.push_back(vNormal);
				index_++;
			}
		}
	}
};


void Mesh_Painter::update_texture2()
{
	this->textures_all.clear();
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{	
		
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			GLuint textures;

			// 调用stb_image生成纹理
			glGenTextures(1, &textures);
			load_texture_STBImage(this->m_my_meshes_[i]->get_texture_file2(j), textures);

			// 将生成的纹理传给shader
			glBindTexture(GL_TEXTURE_2D, textures);
			glUniform1i(glGetUniformLocation(this->program_all[index], "texture"), 0);
			this->textures_all.push_back(textures);
			index++;
		}
		
	}
};

void Mesh_Painter::draw_meshes2_material()
{
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			// 指定使用渲染器，不同的模型可以指定不同的渲染器
			// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
			glUseProgram(this->program_all[index]);
			
			#ifdef __APPLE__
				glBindVertexArrayAPPLE(this->vao_all[index]);
			#else
				glBindVertexArray(this->vao_all[index]);
			#endif

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);// 该语句必须，否则将只使用同一个纹理进行绘制

			float x0,y0,z0,x1, y1, z1, x2, y2, z2;
			this->m_my_meshes_[i]->get_scale(x0,y0,z0);
			this->m_my_meshes_[i]->get_translate(x1, y1, z1);
			this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
			
			modelMatrix = mat4(1.0) * Scale(x0+scalex,y0+scalex,z0+scalex) * Translate(x1+translatex, y1+translatey, z1+translatez) * RotateZ(z2) * RotateY(y2) * RotateX(x2) ;	// 模型变换矩阵
			mat4 mm = modelMatrix;
			glUniformMatrix4fv(modelMatrix_all[index], 1, GL_TRUE, modelMatrix);

			viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
			glUniformMatrix4fv(modelView_all[index], 1, GL_TRUE, viewMatrix);

			projMatrix = perspective(fov,aspect,near,far);
			glUniformMatrix4fv(projection_all[index], 1, GL_TRUE, projMatrix);

			
			int fnum = this->m_my_meshes_[i]->fnum[j+1];
			if(fnum==3)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 3);
			if(fnum==4)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 6);
			
				
			
			if(this->light_reflex)
			{
				GLint LightPosition = glGetUniformLocation(this->program_all[index], "LightPosition");
				// 将阴影矩阵传入点着色器
				// 根据材质反射率计算光源三个分量
				vec4 ambient_product = light_ambient * this->m_my_meshes_[i]->Kd[j];
				vec4 diffuse_product = light_diffuse * this->m_my_meshes_[i]->Ka[j];
				vec4 specular_product = light_specular * this->m_my_meshes_[i]->Ks[j];
				
				// 光源位置、材质高光系数、光源三种分量传入点着色器和片元着色器
				glUniform4fv(LightPosition, 1, light_position );
				glUniform1f(Shininess_all[index], this->m_my_meshes_[i]->Ns[j]);
				glUniform4fv(AmbientProduct_all[index], 1, ambient_product );
				glUniform4fv(DiffuseProduct_all[index],  1, diffuse_product );
				glUniform4fv(SpecularProduct_all[index],  1, specular_product );
			}
			index++;
			glUseProgram(0);
		}
	}
};

void Mesh_Painter::draw_speedup()
{
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		if(!speedpass[i])
			continue;
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			// 指定使用渲染器，不同的模型可以指定不同的渲染器
			// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
			glUseProgram(this->program_all[index]);
			
			#ifdef __APPLE__
				glBindVertexArrayAPPLE(this->vao_all[index]);
			#else
				glBindVertexArray(this->vao_all[index]);
			#endif

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);// 该语句必须，否则将只使用同一个纹理进行绘制

			float x0,y0,z0,x1, y1, z1, x2, y2, z2;
			this->m_my_meshes_[i]->get_scale(x0,y0,z0);
			this->m_my_meshes_[i]->get_translate(x1, y1, z1);
			this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
			
			modelMatrix = mat4(1.0) * Scale(x0+scalex,y0+scalex,z0+scalex) * Translate(x1+translatex, y1+translatey, z1+translatez) * RotateZ(z2) * RotateY(y2) * RotateX(x2) ;	// 模型变换矩阵
			mat4 mm = modelMatrix;
			glUniformMatrix4fv(modelMatrix_all[index], 1, GL_TRUE, modelMatrix);

			viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
			glUniformMatrix4fv(modelView_all[index], 1, GL_TRUE, viewMatrix);

			projMatrix = perspective(fov,aspect,near,far);
			glUniformMatrix4fv(projection_all[index], 1, GL_TRUE, projMatrix);

			
			int fnum = this->m_my_meshes_[i]->fnum[j+1];
			if(fnum==3)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 3);
			if(fnum==4)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 6);
			
				
			
			if(this->light_reflex)
			{
				GLint LightPosition = glGetUniformLocation(this->program_all[index], "LightPosition");
				// 将阴影矩阵传入点着色器
				// 根据材质反射率计算光源三个分量
				vec4 ambient_product = light_ambient * this->m_my_meshes_[i]->Kd[j];
				vec4 diffuse_product = light_diffuse * this->m_my_meshes_[i]->Ka[j];
				vec4 specular_product = light_specular * this->m_my_meshes_[i]->Ks[j];
				
				// 光源位置、材质高光系数、光源三种分量传入点着色器和片元着色器
				glUniform4fv(LightPosition, 1, light_position );
				glUniform1f(Shininess_all[index], this->m_my_meshes_[i]->Ns[j]);
				glUniform4fv(AmbientProduct_all[index], 1, ambient_product );
				glUniform4fv(DiffuseProduct_all[index],  1, diffuse_product );
				glUniform4fv(SpecularProduct_all[index],  1, specular_product );
			}
			index++;
			glUseProgram(0);
		}
	}
};

void Mesh_Painter::draw_speedup_shadow()
{
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		if(!speedpass[i])
			continue;
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			// 指定使用渲染器，不同的模型可以指定不同的渲染器
			// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
			glUseProgram(this->program_all[index]);
			
			#ifdef __APPLE__
				glBindVertexArrayAPPLE(this->vao_all[index]);
			#else
				glBindVertexArray(this->vao_all[index]);
			#endif

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);// 该语句必须，否则将只使用同一个纹理进行绘制

			float x0,y0,z0,x1, y1, z1, x2, y2, z2;
			this->m_my_meshes_[i]->get_scale(x0,y0,z0);
			this->m_my_meshes_[i]->get_translate(x1, y1, z1);
			this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
			
			modelMatrix = mat4(1.0) * Scale(x0+scalex,y0+scalex,z0+scalex) * Translate(x1+translatex, y1+translatey, z1+translatez) * RotateZ(z2) * RotateY(y2) * RotateX(x2) ;	// 模型变换矩阵
			mat4 mm = Camera::getShadowMatrix() * modelMatrix;
			glUniformMatrix4fv(modelMatrix_all[index], 1, GL_TRUE, mm);

			viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
			glUniformMatrix4fv(modelView_all[index], 1, GL_TRUE, viewMatrix);

			projMatrix = perspective(fov,aspect,near,far);
			glUniformMatrix4fv(projection_all[index], 1, GL_TRUE, projMatrix);

			
			int fnum = this->m_my_meshes_[i]->fnum[j+1];
			if(fnum==3)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 3);
			if(fnum==4)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 6);
			
				
			
			if(this->light_reflex)
			{
				GLint LightPosition = glGetUniformLocation(this->program_all[index], "LightPosition");
				// 将阴影矩阵传入点着色器
				// 根据材质反射率计算光源三个分量
				vec4 ambient_product = light_ambient * this->m_my_meshes_[i]->Kd[j];
				vec4 diffuse_product = light_diffuse * this->m_my_meshes_[i]->Ka[j];
				vec4 specular_product = light_specular * this->m_my_meshes_[i]->Ks[j];
				
				// 光源位置、材质高光系数、光源三种分量传入点着色器和片元着色器
				glUniform4fv(LightPosition, 1, light_position );
				glUniform1f(Shininess_all[index], this->m_my_meshes_[i]->Ns[j]);
				glUniform4fv(AmbientProduct_all[index], 1, ambient_product );
				glUniform4fv(DiffuseProduct_all[index],  1, diffuse_product );
				glUniform4fv(SpecularProduct_all[index],  1, specular_product );
			}
			index++;
			glUseProgram(0);
		}
	}
};

void Mesh_Painter::draw_meshes2_shadow()
{
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			// 指定使用渲染器，不同的模型可以指定不同的渲染器
			// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
			glUseProgram(this->program_all[index]);
			
			#ifdef __APPLE__
				glBindVertexArrayAPPLE(this->vao_all[index]);
			#else
				glBindVertexArray(this->vao_all[index]);
			#endif

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);// 该语句必须，否则将只使用同一个纹理进行绘制

			float x0,y0,z0,x1, y1, z1, x2, y2, z2;
			this->m_my_meshes_[i]->get_scale(x0,y0,z0);
			this->m_my_meshes_[i]->get_translate(x1, y1, z1);
			this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
			
			modelMatrix = mat4(1.0) * Scale(x0,y0,z0) * Translate(x1, y1, z1) * RotateZ(z2) * RotateY(y2) * RotateX(x2) ;	// 模型变换矩阵
			mat4 mm = Camera::getShadowMatrix() * modelMatrix;
			glUniformMatrix4fv(modelMatrix_all[index], 1, GL_TRUE, mm);

			viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
			glUniformMatrix4fv(modelView_all[index], 1, GL_TRUE, viewMatrix);

			projMatrix = perspective(fov,aspect,near,far);
			glUniformMatrix4fv(projection_all[index], 1, GL_TRUE, projMatrix);

			
			int fnum = this->m_my_meshes_[i]->fnum[j+1];
			if(fnum==3)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 3);
			if(fnum==4)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 6);
			
				
			
			if(this->light_reflex)
			{
				GLint LightPosition = glGetUniformLocation(this->program_all[index], "LightPosition");
				// 将阴影矩阵传入点着色器
				// 根据材质反射率计算光源三个分量
				vec4 ambient_product = light_ambient * this->m_my_meshes_[i]->Kd[j];
				vec4 diffuse_product = light_diffuse * this->m_my_meshes_[i]->Ka[j];
				vec4 specular_product = light_specular * this->m_my_meshes_[i]->Ks[j];
				
				// 光源位置、材质高光系数、光源三种分量传入点着色器和片元着色器
				glUniform4fv(LightPosition, 1, light_position );
				glUniform1f(Shininess_all[index], this->m_my_meshes_[i]->Ns[j]);
				glUniform4fv(AmbientProduct_all[index], 1, ambient_product );
				glUniform4fv(DiffuseProduct_all[index],  1, diffuse_product );
				glUniform4fv(SpecularProduct_all[index],  1, specular_product );
			}
			index++;
			glUseProgram(0);
		}
	}
};

void Mesh_Painter::draw_meshestest()
{
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			// 指定使用渲染器，不同的模型可以指定不同的渲染器
			// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
			glUseProgram(this->program_all[index]);
			
			#ifdef __APPLE__
				glBindVertexArrayAPPLE(this->vao_all[index]);
			#else
				glBindVertexArray(this->vao_all[index]);
			#endif

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);// 该语句必须，否则将只使用同一个纹理进行绘制

			float x0,y0,z0,x1, y1, z1, x2, y2, z2;
			this->m_my_meshes_[i]->get_scale(x0,y0,z0);
			this->m_my_meshes_[i]->get_translate(x1, y1, z1);
			this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
			
			modelMatrix = mat4(1.0) * Scale(scalex,scaley,scalez) * Translate(translatex, translatey, translatez) * RotateZ(z2) * RotateY(y2) * RotateX(x2) ;	// 模型变换矩阵
			mat4 mm = modelMatrix;
			glUniformMatrix4fv(modelMatrix_all[index], 1, GL_TRUE, modelMatrix);

			viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
			glUniformMatrix4fv(modelView_all[index], 1, GL_TRUE, viewMatrix);

			projMatrix = perspective(fov,aspect,near,far);
			glUniformMatrix4fv(projection_all[index], 1, GL_TRUE, projMatrix);

			
			int fnum = this->m_my_meshes_[i]->fnum[j+1];
			if(fnum==3)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 3);
			if(fnum==4)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 6);
			
				
			
			if(this->light_reflex)
			{
				GLint LightPosition = glGetUniformLocation(this->program_all[index], "LightPosition");
				// 将阴影矩阵传入点着色器
				// 根据材质反射率计算光源三个分量
				vec4 ambient_product = light_ambient * table_ambient;
				vec4 diffuse_product = light_diffuse * table_diffuse;
				vec4 specular_product = light_specular * table_specular;
				
				// 光源位置、材质高光系数、光源三种分量传入点着色器和片元着色器
				glUniform4fv(LightPosition, 1, light_position );
				glUniform1f(Shininess_all[index], table_shininess);
				glUniform4fv(AmbientProduct_all[index], 1, ambient_product );
				glUniform4fv(DiffuseProduct_all[index],  1, diffuse_product );
				glUniform4fv(SpecularProduct_all[index],  1, specular_product );
			}
			index++;
			glUseProgram(0);
		}
	}
};

void Mesh_Painter::draw_meshescar()
{
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			// 指定使用渲染器，不同的模型可以指定不同的渲染器
			// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
			glUseProgram(this->program_all[index]);
			
			#ifdef __APPLE__
				glBindVertexArrayAPPLE(this->vao_all[index]);
			#else
				glBindVertexArray(this->vao_all[index]);
			#endif

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);// 该语句必须，否则将只使用同一个纹理进行绘制

			float x0,y0,z0,x1, y1, z1, x2, y2, z2;
			this->m_my_meshes_[i]->get_scale(x0,y0,z0);
			this->m_my_meshes_[i]->get_translate(x1, y1, z1);
			this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
			
			modelMatrix = Translate(positionx_car+car_movex, positiony_car, positionz_car+car_movez) * Scale(x0,y0,z0) * RotateZ(z2) * RotateY(carturn_right) * RotateX(x2) ;	// 模型变换矩阵
			mat4 mm = modelMatrix;
			glUniformMatrix4fv(modelMatrix_all[index], 1, GL_TRUE, modelMatrix);

			viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
			glUniformMatrix4fv(modelView_all[index], 1, GL_TRUE, viewMatrix);

			projMatrix = perspective(fov,aspect,near,far);
			glUniformMatrix4fv(projection_all[index], 1, GL_TRUE, projMatrix);

			
			int fnum = this->m_my_meshes_[i]->fnum[j+1];
			if(fnum==3)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 3);
			if(fnum==4)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 6);
			
				
			
			if(this->light_reflex)
			{
				GLint LightPosition = glGetUniformLocation(this->program_all[index], "LightPosition");
				// 将阴影矩阵传入点着色器
				// 根据材质反射率计算光源三个分量
				vec4 ambient_product = light_ambient * table_ambient;
				vec4 diffuse_product = light_diffuse * table_diffuse;
				vec4 specular_product = light_specular * table_specular;
				
				// 光源位置、材质高光系数、光源三种分量传入点着色器和片元着色器
				glUniform4fv(LightPosition, 1, light_position );
				glUniform1f(Shininess_all[index], table_shininess);
				glUniform4fv(AmbientProduct_all[index], 1, ambient_product );
				glUniform4fv(DiffuseProduct_all[index],  1, diffuse_product );
				glUniform4fv(SpecularProduct_all[index],  1, specular_product );
			}
			index++;
			glUseProgram(0);
		}
	}
};

void Mesh_Painter::draw_carshadow2()
{
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			// 指定使用渲染器，不同的模型可以指定不同的渲染器
			// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
			glUseProgram(this->program_all[index]);
			
			#ifdef __APPLE__
				glBindVertexArrayAPPLE(this->vao_all[index]);
			#else
				glBindVertexArray(this->vao_all[index]);
			#endif

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, this->textures_all[index]);// 该语句必须，否则将只使用同一个纹理进行绘制

			float x0,y0,z0,x1, y1, z1, x2, y2, z2;
			this->m_my_meshes_[i]->get_scale(x0,y0,z0);
			this->m_my_meshes_[i]->get_translate(x1, y1, z1);
			this->m_my_meshes_[i]->get_rotate(x2, y2, z2);
			
			modelMatrix = Translate(positionx_car+car_movex, positiony_car, positionz_car+car_movez) * Scale(x0,y0,z0) * RotateZ(z2) * RotateY(carturn_right) * RotateX(x2) ;	// 模型变换矩阵
			mat4 mm = Camera::getShadowMatrix() * modelMatrix;
			glUniformMatrix4fv(modelMatrix_all[index], 1, GL_TRUE, mm);

			viewMatrix = lookAt(eye, at, up);	// 调用 Camera::lookAt 函数计算视图变换矩阵
			glUniformMatrix4fv(modelView_all[index], 1, GL_TRUE, viewMatrix);

			projMatrix = perspective(fov,aspect,near,far);
			glUniformMatrix4fv(projection_all[index], 1, GL_TRUE, projMatrix);

			
			int fnum = this->m_my_meshes_[i]->fnum[j+1];
			if(fnum==3)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 3);
			if(fnum==4)
				glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->mtlindex[j+1] * 6);
			
				
			
			if(this->light_reflex)
			{
				GLint LightPosition = glGetUniformLocation(this->program_all[index], "LightPosition");
				// 将阴影矩阵传入点着色器
				// 根据材质反射率计算光源三个分量
				vec4 ambient_product = light_ambient * table_ambient;
				vec4 diffuse_product = light_diffuse * table_diffuse;
				vec4 specular_product = light_specular * table_specular;
				
				// 光源位置、材质高光系数、光源三种分量传入点着色器和片元着色器
				glUniform4fv(LightPosition, 1, light_position );
				glUniform1f(Shininess_all[index], table_shininess);
				glUniform4fv(AmbientProduct_all[index], 1, ambient_product );
				glUniform4fv(DiffuseProduct_all[index],  1, diffuse_product );
				glUniform4fv(SpecularProduct_all[index],  1, specular_product );
			}
			index++;
			glUseProgram(0);
		}
	}
};


void Mesh_Painter::init_shaders2(std::string vs, std::string fs)
{
	this->program_all.clear();
	this->modelMatrix_all.clear();
	this->modelView_all.clear();
	this->projection_all.clear();
	if(this->light_reflex)
	{
		this->AmbientProduct_all.clear();
		this->DiffuseProduct_all.clear();
		this->SpecularProduct_all.clear();
		this->Shininess_all.clear();
	}
	
	int index=0;
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		for(int j=0; j<this->m_my_meshes_[i]->mtlname.size(); j++)
		{
			GLuint program = InitShader(vs.c_str(), fs.c_str());
			this->program_all.push_back(program);

			GLuint 	 modelMatrixID = glGetUniformLocation(program, "ModelMatrix");
			modelMatrix_all.push_back(modelMatrixID);

			GLuint 	ModelViewID = glGetUniformLocation(program, "ModelView");
			modelView_all.push_back(ModelViewID);

			GLuint 	ProjectionID = glGetUniformLocation(program, "Projection");
			projection_all.push_back(ProjectionID);

			
			if(this->light_reflex)
			{
				GLuint 	AmbientProductID = glGetUniformLocation(program, "AmbientProduct");
				AmbientProduct_all.push_back(AmbientProductID);

				GLuint 	DiffuseProductID = glGetUniformLocation(program, "DiffuseProduct");
				DiffuseProduct_all.push_back(DiffuseProductID);

				GLuint 	SpecularProductID = glGetUniformLocation(program, "SpecularProduct");
				SpecularProduct_all.push_back(SpecularProductID);

				GLuint 	ShininessID = glGetUniformLocation(program, "Shininess");
				Shininess_all.push_back(ShininessID);
			}
			index++;
		}
	}

};


void Mesh_Painter::init_shaders(std::string vs, std::string fs)
{
	this->program_all.clear();
	this->modelMatrix_all.clear();
	this->modelView_all.clear();
	this->projection_all.clear();
	if(this->light_reflex)
	{
		this->AmbientProduct_all.clear();
		this->DiffuseProduct_all.clear();
		this->SpecularProduct_all.clear();
		this->Shininess_all.clear();
	}
	
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint program = InitShader(vs.c_str(), fs.c_str());
		this->program_all.push_back(program);

		GLuint 	 modelMatrixID = glGetUniformLocation(program, "ModelMatrix");
		modelMatrix_all.push_back(modelMatrixID);

		GLuint 	ModelViewID = glGetUniformLocation(program, "ModelView");
		modelView_all.push_back(ModelViewID);

		GLuint 	ProjectionID = glGetUniformLocation(program, "Projection");
		projection_all.push_back(ProjectionID);

		
		if(this->light_reflex)
		{
			GLuint 	AmbientProductID = glGetUniformLocation(program, "AmbientProduct");
			AmbientProduct_all.push_back(AmbientProductID);

			GLuint 	DiffuseProductID = glGetUniformLocation(program, "DiffuseProduct");
			DiffuseProduct_all.push_back(DiffuseProductID);

			GLuint 	SpecularProductID = glGetUniformLocation(program, "SpecularProduct");
			SpecularProduct_all.push_back(SpecularProductID);

			GLuint 	ShininessID = glGetUniformLocation(program, "Shininess");
			Shininess_all.push_back(ShininessID);
		}
		
	}

};

void Mesh_Painter::set_light(bool x)
{
	this->light_reflex = x;
}

bool Mesh_Painter::get_light()
{
	return this->light_reflex;
}

void Mesh_Painter::add_mesh(My_Mesh* m)
{
	this->m_my_meshes_.push_back(m);
};

void Mesh_Painter::clear_mehs()
{
	this->m_my_meshes_.clear();

	this->textures_all.clear();
	this->program_all.clear();
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();
	this->modelMatrix_all.clear();
	this->modelView_all.clear();
	this->projection_all.clear();
	if(this->light_reflex)
	{
		this->AmbientProduct_all.clear();
		this->DiffuseProduct_all.clear();
		this->SpecularProduct_all.clear();
		this->Shininess_all.clear();
	}
};
