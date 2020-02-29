#pragma   once 

#include "Angel.h"
#include "robot.h"
using namespace Robot;
#include <vec.h>

namespace Camera
{
    // 相机位置参数
    extern int viewtype;
    extern float radius;
    extern float rotateAngle;
    extern float upAngle;
    extern vec4 eye;
    extern vec4 at;
    extern vec4 up;

    // 投影参数
    #undef near
    #undef far
    extern float near;
    extern float far;

    // 透视投影参数
    extern float fov;
    extern float aspect;

    // 光源
	extern float light_radius;
	extern float light_rotateAngle;
	extern float light_origin_rotateAngle;
	extern float light_upAngle;
	extern float light_origin_upAngle;

	extern vec4 light_position;
	extern vec4 light_ambient;
	extern vec4 light_diffuse;
	extern vec4 light_specular;

    // 正交投影参数
    extern float scale;

    extern mat4 modelMatrix;
    extern mat4 viewMatrix;
    extern mat4 projMatrix;

    

	mat4 lookAt(vec4& eye, vec4& at,vec4& up );

	mat4 ortho( const GLfloat left, const GLfloat right,
		const GLfloat bottom, const GLfloat top,
		const GLfloat zNear, const GLfloat zFar );

	mat4 perspective( const GLfloat fovy, const GLfloat aspect,
		const GLfloat zNear, const GLfloat zFar);

	mat4 frustum( const GLfloat left, const GLfloat right,
	      const GLfloat bottom, const GLfloat top,
	      const GLfloat zNear, const GLfloat zFar );
        
    mat4 getShadowMatrix();
}