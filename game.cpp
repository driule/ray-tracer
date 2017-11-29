#include "precomp.h" // include (only) this in every .cpp file

Scene* scene;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	printf("Use arrows to move camera around....\n");
	scene = new Scene();
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
}

static Sprite rotatingGun( new Surface( "assets/aagun.tga" ), 36 );
static int frame = 0;

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	this->moveCamera();
	// clear the graphics window
	//screen->Clear( 0 );

	// print something in the graphics window
	//screen->Print( "hello world", 2, 2, 0xffffff );

	scene->render(screen);

	// move camera around
	//if (frame < 25) scene->camera->position.x += 0.1;
	//else if (frame > 75) scene->camera->position.y += 0.1;
	//else if (frame > 50) scene->camera->position.x -= 0.1;
	//else if (frame > 25) scene->camera->position.y -= 0.1;

	//scene->camera->calculateScreen();
	//scene->primitives[0]->position = vec3(scene->primitives[0]->position.x, scene->primitives[0]->position.y, scene->primitives[0]->position.z);

	// print something to the text window
	//printf( "this goes to the console window.\n" );

	// draw a sprite
	//rotatingGun.SetFrame( frame );
	//rotatingGun.Draw( screen, 100, 100 );
	if (++frame == 100) frame = 0;
}

void Game::moveCamera()
{
	if (GetAsyncKeyState(VK_DOWN))
	{
		scene->camera->position.y += 0.1;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		scene->camera->position.x += 0.1;
	}
	if (GetAsyncKeyState(VK_UP))
	{
		scene->camera->position.y -= 0.1;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		scene->camera->position.x -= 0.1;
	}
	if (GetAsyncKeyState(VK_OEM_PLUS))
	{
		scene->camera->fieldOfView += 0.1;
		scene->camera->calculateScreen();
	}
	if (GetAsyncKeyState(VK_OEM_MINUS))
	{
		scene->camera->fieldOfView -= 0.1;
		scene->camera->calculateScreen();
	}
}