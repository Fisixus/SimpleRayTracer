
#include "ReadInputFile.h"

using namespace Angel;
using namespace std;

void writeOutputPixels();
int sphereIntersectionControl(Object3D &obj3D, Ray &ray);
color3 trace(Ray &ray);

typedef vec3 point3;
typedef vec3 color3;

const color3 background_color = {0.4, 0.4, 0.4};
enum { No_InterSection = 0, InterSection = 1};
int status = No_InterSection;

typedef struct {
	point3 origin;
	point3 destination;
	vec3 direction;
}Ray;

color3** outputPixels;
ReadInputFile rif;
string inputFilename = "test1.in";
int columns;
int rows;
vector<LightSource> lightSources;
vector<Pigment> pigments;
vector<Surface> surfaces;
vector<Object3D> object3Ds;
Camera camera;
float eps = 0.001; //for numerical issues
bool intersectionInInside = false;
point3 sphereIntersectionPoint;
vec3 sphereIntersectionNormal;

int main(int argc, char **argv) {
	initiliazeSceneValues();
	outputPixels = new vec3*[rows];
	for (int i = 0; i < rows; i++) {
		outputPixels[i] = new vec3[columns];
	}

	point3 cZ = -normalize(camera.at - camera.eye);
	point3 cX = normalize(cross(camera.up, cZ));
	point3 cY = cross(cZ,cX);
	float h = 2 * tan((camera.fovY / 2) * DegreesToRadians);
	float w = h * (columns / (rows*1.0f));
	for(int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			float pX = w * (j / (columns*1.0)) - (w / 2); // px (in camera coordinates)
			float pY = -h * (i / (rows*1.0)) + (h / 2); // py (in camera coordinates)
			point3 pixel = camera.eye + pX * cX + pY * cY + -cZ; //P(i,j) in world coordinates
			Ray ray;
			ray.origin = camera.eye;
			ray.direction = normalize(pixel - ray.origin);
			ray.destination = pixel;
			outputPixels[i][j] = trace(ray);
		}
	}
	writeOutputPixels();
	return 0;
}

void initiliazeSceneValues() {
	rif.readInputandFillSceneElements(inputFilename);
	columns = rif.getWidth();
	rows = rif.getHeight();
	camera = rif.getCamera();
	lightSources = rif.getLightSources();
	surfaces = rif.getSurfaces();
	pigments = rif.getPigments();
	object3Ds = rif.getObject3Ds();
}


color3 trace(Ray &ray) {
	color3 localC;
	point3 p;
	vec3 normal;


	for (auto &obj3D : object3Ds) // access by reference to avoid copying
	{
		//check intersection for each object
		status = sphereIntersectionControl(obj3D, ray);
		if (status == No_InterSection)
			return background_color;
	}

	//normal = compute_normal(p);
	localC = (0, 0, 0);
	for (auto &lightSource : lightSources) // access by reference to avoid copying
	{
		//if (visible(p, lightSource)) {  // check shadow ray
			//localC = phong(lightSource, p, normal);
		
		//}
	}
	return localC;
}

int sphereIntersectionControl(Object3D &obj3D, Ray &ray) {
	float radius = obj3D.radius;
	string type = obj3D.type;
	point3 center = obj3D.center;

	point3 u = obj3D.center - ray.origin;
	float a = dot(ray.direction, ray.direction);
	float b = -2 * dot(ray.direction, u);
	float c = dot(u, u) - pow(radius, 2);
	//4(r^2-|u-(d.u)d|^2)
	float n = dot(u - dot(ray.direction, u)* ray.direction, u - dot(ray.direction, u)* ray.direction);
	float discriminant = 4*(pow(radius,2) - n );

	if (discriminant < 0) { //no intersection
		return No_InterSection;
	}

	else { //intersection

		float firstRoot = dot(ray.destination, u) + n;
		float secondRoot = dot(ray.destination, u) - n;

		if (firstRoot < 0 && secondRoot < 0) {
			return No_InterSection;
		}

		else if (firstRoot > 0 && secondRoot < 0) {
			intersectionInInside = true;
			sphereIntersectionPoint = ray.origin + firstRoot * ray.direction;
			sphereIntersectionNormal = -normalize(sphereIntersectionPoint - obj3D.center);
		}

		else if (firstRoot < 0 && secondRoot > 0) {
			intersectionInInside = true;
			sphereIntersectionPoint = ray.origin + secondRoot * ray.direction;
			sphereIntersectionNormal = -normalize(sphereIntersectionPoint - obj3D.center);
		}

		else if (firstRoot > 0 && secondRoot > 0) {
			if (firstRoot > secondRoot) {
				sphereIntersectionPoint = ray.origin + secondRoot * ray.direction;
				sphereIntersectionNormal = normalize(sphereIntersectionPoint - obj3D.center);
			}
			else {
				sphereIntersectionPoint = ray.origin + firstRoot * ray.direction;
				sphereIntersectionNormal = normalize(sphereIntersectionPoint - obj3D.center);
			}
		}
		return InterSection;
	}
	
}

void writeOutputPixels() {

}
