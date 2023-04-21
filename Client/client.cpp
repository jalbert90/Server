// Because Windows.h and WinSock2.h overlap
#ifndef W32_LEAN_AND_MEAN
#define W32_LEAN_AND_MEAN
#endif

// #include<Windows.h>
#include <WinSock2.h>			// For API
#include <WS2tcpip.h>			// Newer stuff?, such as getaddrinfo()
#include <stdio.h>				// For printf()

// Tell linker we need Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN	512

int main(int argc, char* argv[]) {
	// Struct containing WS (Windows Socket) data?
	WSADATA wsaData;

	// Integer to hold function return values (to be checked for errors)
	int iResult;

	// Initialize WS DLL
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		printf("WSAStartup failed %d\n", iResult);
		return 1;
	}

	// Struct addrinfo contains sockaddr struct and points to next addrinfo
	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	// Zero out hints memory location
	ZeroMemory(&hints, sizeof(hints));

	// Set hints struct fields?
	hints.ai_socktype = SOCK_STREAM;		// Connection based socket
	hints.ai_protocol = IPPROTO_TCP;		// TCP protocol
	hints.ai_family = AF_UNSPEC;			// IPv4 or IPv6

	// Resolve server name passed on command line to an IP address and port
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("Failed at getaddrinfo: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET object used to connect to server
	SOCKET ConnectSocket = INVALID_SOCKET;

	// Connect to first IP returned by getaddrinfo() that matches desired attributes
	ptr = result;

	// Create a socket for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Failed at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

}