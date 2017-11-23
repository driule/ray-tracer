#pragma once
namespace Tmpl8 {
	class Camera
	{
	public:
		Camera();
		~Camera();
		vec3 position;
		vec3 viewDirection;
		int fieldOfView;
		vec3 generateRay(int x, int y);
	private:

	};
}

