#pragma   once 

#include "Angel.h"
#include <vec.h>

namespace Robot
{
    // 机器人参数
    extern float TORSO_HEIGHT;
    extern float TORSO_WIDTH;
    extern float TORSO_LENGTH;
    extern float UPPER_ARM_HEIGHT;
    extern float LOWER_ARM_HEIGHT;
    extern float UPPER_ARM_WIDTH;
    extern float LOWER_ARM_WIDTH;
    extern float UPPER_LEG_HEIGHT;
    extern float LOWER_LEG_HEIGHT;
    extern float UPPER_LEG_WIDTH;
    extern float LOWER_LEG_WIDTH;
    extern float HEAD_HEIGHT;
    extern float HEAD_WIDTH;

    extern GLfloat theta[10];
    extern GLfloat robot_movex;
    extern GLfloat robot_movey;
    extern GLfloat robot_movez;

    extern float robotturn_right;
    extern float carturn_right;
    extern bool is_robotview;

    extern GLfloat positionx_robot;
    extern GLfloat positiony_robot;
    extern GLfloat positionz_robot;

    extern GLfloat car_movex;
    extern GLfloat car_movey;
    extern GLfloat car_movez;

    extern GLfloat positionx_car;
    extern GLfloat positiony_car;
    extern GLfloat positionz_car;

    extern GLfloat tx;
    extern GLfloat ty;
    extern GLfloat tz;
}