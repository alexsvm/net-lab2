/*
ѕростой UDP Ёхо-сервер
ќтсылает клиенту полученный от него же данные.

‘айл: tcp_server.cpp

ѕример компил€ции:
bcc32 -otcp_server.exe tcp_server.cpp Ws2_32.lib
*/
#include <Winsock2.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

// TCP-порт сервера
#define SERVICE_PORT 1500 

//
int main(void)
{
	setlocale(LC_ALL, "");
	char SERVERADDR[20];        //строка дл€ IP или NetBIOS-имени сервера
	int PORT;                   //переменна€ дл€ номера порта
	char s_port[20];            //строка дл€ номера порта
	SOCKET  S;  //дескриптор прослушивающего сокета
	printf("*** UDP CLIENT from ALEXEY GORELOV ***\n\n");

	sockaddr_in client, server;
	WSADATA     wsadata;
	char        sName[128];
	bool        bTerminate = false;

	// »нициализируем библиотеку сокетов
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	// ѕытаемс€ получить им€ текущей машины
	gethostname(sName, sizeof(sName));
	printf("Client host: %s\n", sName);

	// —оздаем UDP сокет
	// ƒл€ TCP-сокета указываем параметр SOCK_STREAM
	// ƒл€ UDP - SOCK_DGRAM 
	if ((S = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't create socket\n");
		exit(1);
	}

	// «аполн€ем структуру адресов 
	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	// –азрешаем работу на всех доступных сетевых интерфейсах,
	// в частности на localhost
	client.sin_addr.s_addr = INADDR_ANY;
	// обратите внимание на преобразование пор€дка байт
	client.sin_port = htons((u_short)SERVICE_PORT);

	// —в€зываем сокет с заданным сетевым интерфесом и портом
	if (bind(S, (sockaddr*)&client, sizeof(client)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't bind\n");
		exit(1);
	}

	printf("¬ведите IP-адрес или NetBIOS-им€ сервера: ");
	gets(SERVERADDR);                        //указываем IP или им€
	printf("¬ведите номер порта: ");
	gets(s_port);                            //указываем номер порта в строку
	PORT = atoi(s_port);                     //переводим строку в число int

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	HOSTENT *hst;

	// преобразование IP-адреса из символьного в сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		server.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
	{
		// попытка получить IP-адрес по доменному имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
			((unsigned long *)&server.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Ќеправильный адрес. %s\n", SERVERADDR);    //иначе - ошибка
			closesocket(S);
			WSACleanup();
			return -1;
		}
	}

	char str[10];
	int nsize;
	printf("”кажите размер передаваемых данных: ");
	gets(str);
	nsize = atoi(str);

	char sReceiveBuffer[nsize];     // буфер дл€ передачи
	for (int i = 0; i <= nsize; i++) sReceiveBuffer[i] = 'a';   //инициализируем буфер символом 'a'

	int start = GetTickCount();    //засечка системного времени старта

	size_t nRetransmitBytesNum = 0;         //общее число байт
	int nSendBytes = 0;
	int nBytesReaded = 0;                        //число байт "за один оборот"

												 // 4 - чтение и передача сообщений    
	while (((GetTickCount() / 1000) - (start / 1000)) <= 20)  // интервал обмена - 20 секунд
	{
		int nFromSize = sizeof(server);

		nSendBytes = sendto(S, sReceiveBuffer, nsize, 0, (sockaddr *)&server, nFromSize);
		nBytesReaded = recvfrom(S, sReceiveBuffer, nsize, 0, (sockaddr *)&server, &nFromSize);
		nRetransmitBytesNum += nBytesReaded;
	}

	printf("–азмер сообщени€: %d\n", nsize);
	printf(" оличество байт:  %d\n", nRetransmitBytesNum);
	printf("ѕропускна€ способность: %d байта в секунду", nRetransmitBytesNum / 20);

	getch();
	// «акрываем серверный сокет
	closesocket(S);
	// освобождаем ресурсы библиотеки сокетов
	WSACleanup();
	return 0;
}
