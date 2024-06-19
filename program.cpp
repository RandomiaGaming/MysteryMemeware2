#include "Renderring/EZWindows.h"

int main() {
	EZ::ClassSettings classSettings = { };
	EZ::RegisterClass(classSettings);
	EZ::WindowSettings windowSettings = { };
	EZ::Window* window = new EZ::Window(windowSettings);
	window->RunMessagePump();
	delete window;
	return 0;
}