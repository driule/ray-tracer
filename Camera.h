#pragma once
namespace Tmpl8 {
	class Camera
	{
	public:
		Camera();

		vec3 position;
		vec3 viewDirection;
		vec3 up;
		vec3 right;
		vec3 viewDirectionNormalized;
		float fieldOfView;
		vec3 topLeft, topRight, bottomLeft;

		void reset();
		void calculateScreen();
		Ray* generateRay(int x, int y);
	};
}

