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
	scene->primitives[3]->position = vec3(scene->primitives[3]->position.x, scene->primitives[3]->position.y + 0.2, scene->primitives[3]->position.z);
	scene->primitives[4]->position = vec3(scene->primitives[4]->position.x + 0.2, scene->primitives[4]->position.y, scene->primitives[4]->position.z);

	// print something to the text window
	//printf( "this goes to the console window.\n" );

	// draw a sprite
	//rotatingGun.SetFrame( frame );
	//rotatingGun.Draw( screen, 100, 100 );
	//if (++frame == 36) frame = 0;
}