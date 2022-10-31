#include "main.h"
#include "blockmove.h"

u8 blockmoveC;
BLOCKMOVE *blockmove;

void createMoveBlock(u32 block){
	blockmove[blockmoveC].temp.pos  = (VEC3){metadt[block].r ,metadt[block].g ,metadt[block].id};
	blockmove[blockmoveC].temp.size = (VEC3){metadt2[block].r,metadt2[block].g,metadt2[block].id};
	blockmove[blockmoveC].temp.rot  = (VEC3){metadt3[block].r,metadt3[block].g,metadt3[block].id};
	blockmove[blockmoveC].vel.pos  = VEC3divR(VEC3subVEC3R((VEC3){metadt4[block].r,metadt4[block].g,metadt4[block].id},blockmove[blockmoveC].temp.pos),30.0f);
	blockmove[blockmoveC].vel.size = VEC3divR(VEC3subVEC3R((VEC3){metadt5[block].r,metadt5[block].g,metadt5[block].id},blockmove[blockmoveC].temp.size),30.0f);
	blockmove[blockmoveC].vel.rot  = VEC3divR(VEC3subVEC3R((VEC3){metadt6[block].r,metadt6[block].g,metadt6[block].id},blockmove[blockmoveC].temp.rot),30.0f);
	metadt4[block].r = metadt[block].r;
	metadt4[block].g = metadt[block].g;
	metadt4[block].id= metadt[block].id;
	metadt5[block].r = metadt2[block].r;
	metadt5[block].g = metadt2[block].g;
	metadt5[block].id= metadt2[block].id;
	metadt6[block].r = metadt3[block].r;
	metadt6[block].g = metadt3[block].g;
	metadt6[block].id= metadt3[block].id;
	blockmove[blockmoveC].block = block;
	blockmove[blockmoveC].itt = 30;
	blockmoveC++;	
}

void blockMoveDeath(u8 id){
	for(int i = id;i < blockmoveC;i++){
		blockmove[i] = blockmove[i+1];
	}
	blockmoveC--;
}

void moveBlocks(){
	for(u32 i = 0;i < blockmoveC;i++){
		VEC3addVEC3(&blockmove[i].temp.pos,blockmove[i].vel.pos);
		VEC3addVEC3(&blockmove[i].temp.size,blockmove[i].vel.size);
		VEC3addVEC3(&blockmove[i].temp.rot,blockmove[i].vel.rot);
		metadt[blockmove[i].block].r  = blockmove[i].temp.pos.x;
		metadt[blockmove[i].block].g  = blockmove[i].temp.pos.y;
		metadt[blockmove[i].block].id = blockmove[i].temp.pos.z;
		metadt2[blockmove[i].block].r = blockmove[i].temp.size.x;
		metadt2[blockmove[i].block].g = blockmove[i].temp.size.y;
		metadt2[blockmove[i].block].id= blockmove[i].temp.size.z;
		metadt3[blockmove[i].block].r = blockmove[i].temp.rot.x;
		metadt3[blockmove[i].block].g = blockmove[i].temp.rot.y;
		metadt3[blockmove[i].block].id= blockmove[i].temp.rot.z;
		blockmove[i].itt--;
		glMes[glMesC].id = 3;
		glMesC++;
		if(!blockmove[i].itt){
			blockMoveDeath(i);
		}
	}
}

