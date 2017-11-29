#include "precomp.h" // include (only) this in every .cpp file

Scene* scene;

// blue = (int)(c.x * 255)
// color = blue + (green << 8) + (red << 16)

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
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
	// clear the graphics window
	//screen->Clear( 0 );

	// print something in the graphics window
	//screen->Print( "hello world", 2, 2, 0xffffff );

	scene->render(screen);

	// move camera around
	if (frame < 25) scene->camera->position.x += 0.1;
	else if (frame > 75) scene->camera->position.y += 0.1;
	else if (frame > 50) scene->camera->position.x -= 0.1;
	else if (frame > 25) scene->camera->position.y -= 0.1;

	//scene->camera->calculateScreen();
	//scene->primitives[0]->position = vec3(scene->primitives[0]->position.x, scene->primitives[0]->position.y, scene->primitives[0]->position.z);

	// print something to the text window
	//printf( "this goes to the console window.\n" );

	// draw a sprite
	//rotatingGun.SetFrame( frame );
	//rotatingGun.Draw( screen, 100, 100 );
	if (++frame == 100) frame = 0;
}