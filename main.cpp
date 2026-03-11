#include <iostream>

#include "Application.h"


int main()
{
	Application app(1280, 720);

	if (!app.Initialize())
	{
		std::cerr << "Failed to initialize application" << std::endl;
		return -1;
	}

	std::cout << "Application initialized successfully" << std::endl;
	app.Run();


	return 0;

}
