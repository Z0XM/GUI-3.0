#include "src/Renderer.hpp"

int main()
{
	Renderer renderer;
	while (renderer.isRunning()) {
		renderer.update();
		renderer.render();
	}
}