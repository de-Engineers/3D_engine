#include <main.h>
#include <math.h>

VEC3 front;
VEC3 up;
VEC3 right;

VEC3 worldUp = {
	0.0, 0.0, 1.0
};

Mat3 cameraMatrix;

void updateCamera() {

	front.x = cosf(player->xangle) * cosf(player->yangle);
    front.y = sinf(player->xangle) * cosf(player->yangle);
	front.z = sinf(player->yangle);

	float frontLength = sqrtf(front.x * front.x + front.y * front.y + front.z * front.z);

	front.x = front.x / frontLength;
    front.y = front.y / frontLength;
    front.z = front.z;
	
	right.x = -front.y * worldUp.z;
	right.y = worldUp.z * front.x;
	right.z = 0.0;

	up.x = front.y * right.z - right.y * front.z;
	up.y = front.z * right.x - right.z * front.x;
	up.z = front.x * right.y - right.x * front.y;

	cameraMatrix.Right = right;
	cameraMatrix.Up = up;
	cameraMatrix.Front = front;
}
