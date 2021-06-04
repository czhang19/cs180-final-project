#ifndef DUMMY_H
#define DUMMY_H

const int FOOT_R = 0;
const int ANKLE_R = 1;
const int LOWLEG_R = 2;
const int KNEE_R = 3;
const int UPLEG_R = 4;
const int PELVIS_R = 5; 

#define RIGHTLEG vector<int>{FOOT_R, ANKLE_R, LOWLEG_R, KNEE_R, UPLEG_R, PELVIS_R}

const int FOOT_L = 6;
const int ANKLE_L = 7;
const int LOWLEG_L = 8;
const int KNEE_L = 9;
const int UPLEG_L = 10;
const int PELVIS_L = 11; 

#define LEFTLEG vector<int>{FOOT_L, ANKLE_L, LOWLEG_L, KNEE_L, UPLEG_L, PELVIS_L}

const int HIPS = 12;
const int BELLY = 13;
const int TORSO = 14;
const int NECK = 27;
const int HEAD_D = 28;

#define BODY vector<int>{HIPS, BELLY, TORSO, NECK, HEAD_D}

const int SHOULDER_R = 15;
const int BICEP_R = 16; 
const int ELBOW_R = 17;
const int FOREARM_R = 18;
const int WRIST_R = 19;
const int HAND_R = 20;

#define RIGHTARM vector<int>{SHOULDER_R, BICEP_R, ELBOW_R, FOREARM_R, WRIST_R, HAND_R}

const int SHOULDER_L = 21;
const int BICEP_L = 22; 
const int ELBOW_L = 23;
const int FOREARM_L = 24;
const int WRIST_L = 25;
const int HAND_L = 26;

#define LEFTARM vector<int>{SHOULDER_L, BICEP_L, ELBOW_L, FOREARM_L, WRIST_L, HAND_L}

#endif