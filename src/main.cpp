#include <QApplication>
#include "Lamp.h"
#include "TestServer.h"
#include <iostream>

void Usage()
{
	std::cout << "Usage" << std::endl;
	std::cout << "Lamp -s server ip address -p port" << std::endl;
}

int main ( int argc, char* argv[] )
{
	
	QString sHostAddress = "127.0.0.1";
	int iHostPort = 9999;
	if ( argc == 5 )
	{
		int iargc = 1;
		if ( strncmp ( argv[ iargc++ ], "-s", 2 ) == 0 )
		{
			sHostAddress = argv[ iargc++ ];
			if ( strncmp ( argv[ iargc++ ], "-p", 2 ) == 0 )
			{
				iHostPort = atoi ( argv[ iargc ] );
			}
			else
			{
				Usage();
			}
		}
		else
		{
			Usage();
		}
	}
	
	// здесь необходимо вставить дополнительную проверку ip адреса
	// или имени хоста и номера порта
	
	QApplication app ( argc, argv );
	// запуск тестового сервера, 
	// который посылает команду включения и несколько команд смены цвета
	TestServer server ( sHostAddress, iHostPort );
	server.show();

	Lamp lamp( sHostAddress, iHostPort );
	lamp.show();
		
	return app.exec();
}
