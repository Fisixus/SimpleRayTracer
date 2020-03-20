
#include "ReadInputFile.h"

void ReadInputFile::readInputandFillSceneElements(string &inputFilename) {
	ifstream inputFile(inputFilename);
	string line;
	int counter = 0;
	if (inputFile.is_open()) {
		while (getline(inputFile, line)) {

			if (counter == 0) {
				outputFilename.assign(line);
				counter++;
			}

			else if (counter == 1) {
				stringstream ss(line);
				ss >> width;
				ss >> height;
				counter++;
			}

			else if (counter == 2) {
				vec3 eye, at, up;
				stringstream ss(line);
				ss >> eye.x >> eye.y >> eye.z;
				getline(inputFile, line);
				ss = stringstream(line);
				ss >> at.x >> at.y >> at.z;
				getline(inputFile, line);
				ss = stringstream(line);
				ss >> up.x >> up.y >> up.z;
				getline(inputFile, line);
				ss = stringstream(line);
				float fovY;
				ss >> fovY;
				camera.eye = eye;
				camera.at = at;
				camera.up = up;
				camera.fovY = fovY;
				counter++;
			}

			else if (counter == 3) {
				stringstream ss(line);
				ss >> totalNumofLightSources;
				counter++;
			}

			else if (counter == 4) {
				for (int i = 0; i < totalNumofLightSources; i++) {
					stringstream ss(line);
					LightSource lightSource;
					float posX, posY, posZ, Ir, Ig, Ib, a, b, c;
					ss >> posX >> posY >> posZ >> Ir >> Ig >> Ib >> a >> b >> c;
					lightSource.pos = { posX,posY,posZ };
					lightSource.Ir = Ir;
					lightSource.Ig = Ig;
					lightSource.Ib = Ib;
					lightSource.a = a;
					lightSource.b = b;
					lightSource.c = c;
					lightSources.push_back(lightSource);
					if (i == totalNumofLightSources - 1)
						break;
					getline(inputFile, line);
				}
				counter++;
			}

			else if (counter == 5) {
				stringstream ss(line);
				ss >> totalNumofPigments;
				counter++;
			}

			else if (counter == 6) {
				for (int i = 0; i < totalNumofPigments; i++) {
					std::stringstream ss(line);
					Pigment pigment;
					string type;
					float r, g, b;
					ss >> type;
					ss >> r >> g >> b;
					pigment.type = type;
					pigment.r = r;
					pigment.g = g;
					pigment.b = b;
					pigments.push_back(pigment);
					if (i == totalNumofPigments - 1)
						break;
					getline(inputFile, line);
				}
				counter++;
			}

			else if (counter == 7) {
				stringstream ss(line);
				ss >> totalNumofSurfaces;
				counter++;
			}

			else if (counter == 8) {
				for (int i = 0; i < totalNumofSurfaces; i++) {
					std::stringstream ss(line);
					Surface surface;
					float ka, kd, ks, shineness, kr;
					ss >> ka >> kd >> ks >> shineness >> kr;
					surface.ka = ka;
					surface.kd = kd;
					surface.ks = ks;
					surface.kr = kr;
					surface.shininess = shineness;
					surfaces.push_back(surface);
					if (i == totalNumofSurfaces - 1)
						break;
					getline(inputFile, line);
				}
				counter++;
			}

			else if (counter == 9) {
			stringstream ss(line);
			ss >> totalNumofObj3Ds;
			counter++;
			}

			else if (counter == 10) {
				for (int i = 0; i < totalNumofObj3Ds; i++) {
					std::stringstream ss(line);
					Object3D object3D;
					string type;
					int numPigment, numSurface;
					float posX, posY, posZ, radius;
					ss >> numPigment >> numSurface >> type >> posX >> posY >> posZ >> radius;
					object3D.center = { posX, posY, posZ };
					object3D.numPigment = numPigment;
					object3D.numSurface = numSurface;
					object3D.radius = radius;
					object3D.type = type;
					object3Ds.push_back(object3D);
					if (i == totalNumofObj3Ds - 1)
						break;
					getline(inputFile, line);
				}
				counter++;
			}


		}
		inputFile.close();

	}

}

const string ReadInputFile::getOutputFileName() {
	return outputFilename;
}

const int ReadInputFile::getWidth() {
	return width;
}

const int ReadInputFile::getHeight() {
	return height;
}

const Camera &ReadInputFile::getCamera() {
	return this->camera;
}

const vector<LightSource> &ReadInputFile::getLightSources() {
	return lightSources;
}
const vector<Surface> &ReadInputFile::getSurfaces() {
	return surfaces;
}

const vector<Pigment> &ReadInputFile::getPigments() {
	return pigments;
}

const vector<Object3D> &ReadInputFile::getObject3Ds() {
	return object3Ds;
}