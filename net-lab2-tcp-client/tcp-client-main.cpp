// TCP-клиент
#include <iostream>
#include <stdio.h>
#include <conio.h>
//#include <string.h>
#include <winsock2.h>
//#include <windows.h>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	char SERVERADDR[20];        //строка дл€ IP или NetBIOS-имени сервера
	int PORT;                   //переменна€ дл€ номера порта
	char s_port[20];            //строка дл€ номера порта
	char buff[1024];
	printf("=== simple TCP client ===\n\n");

	printf("¬ведите IP-адрес или NetBIOS-им€ сервера: ");
	gets_s(SERVERADDR, 20);                        //указываем IP или им€
	printf("¬ведите номер порта: ");
	gets_s(s_port, 20);                            //указываем номер порта в строку
	PORT = atoi(s_port);                     //переводим строку в число int

	// 1 - инициализаци€ библиотеки Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// 2 - создание сокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);    //тип сокета - потоковый с предв. установкой соединени€
	if (my_sock < 0)          //если socket() вернула -1 - ошибка
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// ”становка соединени€. «аполнение структуры sockaddr_in - указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// ѕреобразование IP-адреса из символьного в сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
	{
		// ѕопытка получить IP-адрес по доменному имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Ќеправильный адрес. %s\n", SERVERADDR);    //иначе - ошибка
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
	}

	// јдрес сервера получен - пытаемс€ установить соединение
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("ќшибка подключени€. %d\n", WSAGetLastError());
		return -1;
	}

	printf("ѕодключение к %s успешно установлено.\n\n", &SERVERADDR);

#define TSec 1 * 1000
#define BuffSize 1 << 15
	char buffer[BuffSize];				// буфер дл€ передачи
	size_t nRetransmitBytesNum;     //общее число байт
	int nReaded;                    //число байт "за один оборот"
	int start, end;
	int BitPerSec;
	for (int nsize = 1; nsize <= (1 << 15); nsize = nsize << 1) {
		printf("–азмер пакета: %d\n", nsize);
		nRetransmitBytesNum = 0;
		nReaded = 0;
		start = GetTickCount();    //засечка системного времени старта
		while ((GetTickCount() - start) <= TSec) // „тение и передача сообщений 
		{
			send(my_sock, buffer, nsize, 0); // sending
			nReaded = recv(my_sock, buffer, nsize, 0); // recieving
			nRetransmitBytesNum += nReaded;
		}
		end = GetTickCount();
		BitPerSec = ((nRetransmitBytesNum * 8) / ((end - start) / 1000));
		printf("  Ѕайт:  %d\n", nRetransmitBytesNum);
		printf("  ѕропускна€ способность: %d бит/сек\n", BitPerSec );
	}


	getch();
	closesocket(my_sock);
	WSACleanup();
	return -1;
}
