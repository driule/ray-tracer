#pragma once
namespace Tmpl8 {
	class Camera
	{
	public:
		Camera();

		vec3 position;
		vec3 viewDirection;
		int fieldOfView;
		vec3 topLeft, topRight, bottomLeft;

		vec3 generateRay(int x, int y);
	private:

	};
}

