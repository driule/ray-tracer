#include "precomp.h" // include (only) this in every .cpp file

int frame, sceneId, movingModelId;
float cameraSpeed = 0.2;
timer _timer;

RayTracerJob** rayTracerJobs;
JobManager* jobManager;

Scene* scene;

void RayTracerJob::Main()
{
	for (uint i = start; i < end; i++)
	{
		scene->render(i);
	}
}

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	printf("--------------------------------------------------\n");
	printf("Application controls:\n");
	printf("Arrows, '+', '-' to move camera around\n");
	printf("WASD to rotate camera\n");
	printf("Numpad '+' and '-' to change FOV\n");
	printf("R to reset camera\n");
	printf("C to print camera configuration\n");
	printf("Numpad 1, 2, 3 to toggle between scenes\n");
	printf("--------------------------------------------------\n");

	// initialize threads
	rayTracerJobs = new RayTracerJob*[SCRHEIGHT / 32];
	for (int i = 0; i < SCRHEIGHT / 32; i++)
	{
		rayTracerJobs[i] = new RayTracerJob(i * 32, (i + 1) * 32);
	}

	JobManager::CreateJobManager(4);
	jobManager = JobManager::GetJobManager();

	//create scene
	scene = new Scene(screen);
	//this->loadTeddy();
	this->loadNiceScene();
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	_timer.reset();
	screen->Clear(0);

	this->handleInput();

	if (MULTITHREADING_ENABLED)
	{
		for (int i = 0; i < SCRHEIGHT / 32; i++)
		{
			jobManager->AddJob2(rayTracerJobs[i]);
		}
		jobManager->RunJobs();
	}
	else
	{
		for (uint i = 0; i < SCRHEIGHT; i++)
		{
			scene->render(i);
		}
	}

	// move models
	if (sceneId == 2)
	{
		if (frame <= 10)
		{
			for (int i = 0; i < 10; i++)
			{
				if (i % 2 == 0) scene->translateModel(i, vec3(0, 1, 0));
				else scene->translateModel(i, vec3(0, -1, 0));
			}
		}
		else if (frame > 10 && frame <= 20)
		{
			for (int i = 0; i < 10; i++)
			{
				if (i % 2 == 0) scene->translateModel(i, vec3(0, -1, 0));
				else scene->translateModel(i, vec3(0, 1, 0));
			}
		}

		if (frame > 20) frame = 0;
	}

	if (sceneId == 0)
	{
		if (frame <= 10)
		{
			scene->translateModel(0, vec3(1, 0, 1));
			scene->translateModel(2, vec3(-1, 0, -1));
		}
		else if (frame > 10 && frame <= 20)
		{

			scene->translateModel(0, vec3(-1, 0, -1));
			scene->translateModel(2, vec3(1, 0, 1));
		}

		if (frame > 20) frame = 0;
	}

	// calculate frame
	frame++;
	if (frame == 100)
	{
		frame = 0;
	}

	// measure FPS
	char buffer[20];
	sprintf(buffer, "FPS: %f", 1000 / _timer.elapsed());
	screen->Print(buffer, 2, 2, 0x000000);

	sprintf(buffer, "Primitives: %i", scene->getPrimitivesCount());
	screen->Print(buffer, 2, 12, 0x000000);
}

void Game::handleInput()
{
	// move the camera
	bool cameraChanged = false;

	if (GetAsyncKeyState(VK_DOWN))
	{
		scene->camera->position -= scene->camera->up * cameraSpeed;
		scene->camera->viewDirection -= scene->camera->up * cameraSpeed;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		scene->camera->position += scene->camera->right * cameraSpeed;
		scene->camera->viewDirection += scene->camera->right * cameraSpeed;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		scene->camera->position += scene->camera->up * cameraSpeed;
		scene->camera->viewDirection += scene->camera->up * cameraSpeed;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		scene->camera->position -= scene->camera->right * cameraSpeed;
		scene->camera->viewDirection -= scene->camera->right * cameraSpeed;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_OEM_PLUS))
	{
		scene->camera->position += scene->camera->viewDirectionNormalized * cameraSpeed;
		scene->camera->viewDirection += scene->camera->viewDirectionNormalized * cameraSpeed;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_OEM_MINUS))
	{
		scene->camera->position -= scene->camera->viewDirectionNormalized * cameraSpeed;
		scene->camera->viewDirection -= scene->camera->viewDirectionNormalized * cameraSpeed;
		cameraChanged = true;
	}

	// rotate the camera
	if (GetAsyncKeyState('A'))
	{
		scene->camera->viewDirection -= scene->camera->right * cameraSpeed;
		cameraChanged = true;
	}
	if (GetAsyncKeyState('D'))
	{
		scene->camera->viewDirection += scene->camera->right * cameraSpeed;
		cameraChanged = true;
	}
	if (GetAsyncKeyState('W'))
	{
		scene->camera->viewDirection += scene->camera->up * cameraSpeed;
		cameraChanged = true;
	}
	if (GetAsyncKeyState('S'))
	{
		scene->camera->viewDirection -= scene->camera->up * cameraSpeed;
		cameraChanged = true;
	}

	// change FOV
	if (GetAsyncKeyState(VK_ADD))
	{
		scene->camera->fieldOfView += 0.01;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_SUBTRACT))
	{
		scene->camera->fieldOfView -= 0.01;
		cameraChanged = true;
	}
	if (cameraChanged)
	{
		scene->camera->calculateScreen();
	}

	// reset camera
	if (GetAsyncKeyState('R'))
	{
		scene->camera->reset();
	}

	// toggle scenes
	if (GetAsyncKeyState(VK_NUMPAD1))
	{
		frame = 0;
		this->loadNiceScene();
	}
	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		frame = 0;
		this->loadTeddy();
	}
	if (GetAsyncKeyState(VK_NUMPAD3))
	{
		frame = 15;
		this->loadTeapot();
	}

	// print camera configuration
	if (GetAsyncKeyState('C'))
	{
		printf("Camera position: (%f, %f, %f) \n", scene->camera->position.x, scene->camera->position.y, scene->camera->position.z);
		printf("FOV: %f \n", scene->camera->fieldOfView);
		printf("UP: (%f, %f, %f) \n", scene->camera->up.x, scene->camera->up.y, scene->camera->up.z);
		printf("RIGHT: (%f, %f, %f) \n", scene->camera->right.x, scene->camera->right.y, scene->camera->right.z);
	}
}

void Game::loadNiceScene()
{
	// https://groups.csail.mit.edu/graphics/classes/6.837/F03/models/
	//movingModelId = scene->loadModel("assets/cube.obj", brownMaterial);

	// Reset scene
	sceneId = 0;
	cameraSpeed = 1;
	scene->clear();
	scene->camera->reset();

	scene->camera->position = vec3(0, -2.5, -40);
	scene->camera->up = vec3(0, 1, 0);
	scene->camera->right = vec3(1, 0, 0);
	scene->camera->calculateScreen();

	// create scene lights
	scene->addLightSource(new DirectLight(vec3(-10, 20, -30), vec4(1, 1, 1, 0), 1000));
	//scene->addLightSource(new DirectLight(vec3(0.0f, -2.f, 0.0f), vec4(1, 1, 1, 0), 20));

	// create scene objects
	Material* floorMaterial = new Material(vec4(0.22, 0.61, 0.86, 1), diffuse);
	Material* wall1Material = new Material(vec4(0.09, 0.63, 0.52, 1), diffuse);
	Material* wall2Material = new Material(vec4(0.3, 0.7, 0.3, 1.0), diffuse);

	Material* glassMaterial = new Material(vec4(0.93, 0.94, 0.95, 1), dielectric);
	glassMaterial->refraction = 1.33;
	glassMaterial->reflection = 0.1;

	Material* mirrorMaterial = new Material(vec4(0.75, 0.8, 0.7, 1), mirror);
	Material* orangeMaterial = new Material(vec4(0.95, 0.61, 0.07, 1), diffuse);
	Material* redMaterial = new Material(vec4(0.8, 0.21, 0.19, 1), diffuse);


	int roomWidth = 50;
	int roomHeigth = 50;

	//Add Teapot
	Material* brownMaterial = new Material(vec4(1, 0.8, 0.5, 0), diffuse);
	scene->loadModel("assets/teapot.obj", brownMaterial, vec3(-15, -10, -20));
	scene->loadModel("assets/teapot.obj", redMaterial, vec3(4, -10, -20));
	scene->loadModel("assets/teapot.obj", glassMaterial, vec3(15, -10, -20));

	//Floor primitives
	scene->addPrimitive(new Triangle(floorMaterial, vec3(-roomWidth, -10, -100), vec3(-roomWidth, -10, 10), vec3(roomWidth, -10, 10)));
	scene->addPrimitive(new Triangle(floorMaterial, vec3(roomWidth, -10, -100), vec3(-roomWidth, -10, -100), vec3(roomWidth, -10, 10)));

	
	//Wall primitives
	scene->addPrimitive(new Triangle(wall1Material, vec3(roomWidth, -roomHeigth, 10), vec3(-roomWidth, -roomHeigth, 10), vec3(roomWidth, roomHeigth, 10)));
	scene->addPrimitive(new Triangle(wall1Material, vec3(-roomWidth, -roomHeigth, 10), vec3(-roomWidth, roomHeigth, 10), vec3(roomWidth, roomHeigth, 10)));

	scene->addPrimitive(new Triangle(wall1Material, vec3(roomWidth, -roomHeigth, -100), vec3(roomWidth, -roomHeigth, 10), vec3(roomWidth, roomHeigth, 10)));
	scene->addPrimitive(new Triangle(wall1Material, vec3(roomWidth, roomHeigth, -100), vec3(roomWidth, -roomHeigth, -100), vec3(roomWidth, roomHeigth, 10)));

	scene->addPrimitive(new Triangle(wall1Material, vec3(-roomWidth, -roomHeigth, 10), vec3(-roomWidth, -roomHeigth, -100), vec3(-roomWidth, roomHeigth, 10)));
	scene->addPrimitive(new Triangle(wall1Material, vec3(-roomWidth, -roomHeigth, -100), vec3(-roomWidth, roomHeigth, -100), vec3(-roomWidth, roomHeigth, 10)));

	scene->addPrimitive(new Triangle(wall1Material, vec3(-roomWidth, -roomHeigth, -100), vec3(roomWidth, -roomHeigth, -100), vec3(roomWidth, roomHeigth, -100)));
	scene->addPrimitive(new Triangle(wall1Material, vec3(-roomWidth, roomHeigth, -100), vec3(-roomWidth, -roomHeigth, -100), vec3(roomWidth, roomHeigth, -100)));


	//Sphere
	scene->addPrimitive(new Sphere(orangeMaterial, vec3(0, -5, -10), 5));

	//More primitives
	scene->addPrimitive(new Torus(glassMaterial, 2, 1, vec3(-3, -6, -20), vec3(0.2, -0.2, -1)));
	//scene->addPrimitive(new Cylinder(glassMaterial, vec3(-10, -20, -20), vec3(0, 1, 0), 0.5, 100));
	//scene->addPrimitive(new Cylinder(glassMaterial, vec3(-13, -20, -22), vec3(0, 1, 0), 0.5, 100));
	//scene->addPrimitive(new Cylinder(glassMaterial, vec3(-16, -20, -24), vec3(0, 1, 0), 0.5, 100));
}

void Game::loadTeddy()
{
	sceneId = 1;
	cameraSpeed = 1;
	scene->clear();

	scene->camera->reset();
	scene->camera->position = vec3(-32, 0, 40);
	scene->camera->up = vec3(0, 1, 0);
	scene->camera->right = vec3(-0.77, 0, -0.62);
	scene->camera->calculateScreen();

	scene->addLightSource(new DirectLight(vec3(-10.0f, 0.0f, 20.0), vec4(1, 1, 1, 0), 250));
	scene->addLightSource(new DirectLight(vec3(8.0f, 0.0f, 18.0), vec4(1, 1, 1, 0), 100));
	scene->addLightSource(new DirectLight(vec3(4.0f, 8.0f, 20.0), vec4(1, 1, 1, 0), 100));

	scene->addLightSource(new DirectLight(vec3(-10.0f, 0.0f, -20.0), vec4(1, 1, 1, 0), 250));
	scene->addLightSource(new DirectLight(vec3(8.0f, 0.0f, -18.0), vec4(1, 1, 1, 0), 100));
	scene->addLightSource(new DirectLight(vec3(4.0f, 8.0f, -20.0), vec4(1, 1, 1, 0), 100));

	Material* redMaterial = new Material(vec4(1, 0, 0, 0), diffuse);
	scene->addPrimitive(
		new Sphere(redMaterial, vec3(-25, 10, 0), 5)
	);

	Material* brownMaterial = new Material(vec4(1, 0.8, 0.5, 0), diffuse);
	for (int i = 0; i < 3; i++)
	{
		scene->loadModel("assets/teddy.obj", brownMaterial, vec3(i * 40, 0, 0));
	}
}

void Game::loadTeapot()
{
	sceneId = 2;
	cameraSpeed = 0.5;
	scene->clear();

	scene->camera->reset();
	scene->camera->position = vec3(-20, -0.013, 20);
	scene->camera->up = vec3(0, 1, 0);
	scene->camera->right = vec3(-0.921, 0, -0.387);
	scene->camera->calculateScreen();

	scene->addLightSource(new DirectLight(vec3(-10.0f, 0.0f, 20.0), vec4(1, 1, 1, 0), 250));
	scene->addLightSource(new DirectLight(vec3(8.0f, 0.0f, 18.0), vec4(1, 1, 1, 0), 100));
	scene->addLightSource(new DirectLight(vec3(4.0f, 8.0f, 20.0), vec4(1, 1, 1, 0), 100));

	scene->addLightSource(new DirectLight(vec3(-10.0f, 0.0f, -20.0), vec4(1, 1, 1, 0), 250));
	scene->addLightSource(new DirectLight(vec3(8.0f, 0.0f, -18.0), vec4(1, 1, 1, 0), 100));
	scene->addLightSource(new DirectLight(vec3(4.0f, 8.0f, -20.0), vec4(1, 1, 1, 0), 100));

	Material* brownMaterial = new Material(vec4(1, 0.8, 0.5, 0), diffuse);
	for (int i = 0; i < 10; i++)
	{
		scene->loadModel("assets/teapot.obj", brownMaterial, vec3(i * 7, 0, 0));
	}
}