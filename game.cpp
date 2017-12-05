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
	scene = new Scene(screen);

	rayTracerJobs = new RayTracerJob*[SCRHEIGHT / 32];
	for (int i = 0; i < SCRHEIGHT / 32; i++)
	{
		rayTracerJobs[i] = new RayTracerJob(i * 32, (i + 1) * 32);
	}

	JobManager::CreateJobManager(4);
	jobManager = JobManager::GetJobManager();

	// https://groups.csail.mit.edu/graphics/classes/6.837/F03/models/
	Material* greyMaterial = new Material(vec4(1, 0.8f, 0.5f, 1.0f), diffuse);
	scene->loadObjModel("assets/cube.obj", greyMaterial);

	// load teddy to the scene
	/*
	scene->camera->position = vec3(0, 0, -100);
	scene->camera->up = vec3(0, 1, 0);
	scene->camera->calculateScreen();
	scene->loadObjModel("assets/teddy.obj", greyMaterial);
	*/
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
		scene->camera->position -= scene->camera->up * 0.1;
		scene->camera->viewDirection -= scene->camera->up * 0.1;		
		changed = true;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		scene->camera->position += scene->camera->right * 0.1;
		scene->camera->viewDirection += scene->camera->right * 0.1;
		changed = true;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		scene->camera->position += scene->camera->up * 0.1;
		scene->camera->viewDirection += scene->camera->up * 0.1;
		changed = true;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		scene->camera->position -= scene->camera->right * 0.1;
		scene->camera->viewDirection -= scene->camera->right * 0.1;
		changed = true;
	}

	// move camera closer
	if (GetAsyncKeyState(VK_OEM_PLUS))
	{
		scene->camera->position += scene->camera->viewDirectionNormalized * 0.1;
		changed = true;
	}
	if (GetAsyncKeyState(VK_OEM_MINUS))
	{
		scene->camera->position -= scene->camera->viewDirectionNormalized * 0.1;
		changed = true;
	}

	// rotating the camera
	if (GetAsyncKeyState('A'))
	{
		scene->camera->viewDirection -= scene->camera->right * 0.1;
		changed = true;
	}
	if (GetAsyncKeyState('D'))
	{
		scene->camera->viewDirection += scene->camera->right * 0.1;
		changed = true;
	}
	if (GetAsyncKeyState('W'))
	{
		scene->camera->viewDirection += scene->camera->up * 0.1;
		changed = true;
	}
	if (GetAsyncKeyState('S'))
	{
		scene->camera->viewDirection -= scene->camera->up * 0.1;
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
}