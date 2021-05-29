// By Jillian Empey

#ifndef HORSEBODYEXT_H
#define HORSEBODYEXT_H
// FOR THE MYHORSE.OBJ FILE

const int CHEST = 30;
const int BARREL = 17;
const int BOOTY = 29;
const int TAIL = 6;
const int HEAD = 26;

#define BASE vector<int>{CHEST, BARREL, BOOTY, TAIL, HEAD}

// LEFT
const int HOOF_LF = 1;
const int PASTERN_LF = 28;
const int ANKLE_LF = 27;
const int LOWLEG_LF = 2;
const int KNEE_LF = 0;
const int UPLEG_LF = 3;

const int HOOF_LB = 23;
const int PASTERN_LB = 18;
const int ANKLE_LB = 24;
const int LOWLEG_LB = 20;
const int KNEE_LB = 21;
const int UPLEG_LB = 25;
const int STIFLE_LB = 19;

#define LEFTBACKLEG vector<int>{HOOF_LB, PASTERN_LB, ANKLE_LB, LOWLEG_LB, KNEE_LB, UPLEG_LB, STIFLE_LB}
#define LEFTFRONTLEG vector<int>{HOOF_LF, PASTERN_LF, ANKLE_LF, LOWLEG_LF, KNEE_LF, UPLEG_LF}
// general front leg indices:
const int HOOF = 0;
const int PASTERN = 1;
const int ANKLE = 2;
const int LOWLEG = 3;
const int KNEE = 4;
const int UPLEG = 5;

// general back leg indices:
const int STIFLE = 6;

// RIGHT
const int HOOF_RF = 22;
const int PASTERN_RF = 5;
const int ANKLE_RF = 4;
const int LOWLEG_RF = 14;
const int KNEE_RF = 15;
const int UPLEG_RF = 16;

const int HOOF_RB = 12;
const int PASTERN_RB = 13;
const int ANKLE_RB = 10;
const int LOWLEG_RB = 7;
const int KNEE_RB = 8;
const int UPLEG_RB = 11;
const int STIFLE_RB = 9;


#define RIGHTBACKLEG vector<int>{HOOF_RB, PASTERN_RB, ANKLE_RB, LOWLEG_RB, KNEE_RB, UPLEG_RB, STIFLE_RB}
#define RIGHTFRONTLEG vector<int>{HOOF_RF, PASTERN_RF, ANKLE_RF, LOWLEG_RF, KNEE_RF, UPLEG_RF}


using namespace std;
using namespace glm;

struct BaseFrame {
	float head_angle, chest_angle, barrel_angle, rear_angle, tail_angle;
	BaseFrame(float h, float c, float b, float r, float t) {
		head_angle = radians(h);
		chest_angle = radians(c);
		barrel_angle = radians(b);
		rear_angle = radians(r);
		tail_angle = radians(t);
	}
};

struct Frame {
	float upper_angle;
	float knee_angle;
	float ankle_angle;
	float hoof_angle;
	float stifle_angle;

	Frame(float a, float b, float c, float d, float e) {
		stifle_angle = radians(a);
		upper_angle = radians(b);
		knee_angle = radians(c);
		ankle_angle = radians(d);
		hoof_angle = radians(e);
	}
};
std::vector<Frame> frontFrames{
	Frame(0.0f,         0.0f,			 0.0f,		 0.0f,		0.0f),
	Frame(0.0f,        20.0f,			10.0f,		 0.0f,		10.0f),
	Frame(0.0f,        30.0f,		    30.0f,		50.0f,		20.0f),
	Frame(0.0f,         0.0f,			90.0f,		50.0f,		30.0f),
	Frame(0.0f,       -45.0f,		   115.0f,		60.0f,		20.0f),
	Frame(0.0f,       -60.0f,			80.0f,		70.0f,		50.0f),
	Frame(0.0f,       -65.0f,			10.0f,		50.0f,		30.0f),
	Frame(0.0f,		  -45.0f,			10.0f,		10.0f,		10.0f)
};



std::vector<Frame> backFrames{
	Frame(20.0f,		-0.0f,			 0.0f,		10.0f,		 0.0f),
	Frame(25.0f,		-0.0f,			 5.0f,		20.0f,		0.0f),
	Frame(30.0f,		-0.0f,			 7.0f,		50.0f,		0.0f),
	Frame(10.0f,		-0.0f,		     0.0f,		70.0f,		0.0f),
	Frame(-10.0f,		-0.0f,		   -10.0f,		90.0f,		0.0f),
	Frame(-12.0f,		-10.0f,		   -25.0f,		80.0f,		0.0f),
	Frame(-10.0f,		-15.0f,		   -30.0f,		30.0f,		0.0f),
	Frame(-0.0f,		-10.0f,			 -.0f,		-5.0f,		 .0f)
};

//		 head_angle,   chest_angle, barrel_angle, rear_angle, tail_angle;

std::vector<BaseFrame> baseFrames{
	
	BaseFrame(5,		   3,			3,			 3,			 35),
	BaseFrame(2.5,		   2,			2,			 2,			 33),
	BaseFrame(0,		   1,			1,			 1,			 30),
	BaseFrame(2.5,		   2,			2,			 2,			 33),
	BaseFrame(5,		   3,			3,			 3,			 35),
	BaseFrame(7,		   4,			4,			 4,			 37),
	BaseFrame(10,		   5,			5,			 5,			 40),
	BaseFrame(7,		   4,			4,			 4,			 37)
};




#endif
