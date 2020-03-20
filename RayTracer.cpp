
#include "ReadInputFile.h"

using namespace Angel;
using namespace std;

typedef vec3 point3;
typedef vec3 color3;

int main(int argc, char **argv) {
	string x = "test1.in";
	ReadInputFile rif;
	rif.readInputandFillSceneElements(x);
	printf("Hello\n");
}