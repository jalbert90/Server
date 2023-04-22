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

	int recvBufLen = DEFAULT_BUFLEN;						// Max bytes that can be received in a message from server
	const char* sendBuf = "Msg sent from client";			// Message to send to server
	char recvBuf[DEFAULT_BUFLEN];							// char array to store message from server

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

	// Attempt to connect to address until it works
	for (ptr = result, ptr != NULL, ptr = ptr->ai_next) {
		// Create a socket for connecting to server and assign to SOCKET object
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET) {
			printf("Failed at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR) {
			printf("connect() failed: %d\n", iResult);
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}

		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect\n");
		WSACleanup();
		return 1;
	}

	// Send sendBuf to ConnectSocket which is connected to ptr->ai_addr
	iResult = send(ConnectSocket, sendBuf, (int)strlen(sendBuf), 0);

	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Number of bytes sent to server
	printf("Bytes sent: %ld\n", iResult);

	// Done sending messages
	// Shutdown send-side (disable send operations)
	iResult = shutdown(ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		printf("shutdown() failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive messages until server closes the connection
	do {
		// Receive message from server and store in recvBuf
		iResult = recv(ConnectSocket, recvBuf, recvBufLen, 0);

		if (iResult > 0) {
			// If bytes received > 0, do this
			printf("Bytes received: %d\n", iResult);
			printf("Message received: %s\n", recvBuf);
		}
		else if (iResult == 0) {
			// If bytes received == 0, do this
			printf("Connection closed by server\n");
		}
		else {
			// Error case
			printf("Error at recv(): %d\n", WSAGetLastError());
		}
	} while (iResult > 0);

	// Disconnect receive side
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;
}