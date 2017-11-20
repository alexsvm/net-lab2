/*
������� TCP ���-������
�������� ������� ���������� �� ���� �� ������.

����: tcp_server.cpp

������ ����������:
bcc32 -otcp_server.exe tcp_server.cpp Ws2_32.lib
*/
#include <Winsock2.h>
#include <stdio.h>
#include <stdlib.h>

// TCP-���� �������
#define SERVICE_PORT 1500 

//
int main(void)
{
	SOCKET  S;  //���������� ��������������� ������
	SOCKET  NS; //���������� ��������������� ������

	sockaddr_in serv_addr;
	WSADATA     wsadata;
	char        sName[128];
	bool        bTerminate = false;

	// �������������� ���������� �������
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	// �������� �������� ��� ������� ������
	gethostname(sName, sizeof(sName));
	printf("\nServer host: %s\n", sName);

	// ������� �����
	// ��� TCP-������ ��������� �������� SOCK_STREAM
	// ��� UDP - SOCK_DGRAM 
	if ((S = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't create socket\n");
		exit(1);
	}


	//� API ������� ����� ��������� �������� 
	//������ � ������� ��������� ����� ������ TCP_NODELAY.
	const int on = 1;
	if (setsockopt(S, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, sizeof(on)) != 0)
	{
		fprintf(stderr, "Can't set socket options\n");
		exit(1);
	}

	// ��������� ��������� ������� 
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	// ��������� ������ �� ���� ��������� ������� �����������,
	// � ��������� �� localhost
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	// �������� �������� �� �������������� ������� ����
	serv_addr.sin_port = htons((u_short)SERVICE_PORT);

	// ��������� ����� � �������� ������� ���������� � ������
	if (bind(S, (sockaddr*)&serv_addr, sizeof(serv_addr)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't bind\n");
		exit(1);
	}

	// ��������� ����� � ����� ������������� ��������� �����
	// � ������������ ����������� �������� �������� �� ���������� 5
	if (listen(S, 5) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't listen\n");
		exit(1);
	}

	printf("Server listen on %s:%d\n",
		inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
	printf("Press Ctrl-C for terminate\n");

	// �������� ���� ��������� ����������� �������� 
	while (!bTerminate)
	{
		printf("Wait for connections.....\n");

		sockaddr_in clnt_addr;
		int addrlen = sizeof(clnt_addr);
		memset(&clnt_addr, 0, sizeof(clnt_addr));

		// ��������� ������ � ����� �������� ������� �� ����������.
		// ����� ����������, �.�. ���������� ���������� ������ ��� 
		// ����������� ������� ��� ������ 
		NS = accept(S, (sockaddr*)&clnt_addr, &addrlen);
		if (NS == INVALID_SOCKET)
		{
			fprintf(stderr, "Can't accept connection\n");
			break;
		}
		// �������� ��������� ��������������� ������ NS �
		// ���������� � �������
		addrlen = sizeof(serv_addr);
		getsockname(NS, (sockaddr*)&serv_addr, &addrlen);
		// ������� inet_ntoa ���������� ��������� �� ���������� ������, 
		// ������� ������������ �� � ����� ������ printf �� ���������
		printf("Accepted connection on %s:%d ",
			inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
		printf("from client %s:%d\n",
			inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

		// �������� � ������������ ������ �� �������
		size_t nRetransmitBytesNum = 0;
		char sReceiveBuffer[65536] = { 0 };
		while (true)
		{
			int nReaded = recv(NS, sReceiveBuffer, sizeof(sReceiveBuffer), 0);
			// � ������ ������ (��������, ������������ �������) �������
			if (nReaded <= 0) break;
			// �������� dct ���������� ������ ��� ������ �������
			if (send(NS, sReceiveBuffer, nReaded, 0) < nReaded)
			{
				fprintf(stderr, "Cann't send all data\n");
			}
			nRetransmitBytesNum += nReaded;
		}

		// ��������� �������������� �����
		closesocket(NS);
		printf("Client disconnected.\n");
		printf("Retransmit %d bytes.\n", nRetransmitBytesNum);
	}
	// ��������� ��������� �����
	closesocket(S);
	// ����������� ������� ���������� �������
	WSACleanup();
	return 0;
}
