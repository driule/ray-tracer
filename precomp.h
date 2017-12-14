// add your includes to this file instead of to individual .cpp files
// to enjoy the benefits of precompiled headers:
// - fast compilation
// - solve issues with the order of header files once (here)
// do not include headers in header files (ever).

#define SCRWIDTH		800
#define SCRHEIGHT		512

#define SCRWIDTHf		800.0f
#define SCRHEIGHTf		512.0f

#define ASPECT_RATIO 0.64f

#define BGCOLOR			vec4(0.5f, 0.5f, 0.5f, 1)

#define EPSILON 1e-6

#define CAMERA_ORIGIN vec3(0, 0, -3)

#define BVH_ENABLED 1

enum MaterialType { diffuse, mirror, dielectric };

// #define FULLSCREEN
// #define ADVANCEDGL	// faster if your system supports it

#include <inttypes.h>
extern "C" 
{ 
#include "glew.h" 
}
#include "gl.h"
#include "io.h"
#include <fstream>
#include <stdio.h>
#include "fcntl.h"
#include "SDL.h"
#include "wglext.h"
#include "freeimage.h"
#include "math.h"
#include "stdlib.h"
#include "emmintrin.h"
#include "immintrin.h"
#include "windows.h"
#include "template.h"
#include "surface.h"
#include "threads.h"
#include <assert.h>
#include <sstream>
#include <vector>

#include "quarticsolver.h"

#include "Ray.h"
#include "Camera.h"
#include "Primitives.h"
#include "Model.h"
#include "BVH.h"
#include "LightSources.h"
#include "Scene.h"


using namespace std;
using namespace Tmpl8;

#include "game.h"