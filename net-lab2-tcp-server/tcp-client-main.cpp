/*
Простой TCP Эхо-сервер
Отсылает клиенту полученный от него же данные.

Файл: tcp_server.cpp

Пример компиляции:
bcc32 -otcp_server.exe tcp_server.cpp Ws2_32.lib
*/
#include <Winsock2.h>
#include <stdio.h>
#include <stdlib.h>

// TCP-порт сервера
#define SERVICE_PORT 1500 

//
int main(void)
{
	SOCKET  S;  //дескриптор прослушивающего сокета
	SOCKET  NS; //дескриптор присоединенного сокета

	sockaddr_in serv_addr;
	WSADATA     wsadata;
	char        sName[128];
	bool        bTerminate = false;

	// Инициализируем библиотеку сокетов
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	// Пытаемся получить имя текущей машины
	gethostname(sName, sizeof(sName));
	printf("\nServer host: %s\n", sName);

	// Создаем сокет
	// Для TCP-сокета указываем параметр SOCK_STREAM
	// Для UDP - SOCK_DGRAM 
	if ((S = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't create socket\n");
		exit(1);
	}


	//В API сокетов можно отключить алгоритм 
	//Нейгла с помощью установки опции сокета TCP_NODELAY.
	const int on = 1;
	if (setsockopt(S, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, sizeof(on)) != 0)
	{
		fprintf(stderr, "Can't set socket options\n");
		exit(1);
	}

	// Заполняем структуру адресов 
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	// Разрешаем работу на всех доступных сетевых интерфейсах,
	// в частности на localhost
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	// обратите внимание на преобразование порядка байт
	serv_addr.sin_port = htons((u_short)SERVICE_PORT);

	// Связываем сокет с заданным сетевым интерфесом и портом
	if (bind(S, (sockaddr*)&serv_addr, sizeof(serv_addr)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't bind\n");
		exit(1);
	}

	// Переводим сокет в режим прослушивания заданного порта
	// с максимальным количеством ожидания запросов на соединение 5
	if (listen(S, 5) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't listen\n");
		exit(1);
	}

	printf("Server listen on %s:%d\n",
		inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
	printf("Press Ctrl-C for terminate\n");

	// Основной цикл обработки подключения клиентов 
	while (!bTerminate)
	{
		printf("Wait for connections.....\n");

		sockaddr_in clnt_addr;
		int addrlen = sizeof(clnt_addr);
		memset(&clnt_addr, 0, sizeof(clnt_addr));

		// Переводим сервис в режим ожидания запроса на соединение.
		// Вызов синхронный, т.е. возвращает управление только при 
		// подключении клиента или ошибке 
		NS = accept(S, (sockaddr*)&clnt_addr, &addrlen);
		if (NS == INVALID_SOCKET)
		{
			fprintf(stderr, "Can't accept connection\n");
			break;
		}
		// Получаем параметры присоединенного сокета NS и
		// информацию о клиенте
		addrlen = sizeof(serv_addr);
		getsockname(NS, (sockaddr*)&serv_addr, &addrlen);
		// Функция inet_ntoa возвращает указатель на глобальный буффер, 
		// поэтому использовать ее в одном вызове printf не получится
		printf("Accepted connection on %s:%d ",
			inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
		printf("from client %s:%d\n",
			inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

		// Получаем и обрабатываем данные от клиента
		size_t nRetransmitBytesNum = 0;
		char sReceiveBuffer[65536] = { 0 };
		while (true)
		{
			int nReaded = recv(NS, sReceiveBuffer, sizeof(sReceiveBuffer), 0);
			// В случае ошибки (например, отсоединения клиента) выходим
			if (nReaded <= 0) break;
			// Отсылаем dct полученные только что данные обратно
			if (send(NS, sReceiveBuffer, nReaded, 0) < nReaded)
			{
				fprintf(stderr, "Cann't send all data\n");
			}
			nRetransmitBytesNum += nReaded;
		}

		// закрываем присоединенный сокет
		closesocket(NS);
		printf("Client disconnected.\n");
		printf("Retransmit %d bytes.\n", nRetransmitBytesNum);
	}
	// Закрываем серверный сокет
	closesocket(S);
	// освобождаем ресурсы библиотеки сокетов
	WSACleanup();
	return 0;
}
