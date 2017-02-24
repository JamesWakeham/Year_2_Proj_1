#include "Application3D.h"

int main() {
	
	auto app = new Application3D();
	//app->run("AIE", 340, 240, false);
	app->run("AIE", 1600, 900, false);
	delete app;

	return 0;
}