#include "precomp.h" // include (only) this in every .cpp file

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
	printf("Numpad1 and Numpad2 to change FOV\n");
	printf("R to reset camera\n");
	printf("C to print camera configuration\n");
	printf("T to load teddy\n");
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
	this->createScene();

	// https://groups.csail.mit.edu/graphics/classes/6.837/F03/models/
	Material* brownMaterial = new Material(vec4(1, 0.8, 0.5, 0), diffuse);
	scene->loadObjModel("assets/cube.obj", brownMaterial);
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

	for (int i = 0; i < SCRHEIGHT / 32; i++)
	{
		jobManager->AddJob2(rayTracerJobs[i]);
	}
	jobManager->RunJobs();

	// measure FPS
	char buffer[15];
	sprintf(buffer, "FPS: %f", 1000 / _timer.elapsed());
	screen->Print(buffer, 2, 2, 0x000000);
}

void Game::handleInput()
{
	// move the camera
	bool cameraChanged = false;

	if (GetAsyncKeyState(VK_DOWN))
	{
		scene->camera->position -= scene->camera->up * 0.2;
		scene->camera->viewDirection -= scene->camera->up * 0.2;		
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		scene->camera->position += scene->camera->right * 0.2;
		scene->camera->viewDirection += scene->camera->right * 0.2;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		scene->camera->position += scene->camera->up * 0.2;
		scene->camera->viewDirection += scene->camera->up * 0.2;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		scene->camera->position -= scene->camera->right * 0.2;
		scene->camera->viewDirection -= scene->camera->right * 0.2;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_OEM_PLUS))
	{
		scene->camera->position += scene->camera->viewDirectionNormalized * 0.2;
		scene->camera->viewDirection += scene->camera->viewDirectionNormalized * 0.2;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_OEM_MINUS))
	{
		scene->camera->position -= scene->camera->viewDirectionNormalized * 0.2;
		scene->camera->viewDirection -= scene->camera->viewDirectionNormalized * 0.2;
		cameraChanged = true;
	}

	// rotate the camera
	if (GetAsyncKeyState('A'))
	{
		scene->camera->viewDirection -= scene->camera->right * 0.2;
		cameraChanged = true;
	}
	if (GetAsyncKeyState('D'))
	{
		scene->camera->viewDirection += scene->camera->right * 0.2;
		cameraChanged = true;
	}
	if (GetAsyncKeyState('W'))
	{
		scene->camera->viewDirection += scene->camera->up * 0.2;
		cameraChanged = true;
	}
	if (GetAsyncKeyState('S'))
	{
		scene->camera->viewDirection -= scene->camera->up * 0.2;
		cameraChanged = true;
	}

	// change FOV
	if (GetAsyncKeyState(VK_NUMPAD1))
	{
		scene->camera->fieldOfView += 0.01;
		cameraChanged = true;
	}
	if (GetAsyncKeyState(VK_NUMPAD2))
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
		scene->camera = new Camera();
	}

	// load teddy
	if (GetAsyncKeyState('T'))
	{
		this->loadTeddy();
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

void Game::createScene()
{
	// create scene lights
	scene->addLightSource(new DirectLight(vec3(-1.0f, 0.0f, -3.0), vec4(1, 1, 1, 0), 20));
	scene->addLightSource(new DirectLight(vec3(0.0f, -2.0f, 0.0f), vec4(0.5, 0.5, 0.5, 0), 20));

	// create scene objects
	Material* redMaterial = new Material(vec4(1, 0, 0, 0), diffuse);
	Material* greenMaterial = new Material(vec4(0, 1, 0, 0), diffuse);
	Material* brownMaterial = new Material(vec4(0.756, 0.556, 0.094, 0), diffuse);
	Material* planeMaterial = new Material(vec4(0.75, 0.8, 0.7, 1), diffuse);
	Material* mirrorMaterial = new Material(vec4(0.75, 0.8, 0.7, 1), mirror);

	Material* blueGlassMaterial = new Material(vec4(0, 0, 1, 0.8), dielectric);
	blueGlassMaterial->refraction = 1.33;
	blueGlassMaterial->reflection = 0.1;

	//simple scene
	scene->addPrimitive(
		new Triangle(greenMaterial, vec3(4, 4, 4), vec3(1, 1, 4), vec3(2, 5, 4))
	);
	scene->addPrimitive(
		new Sphere(redMaterial, vec3(5, -1, 3), 0.5)
	);
	scene->addPrimitive(
		new Sphere(brownMaterial, vec3(-2, -1, 2), 0.5)
	);

	/*scene->primitives.push_back(
		new Plane(planeMaterial, scene->primitives.size(), vec3(0, 0, 5), vec3(0, 0, -1)) // back
	);*/
	//

	// complex scene
	//*
	scene->addPrimitive(
		new Triangle(greenMaterial, vec3(4, 4, 4), vec3(1, 1, 4), vec3(2, 5, 4))
	);
	scene->addPrimitive(
		new Sphere(blueGlassMaterial, vec3(2, -1, 2), 0.5) // spehere in the box
	);

	scene->addPrimitive(
		new Cylinder(redMaterial, vec3(-1.5, -1.5, 0), vec3(1, 0, 0), 0.1, 0.5)
	);

	scene->addPrimitive(
		new Torus(brownMaterial, 0.4f, 0.2f, vec3(-0.5, -0.5, 0.2), vec3(0.5, 0.5, -1))
	);

	// create box from planes
	/*scene->addPrimitive(
		new Plane(planeMaterial, vec3(0, 0, 5), vec3(0, 0, -1)) // back
	);
	scene->addPrimitive(
		new Plane(mirrorMaterial, vec3(0, -5, 5), vec3(0, 1, 0)) //top
	);
	scene->addPrimitive(
		new Plane(mirrorMaterial, vec3(0, 5, 5), vec3(0, -1, 0)) //bottom
	);
	scene->addPrimitive(
		new Plane(greenMaterial, vec3(-5, 0, 5), vec3(1, 0, 0)) //right
	);
	scene->addPrimitive(
		new Plane(greenMaterial, vec3(5, 0, 5), vec3(-1, 0, 0)) // left
	);//*/
}

void Game::loadTeddy()
{
	scene->clear();

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

	scene->camera->position = vec3(-32, 0, 40);
	scene->camera->up = vec3(0, 1, 0);
	scene->camera->right = vec3(-0.77, 0, -0.62);
	scene->camera->calculateScreen();

	Material* brownMaterial = new Material(vec4(1, 0.8, 0.5, 0), diffuse);
	scene->loadObjModel("assets/teddy.obj", brownMaterial);
}