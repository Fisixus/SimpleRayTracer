
#include "ReadInputFile.h"

using namespace Angel;
using namespace std;
#define n1 0.3
#define EPS 0.001 //for numerical issues
typedef vec3 point3;
typedef vec3 color3;
const color3 background_color = color3(0.5, 0.5, 0.5);
enum { No_InterSection = 0, InterSection = 1 };
int status = No_InterSection;

typedef struct {
	point3 origin;
	point3 destination;
	vec3 direction;
}Ray;

void initiliazeSceneValues();
void writePixelsToOutputfile();
int sphereIntersectionControl(Object3D &obj3D, Ray &ray, int counter);
color3 trace(Ray &ray, int depth);
Ray reflect(Ray &ray);
Ray transmit(Ray &ray, float n2);
bool isVisible(LightSource &lightSource);
color3 objectAmbientColor();
color3 phongModel(LightSource &lightSource);
void findIntersectedRoots(Object3D &obj3D, Ray &ray);


color3** outputPixels;
ReadInputFile rif;
string inputFilename;
string outputFilename;
int columns;
int rows;
vector<LightSource> lightSources;
vector<Pigment> pigments;
vector<Surface> surfaces;
vector<Object3D> object3Ds;
Camera camera;
point3 sphereIntersectionPoint;
vec3 sphereIntersectionNormal;
int whichObjectNum = 0;
vector<float> visibleRoots;

int main(int argc, char **argv) {
	if (argc <= 1)
	{
		// On some operating systems, argv[0] can end up as an empty string instead of the program's name.
		// We'll conditionalize our response on whether argv[0] is empty or not.
		if (argv[0])
			std::cout << "Usage: " << argv[0] << " <number>" << '\n';
		else
			std::cout << "Usage: <program name> <number>" << '\n';

		std::exit(1);
	}
	inputFilename.assign(argv[1]);

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
			outputPixels[i][j] = trace(ray, 0);
		}
	}
	writePixelsToOutputfile();
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
	outputFilename = rif.getOutputFileName();
}

color3 trace(Ray &ray, int depth) {
	color3 localC, reflectedC, transmittedC;
	int counter = 0;
	if (depth > 4) return background_color;
	for (auto &obj3D : object3Ds) // access by reference to avoid copying
	{
		//check intersection for each object
		status = sphereIntersectionControl(obj3D, ray, counter);
		if (status == InterSection) {
			break;
		}
		if(++counter == object3Ds.size())
			return background_color;
	}

	localC = (0, 0, 0);
	localC = objectAmbientColor();
	counter = 0;
	for (auto &lightSource : lightSources) // access by reference to avoid copying
	{
		if (counter++ == 0) continue; //First lightsource is for ambience
		if (isVisible(lightSource)) {  // check shadow ray
			localC += phongModel(lightSource);
		}
	}
	Surface surface = surfaces[object3Ds[whichObjectNum].numSurface];
	if (surface.kr > 0) { //reflective surface
		Ray rr = reflect(ray);
		reflectedC = trace(rr, depth + 1);
	}
	if (surface.kt > 0) {
		Ray rt = transmit(ray, surface.n2);
		transmittedC = trace(rt, depth + 1);
	}
	return localC + surface.kr*reflectedC + surface.kt*transmittedC;
}

Ray transmit(Ray &ray, float n2) {
	vec3 v = -normalize(ray.direction);
	float cosQ1 = dot(v, sphereIntersectionNormal);
	vec3 u1 = cosQ1 * sphereIntersectionNormal;
	vec3 w1 = u1 - v;
	vec3 w2 = (n1 / n2)* w1;
	
	float cosQ2 = sqrtf(1 - (pow(n1 / n2, 2)*(1 - pow(dot(v, sphereIntersectionNormal), 2))));
	vec3 u2 = -cosQ2 * sphereIntersectionNormal;
	vec3 t = w2 + u2;
	ray.origin = sphereIntersectionPoint;
	ray.direction = normalize(t);
	return ray;
}

Ray reflect(Ray &ray) {
	vec3 v = -normalize(ray.direction);
	ray.origin = sphereIntersectionPoint;
	ray.direction = normalize(2 * (dot(sphereIntersectionNormal, v)*sphereIntersectionNormal) - v);
	return ray;
}

color3 phongModel(LightSource &lightSource) {
	float d = sqrt(dot(lightSource.pos - sphereIntersectionPoint, lightSource.pos - sphereIntersectionPoint));
	float attenuation = lightSource.a + d * lightSource.b + d * d*lightSource.c;

	vec3 l = normalize(lightSource.pos - sphereIntersectionPoint);
	vec3 v = normalize(camera.eye - sphereIntersectionPoint);
	vec3 h = normalize(l + v);

	Pigment pigment = pigments[object3Ds[whichObjectNum].numPigment];
	color3 pigmentColor = color3(pigment.r, pigment.g, pigment.b);
	color3 Id = (double)dot(l, sphereIntersectionNormal) > 0.0 ? (double)dot(l, sphereIntersectionNormal) : 0.0;

	Surface surface = surfaces[object3Ds[whichObjectNum].numSurface];
	color3 Is = pow((double)dot(sphereIntersectionNormal, h) > 0.0 ? (double)dot(sphereIntersectionNormal, h) : 0.0, surface.shininess);

	color3 lightIntensity = color3(lightSource.Ir, lightSource.Ig, lightSource.Ib);
	color3 I = (lightIntensity / attenuation) * (Id*surface.kd*pigmentColor + Is * surface.ks);
	return I;
}

color3 objectAmbientColor() {
	color3 ambientIntensity = color3(lightSources[0].Ir, lightSources[0].Ig, lightSources[0].Ib);
	Surface surface = surfaces[object3Ds[whichObjectNum].numSurface];
	color3 Ia = ambientIntensity * surface.ka;
	Pigment pigment = pigments[object3Ds[whichObjectNum].numPigment];
	color3 pigmentColor = color3(pigment.r, pigment.g, pigment.b);
	return Ia * pigmentColor;
}

bool isVisible(LightSource &lightSource) {
	visibleRoots.clear();
	Ray ray;
	ray.origin = lightSource.pos;
	ray.direction = normalize(sphereIntersectionPoint - ray.origin);
	ray.destination = sphereIntersectionPoint;

	for (auto &obj3D : object3Ds) // access by reference to avoid copying
	{
		findIntersectedRoots(obj3D, ray);
	}

	float minIntersectRoot = FLT_MAX;
	for (int i = 0; i < visibleRoots.size(); i++) {
		if (visibleRoots[i] >= 0 && visibleRoots[i] <= minIntersectRoot) {
			minIntersectRoot = visibleRoots[i];
		}
	}
	point3 firstIntersectedPoint = ray.origin + minIntersectRoot * ray.direction;
	if (minIntersectRoot > 0 
		&& abs((firstIntersectedPoint.x - sphereIntersectionPoint.x)) < EPS
		&& abs((firstIntersectedPoint.y - sphereIntersectionPoint.y)) < EPS
		&& abs((firstIntersectedPoint.z - sphereIntersectionPoint.z)) < EPS) {
		return true;
	}
	return false;
}

void findIntersectedRoots(Object3D &obj3D, Ray &ray) {
	float radius = obj3D.radius;
	string type = obj3D.type;
	point3 center = obj3D.center;

	point3 u = obj3D.center - ray.origin;
	float a = dot(ray.direction, ray.direction);
	float b = -2 * dot(ray.direction, u);
	float c = dot(u, u) - pow(radius, 2);
	//4(r^2-|u-(d.u)d|^2)
	float n = dot(u - dot(ray.direction, u)* ray.direction, u - dot(ray.direction, u)* ray.direction);
	float discriminant = 4 * (pow(radius, 2) - n);
	if (discriminant < EPS) { //no intersection
		return ;
	}

	else { //intersection

		float firstRoot = (-b - sqrt(discriminant)) / (2.0*a);
		float secondRoot = (-b + sqrt(discriminant)) / (2.0*a);

		if (firstRoot < EPS && secondRoot < EPS) {
			return;
		}

		else if (firstRoot > EPS && secondRoot < EPS) {
			//Intersection inside
			visibleRoots.push_back(firstRoot);
		}

		else if (firstRoot < EPS && secondRoot > EPS) {
			//Intersection inside
			visibleRoots.push_back(secondRoot);
		}

		else if (firstRoot > EPS && secondRoot > EPS) {
			visibleRoots.push_back(secondRoot);
			visibleRoots.push_back(firstRoot);
		}
		return ;
	}
}

int sphereIntersectionControl(Object3D &obj3D, Ray &ray, int counter) {
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

	if (discriminant < EPS) { //no intersection
		whichObjectNum = -1;
		return No_InterSection;
	}

	else { //intersection

		float firstRoot = (-b - sqrt(discriminant)) / (2.0*a);
		float secondRoot = (-b + sqrt(discriminant)) / (2.0*a);

		if (firstRoot < EPS && secondRoot < EPS) {
			whichObjectNum = -1;
			return No_InterSection;
		}

		else if (firstRoot > EPS && secondRoot < EPS) {
			//Intersection inside
			sphereIntersectionPoint = ray.origin + firstRoot * ray.direction;
			sphereIntersectionNormal = -normalize(sphereIntersectionPoint - obj3D.center);
		}

		else if (firstRoot < EPS && secondRoot > EPS) {
			//Intersection inside
			sphereIntersectionPoint = ray.origin + secondRoot * ray.direction;
			sphereIntersectionNormal = -normalize(sphereIntersectionPoint - obj3D.center);
		}

		else if (firstRoot > EPS && secondRoot > EPS) {
			if (firstRoot > secondRoot) {
				sphereIntersectionPoint = ray.origin + secondRoot * ray.direction;
				sphereIntersectionNormal = normalize(sphereIntersectionPoint - obj3D.center);
			}
			else {
				sphereIntersectionPoint = ray.origin + firstRoot * ray.direction;
				sphereIntersectionNormal = normalize(sphereIntersectionPoint - obj3D.center);
			}
		}
		whichObjectNum = counter;
		return InterSection;
	}
	
}

void writePixelsToOutputfile() {
	FILE *fp = fopen(outputFilename.c_str(), "wb"); //binary
	fprintf(fp, "P6\n%d %d\n255\n", rows, columns);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < columns; j++) {
			unsigned char color[3];
			//red
			if ((outputPixels[i][j].x >= 1.0)) {
				color[0] = 255;
			} 
			else if (outputPixels[i][j].x <= 0.0) {
				color[0] = 0;
			}
			else {
				color[0] = (int)floor(outputPixels[i][j].x  * 256.0);
			}
			//blue
			if ((outputPixels[i][j].y >= 1.0)) {
				color[1] = 255;
			}
			else if (outputPixels[i][j].y <= 0.0) {
				color[1] = 0;
			}
			else {
				color[1] = (int)floor(outputPixels[i][j].y  * 256.0);
			}
			//green
			if ((outputPixels[i][j].z >= 1.0)) {
				color[2] = 255;
			}
			else if (outputPixels[i][j].z <= 0.0) {
				color[2] = 0;
			}
			else {
				color[2] = (int)floor(outputPixels[i][j].z  * 256.0);
			}

			fwrite(color, 1, 3, fp);
		}
	}
	fclose(fp);
}
