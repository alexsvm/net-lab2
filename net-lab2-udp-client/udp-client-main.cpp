/*
������� UDP ���-������
�������� ������� ���������� �� ���� �� ������.

����: tcp_server.cpp

������ ����������:
bcc32 -otcp_server.exe tcp_server.cpp Ws2_32.lib
*/
#include <Winsock2.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

// TCP-���� �������
#define SERVICE_PORT 1500 

//
int main(void)
{
	setlocale(LC_ALL, "");
	char SERVERADDR[20];        //������ ��� IP ��� NetBIOS-����� �������
	int PORT;                   //���������� ��� ������ �����
	char s_port[20];            //������ ��� ������ �����
	SOCKET  S;  //���������� ��������������� ������
	printf("=== simple UDP client ===\n\n");

	sockaddr_in client, server;
	WSADATA     wsadata;
	char        sName[128];
	bool        bTerminate = false;

	// �������������� ���������� �������
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	// �������� �������� ��� ������� ������
	gethostname(sName, sizeof(sName));
	printf("Client host: %s\n", sName);

	// ������� UDP �����
	// ��� TCP-������ ��������� �������� SOCK_STREAM
	// ��� UDP - SOCK_DGRAM 
	if ((S = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't create socket\n");
		exit(1);
	}

	// ��������� ��������� ������� 
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	// ��������� ������ �� ���� ��������� ������� �����������,
	// � ��������� �� localhost
	client.sin_addr.s_addr = INADDR_ANY;
	// �������� �������� �� �������������� ������� ����
	client.sin_port = htons((u_short)SERVICE_PORT);

	// ��������� ����� � �������� ������� ���������� � ������
	if (bind(S, (sockaddr*)&client, sizeof(client)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't bind\n");
		exit(1);
	}

	printf("������� IP-����� ��� NetBIOS-��� �������: ");
	gets_s(SERVERADDR, 20);                        //��������� IP ��� ���
	printf("������� ����� �����: ");
	gets_s(s_port, 20);                            //��������� ����� ����� � ������
	PORT = atoi(s_port);                     //��������� ������ � ����� int

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	HOSTENT *hst;

	// �������������� IP-������ �� ����������� � ������� ������
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		server.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
	{
		// ������� �������� IP-����� �� ��������� ����� �������
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list �������� �� ������ �������,
			// � ������ ���������� �� ������
			((unsigned long *)&server.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("������������ �����. %s\n", SERVERADDR);    //����� - ������
			closesocket(S);
			WSACleanup();
			return -1;
		}
	}


#define TSec 1 * 1000
#define BuffSize 1 << 15
	char sReceiveBuffer[BuffSize];     // ����� ��� ��������
	size_t nRetransmitBytesNum;     //����� ����� ����
	int nSendBytes = 0;
	int nBytesReaded = 0;                        //����� ���� "�� ���� ������"
	//int nReaded;                    //����� ���� "�� ���� ������"
	int start, end;
	int BitPerSec;
	for (int nsize = 1; nsize <= (1 << 15); nsize = nsize << 1) {
		printf("������ ������: %d\n", nsize);
		nRetransmitBytesNum = 0;
		nBytesReaded = 0;
		int nFromSize = sizeof(server);

		start = GetTickCount();    //������� ���������� ������� ������
		while ((GetTickCount() - start) <= TSec) // ������ � �������� ��������� 
		{
			nSendBytes = sendto(S, sReceiveBuffer, nsize, 0, (sockaddr *)&server, nFromSize);
			nBytesReaded = recvfrom(S, sReceiveBuffer, nsize, 0, (sockaddr *)&server, &nFromSize);
			nRetransmitBytesNum += nBytesReaded;
		}
		end = GetTickCount();
		BitPerSec = ((nRetransmitBytesNum * 8) / ((end - start) / 1000));
		printf("  ����:  %d\n", nRetransmitBytesNum);
		printf("  ���������� �����������: %d ���/���\n", BitPerSec);
	}

	getch();
	// ��������� ��������� �����
	closesocket(S);
	// ����������� ������� ���������� �������
	WSACleanup();
	return 0;
}
