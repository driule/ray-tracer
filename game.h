#pragma once

namespace Tmpl8 {

class Game
{
public:
	void SetTarget( Surface* surface ) { screen = surface; }
	void Init();
	void Shutdown();
	void Tick( float deltaTime );
	void MouseUp( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseDown( int button ) { /* implement if you want to detect mouse button presses */ }
	void MouseMove( int x, int y ) { /* implement if you want to detect mouse movement */ }
	void KeyUp( int key ) { /* implement if you want to handle keys */ }
	void KeyDown( int key ) { /* implement if you want to handle keys */ }

	void handleInput();
private:
	Surface* screen;

	void createScene();
	void loadTeddy();
	void loadTeapot();
};

class RayTracerJob : public Job
{
public:
	RayTracerJob(int start, int end) : start(start), end(end) {};
	void Main();
private:
	int start;
	int end;
};

}; // namespace Tmpl8