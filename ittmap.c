#include "main.h"
#include <math.h>
#include <stdio.h>

CVEC3 map2crds(u32 map){
	CVEC3 m;
	m.x = map % properties->lvlSz;
	m.y = map / properties->lvlSz % properties->lvlSz;
	m.z = map / properties->lvlSz / properties->lvlSz;
	return m;
}

void ittmap(){

}
