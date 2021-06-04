// By Jillian Empey

#ifndef HORSEBODYEXT_H
#define HORSEBODYEXT_H
// FOR THE MYHORSE.OBJ FILE

const int CHEST = 30;
const int BARREL = 17;
const int BOOTY = 29;
const int TAIL = 6;
const int HEAD_H = 26;

#define BASE vector<int>{CHEST, BARREL, BOOTY, TAIL, HEAD_H}

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

/* 
jilly horse: 31 shapes, bounding boxes
0) x: 0.428465, 0.752287 y: 1.49828, 1.82211 z: 1.42143, 1.74525
1) x: 0.386931, 0.844505 y: 0.015655, 0.246412 z: 1.56785, 2.02542
2) x: 0.46098, 0.719772 y: 0.570766, 1.56412 z: 1.46688, 1.6998
3) x: 0.435352, 0.833377 y: 1.76959, 3.31033 z: 1.3389, 1.82779
4) x: -0.613853, -0.369333 y: 0.394322, 0.638841 z: 1.45581, 1.70033
5) x: -0.54227, -0.414945 y: 0.110189, 0.481736 z: 1.53674, 1.84534
6) x: -0.348152, 0.437033 y: 1.95048, 4.53343 z: -2.99046, -1.89631
7) x: -0.770122, -0.429381 y: 0.607203, 2.07572 z: -1.96691, -1.65867
8) x: -0.803096, -0.396407 y: 1.86215, 2.35235 z: -2.07851, -1.5883
9) x: -0.738573, -0.323173 y: 2.61253, 4.25247 z: -1.7448, -0.762218
10) x: -0.74772, -0.454914 y: 0.400575, 0.693381 z: -1.82742, -1.53461
11) x: -0.716499, -0.385108 y: 2.15622, 3.34119 z: -1.89887, -0.940796
12) x: -0.830491, -0.372919 y: 0.044989, 0.275746 z: -1.69533, -1.23776
13) x: -0.665961, -0.538636 y: 0.098945, 0.470492 z: -1.67962, -1.37102
14) x: -0.624066, -0.365274 y: 0.58612, 1.57947 z: 1.45624, 1.68915
15) x: -0.656581, -0.332759 y: 1.51364, 1.83746 z: 1.41078, 1.73461
16) x: -0.649694, -0.251669 y: 1.78494, 3.32569 z: 1.32825, 1.81714
17) x: -0.606788, 0.716053 y: 2.91943, 4.51227 z: -0.693465, 0.972821
18) x: 0.516224, 0.64355 y: 0.076354, 0.447901 z: -1.71039, -1.40179
19) x: 0.419221, 0.834621 y: 2.58994, 4.22987 z: -1.77557, -0.792986
20) x: 0.412063, 0.752804 y: 0.584612, 2.05313 z: -1.99768, -1.68944
21) x: 0.379089, 0.785778 y: 1.83956, 2.32976 z: -2.10927, -1.61907
22) x: -0.698115, -0.240541 y: 0.03101, 0.261766 z: 1.5572, 2.01478
23) x: 0.351694, 0.809267 y: 0.022398, 0.253155 z: -1.7261, -1.26853
24) x: 0.434465, 0.727271 y: 0.377984, 0.67079 z: -1.85819, -1.56538
25) x: 0.465686, 0.797077 y: 2.13363, 3.31859 z: -1.92964, -0.971565
26) x: -0.395775, 0.477751 y: 3.71168, 5.48148 z: 1.78529, 4.15107
27) x: 0.46557, 0.710089 y: 0.378968, 0.623487 z: 1.46645, 1.71097
28) x: 0.542775, 0.670101 y: 0.094835, 0.466382 z: 1.54738, 1.85599
29) x: -0.690502, 0.799767 y: 3.02745, 4.69412 z: -1.93169, -0.265013
30) x: -0.65127, 0.75091 y: 2.77309, 4.57865 z: 0.510688, 2.27979
*/
