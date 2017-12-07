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
	printf("Use arrows to move camera around....\n");

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

	this->moveCamera();

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

void Game::moveCamera()
{
	bool changed = false;
	// move the camera
	if (GetAsyncKeyState(VK_DOWN))
	{
		scene->camera->position -= scene->camera->up * 0.2;
		scene->camera->viewDirection -= scene->camera->up * 0.2;		
		changed = true;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		scene->camera->position += scene->camera->right * 0.2;
		scene->camera->viewDirection += scene->camera->right * 0.2;
		changed = true;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		scene->camera->position += scene->camera->up * 0.2;
		scene->camera->viewDirection += scene->camera->up * 0.2;
		changed = true;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		scene->camera->position -= scene->camera->right * 0.2;
		scene->camera->viewDirection -= scene->camera->right * 0.2;
		changed = true;
	}

	// move camera closer
	if (GetAsyncKeyState(VK_OEM_PLUS))
	{
		scene->camera->position += scene->camera->viewDirectionNormalized * 0.2;
		scene->camera->viewDirection += scene->camera->viewDirectionNormalized * 0.2;
		changed = true;
	}
	if (GetAsyncKeyState(VK_OEM_MINUS))
	{
		scene->camera->position -= scene->camera->viewDirectionNormalized * 0.2;
		scene->camera->viewDirection -= scene->camera->viewDirectionNormalized * 0.2;
		changed = true;
	}

	// rotating the camera
	if (GetAsyncKeyState('A'))
	{
		scene->camera->viewDirection -= scene->camera->right * 0.2;
		changed = true;
	}
	if (GetAsyncKeyState('D'))
	{
		scene->camera->viewDirection += scene->camera->right * 0.2;
		changed = true;
	}
	if (GetAsyncKeyState('W'))
	{
		scene->camera->viewDirection += scene->camera->up * 0.2;
		changed = true;
	}
	if (GetAsyncKeyState('S'))
	{
		scene->camera->viewDirection -= scene->camera->up * 0.2;
		changed = true;
	}

	// change FOV
	if (GetAsyncKeyState(VK_NUMPAD1))
	{
		scene->camera->fieldOfView += 0.01;
		changed = true;
	}
	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		scene->camera->fieldOfView -= 0.01;
		changed = true;
	}
	if (changed)
	{
		scene->camera->calculateScreen();
		printf("camera.z: %f \n", scene->camera->position.z);
		printf("FOV: %f \n", scene->camera->fieldOfView);
		printf("UP: %f, %f, %f \n", scene->camera->up.x, scene->camera->up.y, scene->camera->up.z);
	}

	// load teddy
	if (GetAsyncKeyState('T'))
	{
		this->loadTeddy();
	}
}

void Game::createScene()
{
	// create scene lights
	scene->lightSources.push_back(new DirectLight(scene->lightSources.size(), vec3(-1.0f, 0.0f, -3.0), vec4(1, 1, 1, 0), 20));
	scene->lightSources.push_back(new DirectLight(scene->lightSources.size(), vec3(0.0f, -2.0f, 0.0f), vec4(0.5, 0.5, 0.5, 0), 20));

	// create scene objects
	Material* redMaterial = new Material(vec4(1, 0, 0, 0), diffuse);
	Material* greenMaterial = new Material(vec4(0, 1, 0, 0), diffuse);
	Material* brownMaterial = new Material(vec4(0.756, 0.556, 0.094, 0), diffuse);
	Material* planeMaterial = new Material(vec4(0.75, 0.8, 0.7, 1), diffuse);
	Material* mirrorMaterial = new Material(vec4(0.75, 0.8, 0.7, 1), mirror);

	Material* blueGlassMaterial = new Material(vec4(0, 0, 1, 0.8), dielectric);
	blueGlassMaterial->refraction = 1.33;
	blueGlassMaterial->reflection = 0.1;

	scene->primitives.push_back(
		new Triangle(greenMaterial, scene->primitives.size(), vec3(4, 4, 4), vec3(1, 1, 4), vec3(2, 5, 4))
	);
	scene->primitives.push_back(
		new Sphere(blueGlassMaterial, scene->primitives.size(), vec3(2, -1, 2), 0.5) // spehere in the box
	);

	scene->primitives.push_back(
		new Cylinder(redMaterial, scene->primitives.size(), vec3(-1.5, -1.5, 0), vec3(1, 0, 0), 0.1, 0.5)
	);

	scene->primitives.push_back(
		new Torus(brownMaterial, scene->primitives.size(), 0.4f, 0.2f, vec3(-0.5, -0.5, 0.2), vec3(0.5, 0.5, -1))
	);

	// create box from planes
	scene->primitives.push_back(
		new Plane(planeMaterial, scene->primitives.size(), vec3(0, 0, 5), vec3(0, 0, -1)) // back
	);
	scene->primitives.push_back(
		new Plane(mirrorMaterial, scene->primitives.size(), vec3(0, -5, 5), vec3(0, 1, 0)) //top
	);
	scene->primitives.push_back(
		new Plane(mirrorMaterial, scene->primitives.size(), vec3(0, 5, 5), vec3(0, -1, 0)) //bottom
	);
	scene->primitives.push_back(
		new Plane(greenMaterial, scene->primitives.size(), vec3(-5, 0, 5), vec3(1, 0, 0)) //right
	);
	scene->primitives.push_back(
		new Plane(greenMaterial, scene->primitives.size(), vec3(5, 0, 5), vec3(-1, 0, 0)) // left
	);
}

void Game::loadTeddy()
{
	scene->primitives.clear();
	scene->lightSources.clear();

	scene->lightSources.push_back(new DirectLight(scene->lightSources.size(), vec3(-10.0f, 0.0f, -20.0), vec4(1, 1, 1, 0), 250));
	scene->lightSources.push_back(new DirectLight(scene->lightSources.size(), vec3(8.0f, 0.0f, -18.0), vec4(1, 1, 1, 0), 100));
	scene->lightSources.push_back(new DirectLight(scene->lightSources.size(), vec3(4.0f, 8.0f, -20.0), vec4(1, 1, 1, 0), 100));


	Material* planeMaterial = new Material(vec4(0.75, 0.8, 0.7, 1), diffuse);
	scene->primitives.push_back(
		new Plane(planeMaterial, scene->primitives.size(), vec3(0, 0, 50), vec3(0, 0, -1))
	);

	Material* redMaterial = new Material(vec4(1, 0, 0, 0), diffuse);
	scene->primitives.push_back(
		new Sphere(redMaterial, scene->primitives.size(), vec3(-25, 10, 0), 5)
	);

	scene->camera->position = vec3(0, 0, -50);
	scene->camera->up = vec3(0, 1, 0);
	scene->camera->calculateScreen();

	Material* brownMaterial = new Material(vec4(1, 0.8, 0.5, 0), diffuse);
	scene->loadObjModel("assets/teddy.obj", brownMaterial);
}