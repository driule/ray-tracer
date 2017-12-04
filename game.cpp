#include "precomp.h" // include (only) this in every .cpp file

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
	this->moveCamera();

	for (int i = 0; i < SCRHEIGHT / 32; i++)
	{
		jobManager->AddJob2(rayTracerJobs[i]);
	}
	jobManager->RunJobs();
}

void Game::moveCamera()
{
	if (GetAsyncKeyState(VK_DOWN))
	{
		scene->camera->position.y += 0.01;
		//scene->camera->calculateScreen();
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		scene->camera->position.x += 0.01;
		//scene->camera->calculateScreen();
	}
	if (GetAsyncKeyState(VK_UP))
	{
		scene->camera->position.y -= 0.01;
		//scene->camera->calculateScreen();
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		scene->camera->position.x -= 0.01;
		//scene->camera->calculateScreen();
	}
	if (GetAsyncKeyState(VK_OEM_PLUS))
	{
		scene->camera->position.z += 0.01;
		//scene->camera->calculateScreen();
	}
	if (GetAsyncKeyState(VK_OEM_MINUS))
	{
		scene->camera->position.z -= 0.01;
		//scene->camera->calculateScreen();
	}

	if (GetAsyncKeyState(VK_NUMPAD1))
	{
		scene->camera->fieldOfView += 0.01;
		scene->camera->calculateScreen();
	}
	if (GetAsyncKeyState(VK_NUMPAD2))
	{
		scene->camera->fieldOfView -= 0.01;
		scene->camera->calculateScreen();
	}
}