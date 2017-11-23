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

#define TSec 1 * 1000
#define BuffSize 1 << 15
	char buffer[BuffSize];				// ����� ��� ��������
	size_t nRetransmitBytesNum;     //����� ����� ����
	int nReaded;                    //����� ���� "�� ���� ������"
	int start, end;
	int BitPerSec;
	for (int nsize = 1; nsize <= (1 << 15); nsize = nsize << 1) {
		printf("������ ������: %d\n", nsize);
		nRetransmitBytesNum = 0;
		nReaded = 0;
		start = GetTickCount();    //������� ���������� ������� ������
		while ((GetTickCount() - start) <= TSec) // ������ � �������� ��������� 
		{
			send(my_sock, buffer, nsize, 0); // sending
			nReaded = recv(my_sock, buffer, nsize, 0); // recieving
			nRetransmitBytesNum += nReaded;
		}
		end = GetTickCount();
		BitPerSec = ((nRetransmitBytesNum * 8) / ((end - start) / 1000));
		printf("  ����:  %d\n", nRetransmitBytesNum);
		printf("  ���������� �����������: %d ���/���\n", BitPerSec );
	}


	getch();
	closesocket(my_sock);
	WSACleanup();
	return -1;
}
