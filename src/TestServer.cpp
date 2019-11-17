#include "TestServer.h"
#include <QTcpServer>
#include <QTextEdit>
#include <QBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTcpSocket>
#include <Lamp.h>
#include <unistd.h>

TestServer::TestServer ( QString& sHostAddress, int iHostPort, QWidget* pwgt ) : QWidget ( pwgt )
{
	m_pTcpServer = new QTcpServer ( this );
	if ( !m_pTcpServer->listen ( QHostAddress ( sHostAddress ), iHostPort ) )
	{
		QMessageBox::critical ( 0, "Server Error", "Unable to start the server: " + m_pTcpServer->errorString() );
		m_pTcpServer->close ();
		return;
	}
	
	connect ( m_pTcpServer, SIGNAL ( newConnection () ), this, SLOT ( slotNewConnection () ) );
	m_textEdit = new QTextEdit;
	m_textEdit->setReadOnly ( true );
	
	QVBoxLayout* pvbxLayout = new QVBoxLayout;
	pvbxLayout->addWidget ( new QLabel ( "Server" ) );
	pvbxLayout->addWidget ( m_textEdit );
	setLayout ( pvbxLayout );
	
} // TestServer

TestServer::~TestServer()
{
}

void TestServer::slotNewConnection ()
{
	QTcpSocket* pClientSocket = m_pTcpServer->nextPendingConnection();
	connect ( pClientSocket, SIGNAL ( disconnected() ), pClientSocket, SLOT ( deleteLater () ) );
	m_textEdit->append ( "Client connected" );
	QString data = "Ip " + m_pTcpServer->serverAddress().toString();
	m_textEdit->append ( data );
	data = "Port " + QString::number( m_pTcpServer->serverPort() );
	m_textEdit->append ( data );
	sendRandomCommand ( pClientSocket );
	
} // slotNewConnection


void TestServer::sendRandomCommand ( QTcpSocket* pSocket )
{
	QByteArray data;
	unsigned char type = ON_COMMAND;
	data.push_back ( type );
	data.push_back ( (unsigned char)0 );
	pSocket->write ( data );
		
	for ( int i = 0; i < 10; ++i )
	{
		data.clear();
		type = CHANGE_COLOR_COMMAND;
		data.push_back ( type );
		data.push_back ( (unsigned char)3 );
		data.push_back ( (unsigned char) (qrand() % 255 )  );
		data.push_back ( (unsigned char)  (qrand() % 255 )  );
		data.push_back ( (unsigned char) (qrand() % 255 )  );
		pSocket->write ( data );
	}
	
	data.clear();
	// type = OFF_COMMAND;
	// data.push_back( type );
	// data.push_back ( (unsigned char)0 );
	// pSocket->write ( data );
	
} // sendRandomCommand