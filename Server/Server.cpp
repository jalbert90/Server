#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN									// Because Windows.h and WinSock2.h have duplicate elements
#endif

// #include <Windows.h>
#include <stdio.h>											// For printf()
#include <WinSock2.h>										// For API
#include <WS2tcpip.h>										// For newer stuff, such as getaddrinfo()

#pragma comment(lib, "Ws2_32.lib")							// Tell linker we need this

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN	512

int main() {
	// Struct, info about WS (Windows Sockets) implementation
	WSADATA wsaData;

	// Integer to store results from function calls (to be checked for errors)
	int iResult;

	// Initialize WS DLL
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup Failed: %d\n", iResult);
		return 1;
	}

	// Create structs to hold address attributes?
	struct addrinfo *result = NULL,
					hints;

	// Zero out hints memory
	ZeroMemory(&hints, sizeof(hints));

	// Set address attributes
	hints.ai_family = AF_INET;				// IPv4
	hints.ai_socktype = SOCK_STREAM;		// Connection based socket
	hints.ai_protocol = IPPROTO_TCP;		// TCP
	hints.ai_flags = AI_PASSIVE;			// addrinfo suitable for binding to a socket

	// Resolve server address? Set server address to INADDR_ANY (IP of this machine?)
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo() failed: %d\n", iResult);
		freeaddrinfo(result);
		WSACleanup();			// ???
		return 1;
	}
}