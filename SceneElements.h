#ifndef SCENE_ELEMENTS_H
#include <iostream>
#include "Angel.h"
#include <vector>
#include <Math.h>
#include <string>
using namespace std;
typedef vec3 point3;
typedef vec3 color3;

typedef struct {
	vec3 eye;
	vec3 at;
	vec3 up;
	float fovY;
}Camera;

typedef struct {
	vec3 pos;
	float Ir;
	float Ig;
	float Ib;
	float a, b, c;
}LightSource;

typedef struct {
	float ka, kd, ks, kr, kt, n2, shininess;
}Surface;

typedef struct {
	string type;
	float r, g, b;
}Pigment;

typedef struct {
	string type;
	float radius;
	point3 center;
	int numSurface;
	int numPigment;
}Object3D;


#endif // !SCENE_ELEMENTS_H