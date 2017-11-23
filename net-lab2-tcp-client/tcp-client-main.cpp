// TCP-������
#include <iostream>
#include <stdio.h>
#include <conio.h>
//#include <string.h>
#include <winsock2.h>
//#include <windows.h>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	char SERVERADDR[20];        //������ ��� IP ��� NetBIOS-����� �������
	int PORT;                   //���������� ��� ������ �����
	char s_port[20];            //������ ��� ������ �����
	char buff[1024];
	printf("=== simple TCP client ===\n\n");

	printf("������� IP-����� ��� NetBIOS-��� �������: ");
	gets_s(SERVERADDR, 20);                        //��������� IP ��� ���
	printf("������� ����� �����: ");
	gets_s(s_port, 20);                            //��������� ����� ����� � ������
	PORT = atoi(s_port);                     //��������� ������ � ����� int

	// 1 - ������������� ���������� Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// 2 - �������� ������
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);    //��� ������ - ��������� � �����. ���������� ����������
	if (my_sock < 0)          //���� socket() ������� -1 - ������
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// ��������� ����������. ���������� ��������� sockaddr_in - �������� ������ � ����� �������
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// �������������� IP-������ �� ����������� � ������� ������
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
	{
		// ������� �������� IP-����� �� ��������� ����� �������
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list �������� �� ������ �������,
			// � ������ ���������� �� ������
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("������������ �����. %s\n", SERVERADDR);    //����� - ������
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
	}

	// ����� ������� ������� - �������� ���������� ����������
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("������ �����������. %d\n", WSAGetLastError());
		return -1;
	}

	printf("����������� � %s ������� �����������.\n\n", &SERVERADDR);

	char str[10];
	int nsize;
	printf("������� ������ ������������ ������: ");
	gets_s(str, 10);
	nsize = atoi(str);
#define NSIZE 32768
	char buffer[NSIZE];     // ����� ��� ��������
	for (int i = 0; i <= nsize; i++) buffer[i] = 'a';   //�������������� ����� �������� 'a'

	int start = GetTickCount();    //������� ���������� ������� ������

	size_t nRetransmitBytesNum = 0;         //����� ����� ����
	int nReaded = 0;                        //����� ���� "�� ���� ������"

	// ������ � �������� ���������    
	while (((GetTickCount() / 1000) - (start / 1000)) <= 20)  // 20 ������
	{
		send(my_sock, buffer, nsize, 0); // sending
		nReaded = recv(my_sock, buffer, nsize, 0); // recieving
		nRetransmitBytesNum += nReaded;
	}

	printf("������ ���������: %d\n", nsize);
	printf("����:  %d\n", nRetransmitBytesNum);
	printf("���������� �����������: %d ����� � �������", nRetransmitBytesNum / 20);

	getch();
	closesocket(my_sock);
	WSACleanup();
	return -1;
}
