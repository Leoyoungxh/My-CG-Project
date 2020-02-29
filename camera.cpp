#include "camera.h"


namespace Camera
{
	// 视角类型 1：第三 2：人 3：车
    int viewtype = 1;
    // 相机位置参数
    float radius = 2.7;
    float rotateAngle = -25.0;
    float upAngle = 15.0;
    vec4 eye(-0.5, -0.5, -0.5, 1.0);
    vec4 at(0.0, 0.0, 0.0, 1.0);
    vec4 up(0.0, 1.0, 0.0, 0.0);

    // 投影参数
    #undef near
    #undef far
    float near = 0.1;
    float far = 100.0;

    // 透视投影参数
    float fov = 45.0;
    float aspect = 1.2;

    // 正交投影参数
    float scale = 1.5;
    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 projMatrix;

	// 光源
	float light_radius = 100.0;
	float light_rotateAngle = 0;
	float light_origin_rotateAngle = 0;
	float light_upAngle = 90;
	float light_origin_upAngle = 90;

	vec4 light_position = vec4(0, 200, 0, 1.0);
	vec4 light_ambient = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 light_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 light_specular = vec4(1.0, 1.0, 1.0, 1.0);

	mat4 lookAt(vec4& eye, vec4& at, vec4& up )
	{
		
		float eyex, eyey, eyez;
		float atx, aty, atz;
		if(viewtype == 3){
			eyex = positionx_car+car_movex  + 1.2*cos(carturn_right*M_PI/180);
			eyey = positiony_car + 0.7;
			eyez = positionz_car+car_movez  - 1.2*sin(carturn_right*M_PI/180);

			atx = positionx_car+car_movex  - 5*cos(carturn_right*M_PI/180);
			aty = eyey - 2;
			atz = positionz_car+car_movez  + 5*sin(carturn_right*M_PI/180);
			
			at = vec4(atx, aty, atz, 1.0);
			
		}
		else if(viewtype == 2){
			eyex = positionx_robot+robot_movex + 0.5*sin(robotturn_right*M_PI/180);
			eyey = positiony_robot + 0.3;
			eyez = positionz_robot+robot_movez + 0.5*cos(robotturn_right*M_PI/180);

			atx = eyex - 3*sin(robotturn_right*M_PI/180);
			aty = eyey - 0.5;
			atz = eyez - 3*cos(robotturn_right*M_PI/180);
			
			at = vec4(atx, aty, atz, 1.0);
		}
		else if(viewtype == 1){
			// 根据rotateAngle和upAngle设置x
			eyex = radius*sin(rotateAngle*M_PI/180)*cos(upAngle*M_PI/180);		
			// 根据upAngle设置y
			eyey = radius*sin(upAngle*M_PI/180);		
			// 根据rotateAngle和upAngle设置z
			eyez = radius*cos(rotateAngle*M_PI/180)*cos(upAngle*M_PI/180);		
		
			at = vec4(0.0, 0.0, 0.0, 1.0);
		}
		
		eye = vec4(eyex, eyey, eyez, 1.0);
		
		return LookAt(eye, at, up);
	}

	mat4 ortho( const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar )
	{
		return Ortho(left, right, bottom, top, near, far);
	}

	mat4 perspective( const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar)
	{
		return Perspective(fov, aspect, near, far);
	}

	mat4 frustum( const GLfloat left, const GLfloat right,
	      const GLfloat bottom, const GLfloat top,
	      const GLfloat zNear, const GLfloat zFar )
	{
		// 任意视锥体矩阵
		mat4 c;
		c[0][0] = 2.0*zNear/(right - left);
		c[0][2] = (right + left)/(right - left);
		c[1][1] = 2.0*zNear/(top - bottom);
		c[1][2] = (top + bottom)/(top - bottom);
		c[2][2] = -(zFar + zNear)/(zFar - zNear);
		c[2][3] = -2.0*zFar*zNear/(zFar - zNear);
		c[3][2] = -1.0;
		c[3][3] = 0.0;
		return c;
	}

	mat4 getShadowMatrix()
	{
		// 根据rotateAngle和upAngle设置x
		float lightx = light_radius*sin(light_rotateAngle*M_PI/180)*cos(light_upAngle*M_PI/180);		
		// 根据rotateAngle和upAngle设置y
		float lighty = light_radius*sin(light_upAngle*M_PI/180);	
		// 根据rotateAngle和upAngle设置z	
		float lightz = light_radius*cos(light_rotateAngle*M_PI/180)*cos(light_upAngle*M_PI/180);	

		light_position = vec4(lightx, lighty, lightz, 1.0);
		mat4 shadowProjMatrix = mat4(1.0);	// 阴影矩阵初始化
		float y=0;
		// 计算具体的阴影矩阵
		shadowProjMatrix = mat4(vec4(-light_position.y-y, light_position.x, 0, y*light_position.x),
									vec4(0,-y,0,y*light_position.y),
									vec4(0, light_position.z, -light_position.y-y, y*light_position.z),
									vec4(0, 1, 0, -light_position.y));
		return shadowProjMatrix;
	}
}