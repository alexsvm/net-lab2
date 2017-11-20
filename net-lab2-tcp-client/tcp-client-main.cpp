// TCP-клиент
#include <iostream>
#include <stdio.h>
#include <conio.h>
//#include <string.h>
#include <winsock2.h>
//#include <windows.h>

//#define PORT 1500
//#define SERVERADDR "192.168.0.62" //IP-адрес

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	char SERVERADDR[20];        //строка для IP или NetBIOS-имени сервера
	int PORT;                   //переменная для номера порта
	char s_port[20];            //строка для номера порта
	char buff[1024];
	printf("*** TCP CLIENT from ALEXEY GORELOV ***\n\n");

	printf("Введите IP-адрес или NetBIOS-имя сервера: ");
	gets(SERVERADDR);                        //указываем IP или имя
	printf("Введите номер порта: ");
	gets(s_port);                            //указываем номер порта в строку
	PORT = atoi(s_port);                     //переводим строку в число int

											 // 1 - инициализация библиотеки Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// 2 - создание сокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);    //тип сокета - потоковый с предв. установкой соединения
	if (my_sock < 0)          //если socket() вернула -1 - ошибка
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// 3 - установка соединения
	// заполнение структуры sockaddr_in - указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// преобразование IP-адреса из символьного в сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
	{
		// попытка получить IP-адрес по доменному имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Неправильный адрес. %s\n", SERVERADDR);    //иначе - ошибка
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
	}

	// адрес сервера получен - пытаемся установить соединение
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("Ошибка подключения. %d\n", WSAGetLastError());
		return -1;
	}

	printf("Подключение к %s успешно установлено.\n\n", &SERVERADDR);

	char str[10];
	int nsize;
	printf("Укажите размер передаваемых данных: ");
	gets(str);
	nsize = atoi(str);

	char buffer[nsize];     // буфер для передачи
	for (int i = 0; i <= nsize; i++) buffer[i] = 'a';   //инициализируем буфер символом 'a'

	int start = GetTickCount();    //засечка системного времени старта

	size_t nRetransmitBytesNum = 0;         //общее число байт
	int nReaded = 0;                        //число байт "за один оборот"

											// 4 - чтение и передача сообщений    
	while (((GetTickCount() / 1000) - (start / 1000)) <= 20)  // интервал обмена - 20 секунд
	{
		send(my_sock, buffer, nsize, 0);                // отправили 
		nReaded = recv(my_sock, buffer, nsize, 0);      // получили
		nRetransmitBytesNum += nReaded;
	}

	printf("Размер сообщения: %d\n", nsize);
	printf("Количество байт:  %d\n", nRetransmitBytesNum);
	printf("Пропускная способность: %d байта в секунду", nRetransmitBytesNum / 20);

	getch();
	closesocket(my_sock);
	WSACleanup();
	return -1;
}
