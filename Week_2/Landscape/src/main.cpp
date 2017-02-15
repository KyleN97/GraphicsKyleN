#include "LandscapeApp.h"

int main() {
	
	auto app = new LandscapeApp();
	app->run("AIE", 1280, 720, false);
	delete app;

	return 0;
}