#include "main.h"
#include "network.h"

#pragma comment(lib,"Ws2_32.lib")

WSADATA wsadata;
SOCKET tcpSock;
SOCKADDR_IN tcpAddress;

u8 networkSettings;

u8 networkplayerC;
NETWORKPLAYER networkplayer[8];

NETWORKPLAYER networkthis;

u32 networkID;

IPADDRESS serverIP = {192,168,1,1};

void serverRecv(){
	u8 packetID = 0;
	for(;;){
		recv(tcpSock,&packetID,1,0);
		switch(packetID){
		case 0:
			recv(tcpSock,networkplayer,networkplayerC*sizeof(NETWORKPLAYER),0);
			break;
		case 1:
			Sleep(999);
			spawnPlayer(networkplayerC);
			networkplayerC++;
			break;
		}
	}
}

void networking(){
	WSAStartup(MAKEWORD(2, 2),&wsadata);
	tcpSock = socket(AF_INET,SOCK_STREAM,0);

	tcpAddress.sin_family = AF_INET;
	tcpAddress.sin_port   = htons(7778);
	tcpAddress.sin_addr.S_un.S_un_b.s_b1 = serverIP.p1;
	tcpAddress.sin_addr.S_un.S_un_b.s_b2 = serverIP.p2;
	tcpAddress.sin_addr.S_un.S_un_b.s_b3 = serverIP.p3;
	tcpAddress.sin_addr.S_un.S_un_b.s_b4 = serverIP.p4;

	while(connect(tcpSock,(SOCKADDR*)&tcpAddress,sizeof(tcpAddress))){}

	networkSettings |= 0x01;
	printf("connected\n");

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

	recv(tcpSock,map,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0);
	recv(tcpSock,metadt,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0);	
	recv(tcpSock,metadt2,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0);
	recv(tcpSock,metadt3,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0);
	recv(tcpSock,metadt4,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0);
	recv(tcpSock,metadt5,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0);
	recv(tcpSock,metadt6,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0);
	recv(tcpSock,lpmap,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(LPMAP),0);
	lmap = HeapAlloc(GetProcessHeap(),8,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB));
	recv(tcpSock,lmap,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB),0,0);
	printf("loaded\n");
	for(u32 i = 0;i < networkplayerC;i++){
		spawnPlayer(i);
	}
	glMes[glMesC].id = 6;
	glMesC++;
	glMes[glMesC].id = 3;
	glMesC++;
	CreateThread(0,0,serverRecv,0,0,0);
	for(;;){
		networkthis.pos = player->pos;
		send(tcpSock,&networkthis,sizeof(NETWORKPLAYER),0);
		Sleep(15);
	}
}