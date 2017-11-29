#pragma once
namespace Tmpl8 {
	class Camera
	{
	public:
		Camera();

		vec3 position;
		vec3 viewDirection;
		float fieldOfView;
		vec3 topLeft, topRight, bottomLeft;

		void calculateScreen();
		Ray* generateRay(int x, int y);
	};
}

