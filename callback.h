#pragma   once 

#include "Angel.h"
#include "camera.h"
#include "robot.h"
using namespace Robot;
using namespace Camera;

extern float scalex;
extern float scaley;
extern float scalez;
extern float translatex;
extern float translatey;
extern float translatez;

extern float carspeed;
extern float speedpo[13][2];
extern bool speedpass[13];

extern float t;
extern float dt;


extern int width;
extern int height;
extern int status;
extern bool sipky[4];

extern float walkdis;

void reshape(GLsizei w, GLsizei h);

void print_help();

void mouse(int button, int state, int x, int y );

void mouseWheel(int button, int dir, int x, int y);

// 键盘响应函数
void keyboard(unsigned char key, int x, int y);
//void special(int key, int x, int y);

void idle(void);

void walk(float step);

void CheckKeys();
void special(int key , int x , int y);
void keyup(int key , int x , int y);