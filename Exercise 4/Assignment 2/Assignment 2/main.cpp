#include "Window.h"
#include "Scene.h"

int main(void)
{
	Graphics::Window window(1024, 768, "Exercise 4 - Bradley Katz");
	Graphics::Scene *scene = Graphics::Scene::LoadScene("city.obj");

	window.LoadScene(scene);
	window.run();
}