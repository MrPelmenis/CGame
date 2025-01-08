#ifndef MYSTRUCTS_H
#define MYSTRUCTS_H

extern float PLayerAcceleration;
extern float PlayerSpeed;
extern float FrameTime;
extern float drag;

typedef struct {
	float x;
	float y;
} PlayerCoords;

typedef struct {
	float x;
	float y;
} Movement;


//r radius dx dy speeds
typedef struct {
	float x;
	float y;
	float dx;
	float dy;
	float r;
} Ball;

#endif
