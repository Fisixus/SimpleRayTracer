#ifndef READ_INPUT_FILES

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h> 
#include <string>
#include "SceneElements.h"

class ReadInputFile {
	private:
		string outputFilename;
		int width;
		int height;
		int totalNumofLightSources;
		int totalNumofPigments;
		int totalNumofSurfaces;
		int totalNumofObj3Ds;
		Camera camera;
		vector<LightSource> lightSources;
		vector<Pigment> pigments;
		vector<Surface> surfaces;
		vector<Object3D> object3Ds;
	public:
		void readInputandFillSceneElements(string &inputFilename);
		const string getOutputFileName(void);
		const int getWidth(void);
		const int getHeight(void);
		const Camera &getCamera(void);
		const vector<LightSource> &getLightSources(void);
		const vector<Pigment> &getPigments(void);
		const vector<Surface> &getSurfaces(void);
		const vector<Object3D> &getObject3Ds(void);
};
#endif