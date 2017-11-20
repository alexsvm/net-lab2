/*
������� UDP ���-������
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

	sockaddr_in serv_addr;
	WSADATA     wsadata;
	char        sName[128];
	bool        bTerminate = false;

	// �������������� ���������� �������
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	// �������� �������� ��� ������� ������
	gethostname(sName, sizeof(sName));
	printf("\nServer host: %s\n", sName);

	// ������� UDP �����
	// ��� TCP-������ ��������� �������� SOCK_STREAM
	// ��� UDP - SOCK_DGRAM 
	if ((S = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't create socket\n");
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

	printf("Server listen on %s:%d\n",
		inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

	printf("Press Ctrl-C for terminate\n");

	// �������� ���� ��������� ����������� �������� 
	while (true)
	{
		sockaddr FromAddress;
		int nFromSize = sizeof(FromAddress);
		char sReceiveBuffer[65536] = { 0 };
		int nBytesReaded = recvfrom(S,
			sReceiveBuffer,
			sizeof(sReceiveBuffer),
			0,
			&FromAddress,
			&nFromSize);

		if (nBytesReaded > 0)
		{
			int nSendBytes = sendto(S,
				sReceiveBuffer,
				nBytesReaded,
				0,
				&FromAddress,
				nFromSize);
			if (nSendBytes != nBytesReaded)
			{
				fprintf(stderr, "Cann't send all data\n");
			}
		}
	}

	// ��������� ��������� �����
	closesocket(S);
	// ����������� ������� ���������� �������
	WSACleanup();
	return 0;
}
