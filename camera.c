#include <main.h>
#include <math.h>
VEC3 playerDir, playerDirRight, playerDirUp;

void cameraUpdate(){
	float angleX, angleY, angleZ;

	float xd, yd, zd, xyd;
	xd = player->xdir;
	yd = player->ydir;
	zd = player->zdir;
	xyd = player->xydir;

	playerDir 		= (VEC3){xd * xyd, yd * xyd, zd};
	playerDirRight  = (VEC3){-yd, xd, 0.0};
	playerDirUp 	= (VEC3){-xd * zd, -yd * zd, xyd};
};