#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN									// Because Windows.h and WinSock2.h have duplicate elements
#endif

// #include <Windows.h>
#include <stdio.h>											// For printf()
#include <WinSock2.h>										// For API
// #include <WS2tcpip.h>									// For newer stuff

#pragma comment(lib, "Ws2_32.lib")							// Tell linker we need this

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN	512

int main() {
	// Struct, info about WS (Windows Sockets) implementation
	WSADATA wsaData;

	int iResult;

	// Initialize WS DLL
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup Failed: %d\n", iResult);
		return 1;
	}


}