#include "robot.h"


namespace Robot
{
    // 机器人参数
    float TORSO_HEIGHT = 0.26;
    float TORSO_WIDTH = 0.22;
    float TORSO_LENGTH = 0.11;
    float UPPER_ARM_HEIGHT = 0.08;
    float LOWER_ARM_HEIGHT = 0.18;
    float UPPER_ARM_WIDTH  = 0.09;
    float LOWER_ARM_WIDTH  = 0.09;
    float UPPER_LEG_HEIGHT = 0.20;
    float LOWER_LEG_HEIGHT = 0.08;
    float UPPER_LEG_WIDTH  = 0.11;
    float LOWER_LEG_WIDTH  = 0.11;
    float HEAD_HEIGHT = 0.13;
    float HEAD_WIDTH = 0.15;

    GLfloat theta[10] = {
    0.0,    // Torso 0
    0.0,    // Head 1
    0.0,    // RightUpperArm 2
    0.0,    // RightLowerArm 3
    0.0,    // LeftUpperArm 4
    0.0,    // LeftLowerArm 5
    0.0,    // RightUpperLeg 6
    0.0,    // RightLowerLeg 7
    0.0,    // LeftUpperLeg 8
    0.0     // LeftLowerLeg 9
    };

    GLfloat robot_movex = 0;
    GLfloat robot_movey = 0;
    GLfloat robot_movez = 0;

    GLfloat positionx_robot = 0;
    GLfloat positiony_robot = TORSO_HEIGHT;
    GLfloat positionz_robot = 0;

    GLfloat car_movex = 0;
    GLfloat car_movey = 0;
    GLfloat car_movez = 0;

    GLfloat positionx_car = 2.5;
    GLfloat positiony_car = 0.2;
    GLfloat positionz_car = -2.5;

    GLfloat tx = 2.5;
    GLfloat ty = 0.2;
    GLfloat tz = -2.5;

    // 往右转的角度
    float robotturn_right = 0;
    float carturn_right = 45;

}