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

	int recvBufLen = DEFAULT_BUFLEN;			// Max number of bytes per message?
	char recvBuf[DEFAULT_BUFLEN];				// char array to store message from client

	// Integer to store results from function calls (to be checked for errors)
	int iResult;

	// Integer to store result from send function call (to be checked for errors)
	int iSendResult;

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

	// Create a SOCKET object to hold a socket
	SOCKET ListenSocket = INVALID_SOCKET;			// Socket will bind to address of machine and listen?

	// Create a socket and assign to SOCKET object
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	if (ListenSocket == SOCKET_ERROR) {
		printf("socket() failed: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Bind newly created socket to IP address of this machine
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

	if (iResult == SOCKET_ERROR) {
		printf("bind() failed: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Done with addrinfo result--free the memory?
	freeaddrinfo(result);

	// Listen for incoming connections on socket bound to IP and port
	iResult = listen(ListenSocket, SOMAXCONN);

	if (iResult == SOCKET_ERROR) {
		printf("listen() failed: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
	}

	// Create a socket to handle client connection
	SOCKET ClientSocket = INVALID_SOCKET;

	// Accept connection from ListenSocket (creates new socket), assign to ClientSocket for handling. (Pass connection to new socket so ListenSocket can continue to listen?)
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket == INVALID_SOCKET) {
		printf("accept() failed: %ld\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// Receive message from client
	do {
		iResult = recv(ClientSocket, recvBuf, recvBufLen, 0);

		if (iResult > 0) {
			printf("Bytes received: %ld\n", iResult);
			printf("Message received: %s\n", recvBuf);

			// Echo message back to client
			iSendResult = send(ClientSocket, recvBuf, iResult, 0);
			
			if (iSendResult == SOCKET_ERROR) {
				printf("send() failed: %ld\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}

			printf("Bytes sent: %ld\n", iSendResult);
		}
		else if (iResult == 0) {
			printf("Connection closing...\n");
		}
		else {
			printf("Error at recv(): %ld\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
		}
	} while (iResult > 0);

	// Disconnect...
	// ...send-side
	iResult = shutdown(ClientSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("Send operations shutdown() failed: %ld\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// ...receive-side
	closesocket(ClientSocket);
	WSACleanup();
	return 0;
}