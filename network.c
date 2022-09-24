#include <WinSock2.h>

#include "main.h"
#include "network.h"
#include "ui.h"

#pragma comment(lib,"Ws2_32.lib")

WSADATA wsadata;
SOCKET tcpSock;
SOCKADDR_IN tcpAddress;

u8 networkSettings;

u8 networkplayerC;
NETWORKDATA networkplayer;

NETWORKPLAYER networkthis;

u32 networkID;

u8 connectStatus;

HANDLE lagCompensationThread;

u8 packetID;

PACKETDATA packetdata;

u8 *playerName;

STRINGS networkplayerNames;

void lagCompensation(){
	while(connectStatus){
		for(u32 i = 0;i < networkplayerC;i++){
			VEC3addVEC3(&networkplayer.player[i].pos,networkplayer.lagcomp[i].vel);
		}
		Sleep(15);
	}
}

void serverRecv(){
	u8 packetID = 0;
	//lagCompensationThread = CreateThread(0,0,lagCompensation,0,0,0);
	for(;;){
		recv(tcpSock,&packetID,1,0);
		for(u32 i = 0;i < networkplayerC;i++){
			networkplayer.lagcomp[i].vel = VEC3subVEC3R(networkplayer.lagcomp[i].posBuf,networkplayer.player[i].pos);
			networkplayer.lagcomp[i].posBuf = networkplayer.player[i].pos;
		}
		switch(packetID){
		case 0:
			recv(tcpSock,networkplayer.player,networkplayerC*sizeof(NETWORKPLAYER),0);
			break;
		case 1:{
			u8 tID;
			recv(tcpSock,&tID,1,0);
			recv(tcpSock,networkplayerNames.str[tID],20,0);
			memcpy(chat[0].text,networkplayerNames.str[tID],20);
			memcpy(chat[0].text+strlen(networkplayerNames.str[tID])," has joined",20);
			chat[0].timer = 800;
			spawnPlayer(networkplayerC);
			networkplayerC++;
			break;
		}
		case 2:
			printf("error\n");
			networkplayerC--;
			for(u32 i = 0;i < entityC;i++){
				if(entity.cpu[i].id==9&&entity.cpu[i].health==networkplayerC){
					entityDeath(i);
					break;
				}
			}
			break;
		case 3:{
			PACKETDATA packetDataIn;
			recv(tcpSock,&packetDataIn,sizeof(PACKETDATA),0);
			spawnEntityEx(packetDataIn.pos1,packetDataIn.pos2,(VEC3){0.0f,0.0f,0.0f},10,(VEC3){0.5f,0.1f,0.1f});
			break;
		case 4:
			for(u32 i2 = CHATSZ-1;i2 > 0;i2--){
				memcpy(chat[i2].text,chat[i2-1].text,40);
				chat[i2].timer = chat[i2-1].timer;
			}
			u8 sendID;
			recv(tcpSock,&sendID,1,0);
			strcpy(chat[0].text,networkplayerNames.str[sendID]);
			recv(tcpSock,chat[0].text+strlen(networkplayerNames.str[sendID])+1,20,0);
			break;
		}
		}
	}
}

void networking(){
	networkplayerNames.str = HeapAlloc(GetProcessHeap(),8,sizeof(u8*)*8);
	for(u32 i = 0;i < MAXPLAYERS;i++){
		networkplayerNames.str[i] = HeapAlloc(GetProcessHeap(),8,20);
	}

	WSAStartup(MAKEWORD(2, 2),&wsadata);
	tcpSock = socket(AF_INET,SOCK_STREAM,0);

	connectStatus++;

	tcpAddress.sin_family = AF_INET;
	tcpAddress.sin_port   = htons(7778);
	tcpAddress.sin_addr.S_un.S_un_b.s_b1 = sliderValues.serverIP.p1;
	tcpAddress.sin_addr.S_un.S_un_b.s_b2 = sliderValues.serverIP.p2;
	tcpAddress.sin_addr.S_un.S_un_b.s_b3 = sliderValues.serverIP.p3;
	tcpAddress.sin_addr.S_un.S_un_b.s_b4 = sliderValues.serverIP.p4;

	while(connect(tcpSock,(SOCKADDR*)&tcpAddress,sizeof(tcpAddress)) && connectStatus){Sleep(15);}
	connectStatus++;
	networkSettings |= 0x01;
	printf("connected\n");
	
	SuspendThread(physicsThread);
	SuspendThread(entityThread);

	HeapFree(GetProcessHeap(),0,map);
	HeapFree(GetProcessHeap(),0,metadt);
	HeapFree(GetProcessHeap(),0,metadt2);
	HeapFree(GetProcessHeap(),0,metadt3);
	HeapFree(GetProcessHeap(),0,metadt4);
	HeapFree(GetProcessHeap(),0,metadt5);
	HeapFree(GetProcessHeap(),0,metadt6);
	HeapFree(GetProcessHeap(),0,lpmap);
	HeapFree(GetProcessHeap(),0,lmap);

	recv(tcpSock,&properties->lvlSz,1,0);
	recv(tcpSock,&properties->lmapSz,4,0);
	recv(tcpSock,&networkplayerC,1,0);
	recv(tcpSock,&networkID,1,0);
	recv(tcpSock,&lmapC,4,0,0);
	networkplayerC--;

	map        = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(MAP));
	metadt     = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(MAP));
	metadt2    = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(MAP));
	metadt3    = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(MAP));
	metadt4    = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(MAP));
	metadt5    = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(MAP));
	metadt6    = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(MAP));
	lpmap      = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(LPMAP));
	lmap       = HeapAlloc(GetProcessHeap(),8,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB));

	printf("%i\n",lmap);
	connectStatus++;
	recv(tcpSock,map,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,MSG_WAITALL);

	connectStatus++;
	recv(tcpSock,metadt,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,MSG_WAITALL);

	connectStatus++;
	recv(tcpSock,metadt2,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,MSG_WAITALL);

	connectStatus++;
	recv(tcpSock,metadt3,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,MSG_WAITALL);

	connectStatus++;
	recv(tcpSock,metadt4,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,MSG_WAITALL);

	connectStatus++;
	recv(tcpSock,metadt5,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,MSG_WAITALL);

	connectStatus++;
	recv(tcpSock,metadt6,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,MSG_WAITALL);

	connectStatus++;
	recv(tcpSock,lpmap,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(LPMAP),MSG_WAITALL);
	connectStatus++;

	playerspawnC = 0;
	for(u32 i = 0;i < BLOCKCOUNT;i++){
		switch(map[i].id){
		case BLOCK_SPAWN:{
			CVEC3 spwncrd = map2crds(i);
			playerspawn[playerspawnC] = (VEC3){spwncrd.x+0.5f,spwncrd.y+0.5f,spwncrd.z+2.25f};
			playerspawnC++;
			break;
		}
		}
	}

	if(playerspawnC){
		player->pos = playerspawn[abs(irnd())%(playerspawnC)];
	}

	settings &= ~SETTINGS_MOVEMENT;
	settings &= ~SETTINGS_LIGHTING;
	settings |= SETTINGS_GAMEPLAY;

	recv(tcpSock,lmap,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB),MSG_WAITALL);
	connectStatus++;
	printf("loaded\n");
	for(u32 i = 0;i < networkplayerC;i++){
		u8 tID;
		recv(tcpSock,&tID,1,0);
		recv(tcpSock,networkplayerNames.str[tID],20,0);
		spawnPlayer(i);
	}
	glMes[glMesC].id = 6;
	glMesC++;
	glMes[glMesC].id = 3;
	glMesC++;
	menuSel = 0;
	ResumeThread(physicsThread);
	ResumeThread(entityThread);
	CreateThread(0,0,serverRecv,0,0,0);

	player->weaponEquiped = 1;
	spawnEntityEx((VEC3){player->pos.x+player->xdir,player->pos.y+player->ydir,player->pos.z},(VEC3){0.1f,0.1f,-0.3f},(VEC3){0.0f,0.0f,0.0f},4,(VEC3){0.0f,0.04f,0.0f});

	send(tcpSock,playerName,20,0);

	for(;;){
		networkthis.pos = player->pos;
		networkthis.pos.z += 0.2f;
		networkthis.rot = player->xangle;
		send(tcpSock,&packetID,1,0);
		switch(packetID){
		case 1:
			send(tcpSock,&packetdata,sizeof(PACKETDATA),0);
			packetID = 0;
			break;
		case 2:
			send(tcpSock,chat[0].text,20,0);
			packetID = 0;
			break;
		}
		send(tcpSock,&networkthis,sizeof(NETWORKPLAYER),0);
		Sleep(30);
	}
}