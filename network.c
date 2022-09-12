#include <WinSock2.h>
#include "main.h"
#include "network.h"


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

void lagCompensation(){
	Sleep(15);
	for(;;){
		for(u32 i = 0;i < networkplayerC;i++){
			VEC3addVEC3(&networkplayer.player[i].pos,networkplayer.lagcomp[i].vel);
		}
		Sleep(15);
	}
}

void serverRecv(){
	u8 packetID = 0;
	for(;;){
		HANDLE lagCompensationThread = CreateThread(0,0,lagCompensation,0,0,0);
		recv(tcpSock,&packetID,1,0);
		TerminateThread(lagCompensationThread,0);
		for(u32 i = 0;i < networkplayerC;i++){
			networkplayer.lagcomp[i].vel = VEC3subVEC3R(networkplayer.player[i].pos,networkplayer.lagcomp[i].posBuf);
			networkplayer.lagcomp[i].posBuf = networkplayer.player[i].pos;
		}
		switch(packetID){
		case 0:
			recv(tcpSock,&networkplayer.player,networkplayerC*sizeof(NETWORKPLAYER),0);
			break;
		case 1:
			spawnPlayer(networkplayerC);
			networkplayerC++;
			break;
		case 2:
			networkplayerC--;
			for(u32 i = 0;i < entityC;i++){
				if(entity.cpu[i].id==9&&entity.cpu[i].health==networkplayerC){
					entityDeath(i);
					break;
				}
			}
			break;
		}
	}
}

void networking(){
	connectStatus++;
	WSAStartup(MAKEWORD(2, 2),&wsadata);
	tcpSock = socket(AF_INET,SOCK_STREAM,0);

	tcpAddress.sin_family = AF_INET;
	tcpAddress.sin_port   = htons(7778);
	tcpAddress.sin_addr.S_un.S_un_b.s_b1 = sliderValues.serverIP.p1;
	tcpAddress.sin_addr.S_un.S_un_b.s_b2 = sliderValues.serverIP.p2;
	tcpAddress.sin_addr.S_un.S_un_b.s_b3 = sliderValues.serverIP.p3;
	tcpAddress.sin_addr.S_un.S_un_b.s_b4 = sliderValues.serverIP.p4;

	while(connect(tcpSock,(SOCKADDR*)&tcpAddress,sizeof(tcpAddress))){}
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
	printf("%i\n",lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB));
	Sleep(1000);
	recv(tcpSock,lmap,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB),0,MSG_WAITALL);
	connectStatus++;
	printf("loaded\n");
	for(u32 i = 0;i < networkplayerC;i++){
		spawnPlayer(i);
	}
	glMes[glMesC].id = 6;
	glMesC++;
	glMes[glMesC].id = 3;
	glMesC++;
	ResumeThread(physicsThread);
	ResumeThread(entityThread);
	CreateThread(0,0,serverRecv,0,0,0);
	for(;;){
		networkthis.pos = player->pos;
		send(tcpSock,&networkthis,sizeof(NETWORKPLAYER),0);
		Sleep(15);
	}
}