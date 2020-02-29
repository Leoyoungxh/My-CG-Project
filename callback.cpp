#include "callback.h"

int width = 500;
int height = 500;
int status = 1;
float walkdis = 0;
float scalex = 0;
float scaley = 0;
float scalez = 0;
float translatex = 0;
float translatey = 0;
float translatez = 0;

float t=0;
float dt=1;

float carspeed = 0.07;
float speedpo[13][2]={{-3,3},{-7,5},{-12,2.5},{-13,-2},
						{-12,-8},{-8,-12},{0,-13},{8,-10},
						{12,-6},{15,0},{15,6},{15,12},{15,18}};
bool speedpass[13]={true,true,true,true,true,true,true,
					true,true,true,true,true,true};

bool sipky[4] = {false, false, false , false};

void keyup(int key , int x , int y)//for glutSpecialUpFunc
{
	switch(key)
	{
		case GLUT_KEY_UP :
			sipky[0]=false;
			break;
		case GLUT_KEY_DOWN :
			sipky[1]=false;
			break;
		case GLUT_KEY_LEFT :
			sipky[2]=false;
			break;
		case GLUT_KEY_RIGHT :
			sipky[3]=false;
			break;
		default: break;
	}
	CheckKeys();
}

void special(int key , int x , int y)//for glutSpecialFunc
{
	switch(key)
	{
		case GLUT_KEY_UP :
			sipky[0]=true;
			break;
		case GLUT_KEY_DOWN :
			sipky[1]=true;
			break;
		case GLUT_KEY_LEFT :
			sipky[2]=true;
			break;
		case GLUT_KEY_RIGHT :
			sipky[3]=true;
			break;
		default: break;
	}
	CheckKeys();
}
void CheckPosition()
{
	for(int i=0; i<13; i++)
	{
		if((tx>speedpo[i][0]-1 && tx<speedpo[i][0]+1) && (tz>speedpo[i][1]-1 && tz<speedpo[i][1]+1) && speedpass[i])
		{
			speedpass[i] = false;
			carspeed += 0.02;
			std::cout<<"speed up "<<std::endl;
		}
	}
	
}

void CheckKeys()
{
	if(viewtype==1)
	{
		if(sipky[0])
		{
			walk(1);
			robot_movex -= dt*0.03*sin(robotturn_right*M_PI/180);
			robot_movez -= dt*0.03*cos(robotturn_right*M_PI/180);
		}
		if(sipky[1])
		{
			walk(1);
			robot_movex += dt*0.03*sin(robotturn_right*M_PI/180);
			robot_movez += dt*0.03*cos(robotturn_right*M_PI/180);
		}
		if(sipky[2])
		{
			robotturn_right += 2;
			if(robotturn_right>=360)
				robotturn_right == 0;
			theta[0] = robotturn_right;
		}
		if(sipky[3])
		{
			robotturn_right -= 2;
			if(robotturn_right<0)
				robotturn_right = 359;
			theta[0] = robotturn_right;
		}
	}
	else if(viewtype==2)
	{
		if(sipky[0])
		{
			walk(1);
			robot_movex -= 0.03*sin(robotturn_right*M_PI/180);
			robot_movez -= 0.03*cos(robotturn_right*M_PI/180);
		}
		if(sipky[1])
		{
			walk(1);
			robot_movex += 0.03*sin(robotturn_right*M_PI/180);
			robot_movez += 0.03*cos(robotturn_right*M_PI/180);
		}
		if(sipky[2])
		{
			robotturn_right += 1;
			if(robotturn_right>=360)
				robotturn_right == 0;
			theta[0] = robotturn_right;
		}
		if(sipky[3])
		{
			robotturn_right -= 1;
			if(robotturn_right<0)
				robotturn_right = 359;
			theta[0] = robotturn_right;
		}
	}
	else if(viewtype==3)
	{
		if(sipky[0])
		{
			car_movex -= carspeed*cos(carturn_right*M_PI/180);
			car_movez += carspeed*sin(carturn_right*M_PI/180);
		}
		if(sipky[1])
		{
			car_movex += carspeed*cos(carturn_right*M_PI/180);
			car_movez -= carspeed*sin(carturn_right*M_PI/180);
		}
		if(sipky[2])
		{
			carturn_right += 0.8;
			if(carturn_right>=360)
				carturn_right == 0;
			theta[0] = carturn_right;
		}
		if(sipky[3])
		{
			carturn_right -= 0.8;
			if(carturn_right<0)
				carturn_right = 359;
			theta[0] = carturn_right;
		}
		tx = positionx_car + car_movex;
		tz = positionz_car + car_movez;
	}
	
	//glutPostRedisplay();
}

void reshape(GLsizei w, GLsizei h)
{
	width = w;
	height = h;
	glViewport(0, 0, width, height);
}

void print_help()
{
	std::cout<<"Keyboard Usage"<<std::endl;
	std::cout<<
	"Q/ESC:		Exit"<<std::endl<<
	"h:			Print help message"<<std::endl<<
	"SPACE:		Reset all parameters"<<std::endl<<
	"x/X:		Increase/Decrease the rotate angle"<<std::endl<<
	"y/Y:		Increase/Decrease the up angle"<<std::endl<<
	"r/R:		Increase/Decrease the distance between camera and object"<<std::endl<<
	"f/F:		Increase/Decrease FOV of perspective projection"<<std::endl<<
	"a/A:		Increase/Decrease width/height aspect of perspective projection"<<std::endl<<
	"s/S:		Increase/Decrease the extent of orthogonal projection"<<std::endl;
}

void mouse(int button, int state, int x, int y )
{
	float x1;
	float y1;
	if(x<0)
		x=0;
	if(y<0)
		y=0;
	if(x>500)
		x=500;
	if(y>250)
		y=250;
	// 实现鼠标坐标的缩放和平移，对应合适的变化
	x1 = float(x) * (180/float(width)) - 90;
	y1 = float(y) * (-180/float(height)) + 90;

	light_rotateAngle = light_origin_rotateAngle + x1;
	light_upAngle = light_origin_upAngle + y1;
	//当阴影会出错的地方保持原来的位置
	// if(light.upAngle >= 90)
	// 	light.upAngle = 90;
	// if(light.rotateAngle-rotateAngle > 130)
	// 	light.rotateAngle = light.rotateAngle - x1;
	// if(light.rotateAngle-rotateAngle < -130)
	// 	camera.rotateAngle = camera.rotateAngle - x1;
	// std::cout << "Light's rotateAngle:" << light.rotateAngle << ",Light's upAngle:" << light.upAngle <<  std::endl;
	// std::cout << "Camera's rotateAngle:" << camera.rotateAngle << ",Camera's upAngle:" << camera.upAngle <<  std::endl
	// 		<< std::endl;
		
}

// 滚轮响应函数
void mouseWheel(int button, int dir, int x, int y)
{
    if(dir>0)
		radius += 0.1;
	else
	{
		radius -= 0.1;
	}
}

// 键盘响应函数
void keyboard(unsigned char key, int x, int y)
{
	switch(key) 
	{
	// ESC键 和 'q' 键退出游戏
	case 033: exit(EXIT_SUCCESS); break;
	case 'q': exit (EXIT_SUCCESS); break;
	case 'h': print_help(); break;

	// 通过按键改变相机和投影的参数
	//case 'x': rotateAngle += 5.0; break;
	//case 'X': rotateAngle -= 5.0; break;

	case 'y': upAngle += 5.0; if(upAngle > 90) upAngle=90; break;
	case 'Y': upAngle -= 5.0; if(upAngle < -90) upAngle=-90;break;

	case 'r': radius += 0.1; break;
	case 'R': radius -= 0.1; break;

	case 'g': fov += 5.0; break;
	case 'G': fov -= 5.0; break;

	case 'f': 
	{
		if(viewtype==2)
			viewtype = 3;
		else if(viewtype==3)
			viewtype = 2;
		break;
	}

	// 操控机器人或小车
	case 'w': 
	{
		upAngle += 5.0; if(upAngle >= 90) upAngle=89; break;
		break;
	}
	case 's': 
	{
		upAngle -= 5.0; if(upAngle <= -90) upAngle=-89;break;
		break;
	}
	case 'a':
	{
		rotateAngle -= 5.0;
		break;
	}
	case 'd':
	{
		rotateAngle += 5.0;
		break;
	}
	

	case 'z': near += 1; break;
	case 'Z': near -= 1; break;
	case 'x': far += 1; break;
	case 'X': far -= 1; break;
	case 'c': scalez += 0.01; break;
	case 'C': scalez -= 0.01; break;

	case 'v': translatex += 5; break;
	case 'V': translatex -= 5; break;
	case 'b': translatey += 5; break;
	case 'B': translatey -= 5; break;
	case 'n': translatez += 5; break;
	case 'N': translatez -= 5; break;


	//case 't': scale += 0.1; break;
	//case 'T': scale -= 0.1; break;
	// case 'w': // 相机前移
	// 	light_upAngle += 5.0; if(light_upAngle >= 90) light_upAngle=89; break;
	// case 'a': // 相机后退
	// 	light_upAngle -= 5.0; if(light_upAngle <= -90) light_upAngle=-89;break;
	// case 's': // 相机左移
	// 	light_rotateAngle -= 5.0;
	// 	break;
	// case 'd': // 相机右移
	// 	light_rotateAngle += 5.0;
	// 	break;

	// 空格键初始化所有参数
	case ' ': 
		radius = 4.0; rotateAngle = 0.0; upAngle = 0.0;
		fov = 45.0; aspect = 1.0; scale = 1.5;
		break;
	case 'o':
		break;
	case '1':
		viewtype = 1;
		break;
	case '2':
		viewtype = 2;
		break;
	case '3':
	 	viewtype = 3;
		break;
	}
	//std::cout<<"car: "<<carturn_right<<std::endl;
	//std::cout<<"scx: "<<car_movex<<"  sz: "<<car_movez<<std::endl;
	//std::cout<<"scalex: "<<scalex<<"  scaley: "<<scaley<<"  scalez: "<<scalez<<std::endl;
	std::cout<<"near: "<<near<<"  far: "<<far<<std::endl;;
	glutPostRedisplay();
}
/*
void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP: // 相机前移
		upAngle += 5.0; if(upAngle >= 90) upAngle=89; break;
	case GLUT_KEY_DOWN: // 相机后退
		upAngle -= 5.0; if(upAngle <= -90) upAngle=-89;break;
	case GLUT_KEY_LEFT: // 相机左移
		rotateAngle -= 5.0;
		break;
	case GLUT_KEY_RIGHT: // 相机右移
		rotateAngle += 5.0;
		break;
	
	}
	std::cout<<rotateAngle<<std::endl;
	glutPostRedisplay();
}
*/
void idle(void)
{
	float t_new;
	t_new = glutGet(GLUT_ELAPSED_TIME);
	dt = t_new - t;
	t = t_new;
	CheckPosition();
	CheckKeys();
	glutPostRedisplay();
}

void walk(float step)
{
	
    if(walkdis>45 || walkdis<-45){
        status = -status;
		if(walkdis>45)
			walkdis=45;
		if(walkdis<-45)
			walkdis=-45;
    }
    
    if(status>0){
        theta[2] += 5.0*step;
        if (theta[2] > 360)
            theta[2] -= 360.0;
        theta[4] -= 5.0*step;
        if (theta[4] < 0.0)
            theta[4] += 360.0;
        theta[6] -= 5.0*step;
        if (theta[6] < 0.0)
            theta[6] += 360.0;
        theta[8] += 5.0*step;
        if (theta[8] > 360)
            theta[8] -= 360.0;
		walkdis += 5.0*step;
    }
    else{
        theta[2] -= 5.0*step;
        if (theta[2] < 0.0)
            theta[2] += 360.0;
        theta[4] += 5.0*step;
        if (theta[4] > 360)
            theta[4] -= 360.0;
        theta[6] += 5.0*step;
        if (theta[6] > 360)
            theta[6] -= 360.0;
        theta[8] -= 5.0*step;
        if (theta[8] < 0.0)
            theta[8] += 360.0;
		walkdis -= 5.0*step;
    }
	
    //std::cout<<theta[8]<<std::endl;
    
}
