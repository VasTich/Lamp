#include "Lamp.h"
#include <QtGui>
#include <QTcpSocket>
#include <QDataStream>
#include <QMessageBox>
#include <unistd.h>

Command::Command () : m_commandType ( START_COMMAND ), m_lenght ( 0 ), m_value ( NULL ),
m_waitingMessageType ( COMMAND_MESSAGE ), m_currentDataPos ( 0 )
 {

 } // Command


Command::~Command ()
{
	if ( m_value )
	{
		delete[] m_value;
		m_value = NULL;

	}

} // ~Command


void Command::setType ( unsigned char type )
{
	m_commandType = type;
	m_waitingMessageType = LENGHT_MESSAGE;

} // setType


void Command::setLenght ( unsigned char length )
{
		m_lenght = length;
		if ( m_lenght > 0 )
		{
			m_value = new unsigned char[ m_lenght ];
			m_waitingMessageType = VALUE_MESSAGE;
		}
		else
		{
			m_waitingMessageType = COMMAND_MESSAGE;
		}

} // setLenght


void Command::reset ()
{
	m_commandType = START_COMMAND;
	m_lenght = 0;
	if ( m_value )
	{
		delete[] m_value;
		m_value = NULL;
	}

	m_waitingMessageType  = COMMAND_MESSAGE;
	m_currentDataPos = 0;

} // reset


bool Command::isFull ()
{
	bool bRes = false;
	if ( m_waitingMessageType == COMMAND_MESSAGE && 
		 m_commandType > START_COMMAND 
		 && m_currentDataPos == 0 )
	{
		bRes = true;
	}

	return bRes;

} // isFull


void Command::addData( unsigned char data )
{
	if ( m_value && m_currentDataPos < m_lenght )
	{
		m_value[ m_currentDataPos++ ] = data;
	}

	if ( m_currentDataPos == m_lenght )
	{
		m_waitingMessageType = COMMAND_MESSAGE;
		m_currentDataPos = 0;
	}
	else
	{
		m_waitingMessageType = VALUE_MESSAGE;
	}

} // addData


Lamp::Lamp (  QString& sHostAddress, int iHostPort, QWidget* parent ) : QWidget ( parent )
{
	isOn = false;
	m_onColor = Qt::yellow;
	m_brush = QBrush ( Qt::NoBrush );
	m_penWidth = 3;
	m_pen = QPen ( Qt::black, m_penWidth, Qt::SolidLine );
	m_lampSize = QRect ( 0, 0, 100, 100 );

	m_pTcpSocket = new QTcpSocket ( this );
	m_pTcpSocket->connectToHost ( sHostAddress, iHostPort );
	connect ( m_pTcpSocket, SIGNAL ( connected () ), SLOT ( slotConnected () ) ) ;
	connect ( m_pTcpSocket, SIGNAL ( readyRead () ), SLOT ( slotReadyRead () ) );
	connect ( m_pTcpSocket, SIGNAL ( error ( QAbstractSocket::SocketError ) ), this, SLOT ( slotError ( QAbstractSocket::SocketError ) ) );
	connect ( m_pTcpSocket, SIGNAL ( disconnected() ), m_pTcpSocket, SLOT ( deleteLater () ) );

} // Lamp

Lamp::~Lamp()
{
} // ~Lamp


QSize Lamp::minimumSizeHint () const
{
	return QSize ( 100, 100 );

} // minimumSizeHint


QSize Lamp::sizeHint () const
{
	return QSize ( 300, 300 );

} // sizeHint


void Lamp::setBrush ( const QBrush &brush )
{
	this->m_brush = brush;

} // setBrush


void Lamp::setPen ( const QPen &pen )
{
	this->m_pen = pen;

} // setPen


void Lamp::paintEvent ( QPaintEvent *event )
{
	QWidget::paintEvent ( event );
	this->drawLamp ();

} // paintEvent


void Lamp::drawLamp ()
{
	QPainter painter ( this );
	painter.setPen ( m_pen );
	painter.setBrush ( m_brush );
	painter.setRenderHint ( QPainter::Antialiasing, true );
	painter.drawEllipse ( m_lampSize );

} // drawLamp


void Lamp::resizeEvent ( QResizeEvent *event )
{
	QWidget::resizeEvent ( event );
	m_lampSize.setHeight ( this->size().height () );
	m_lampSize.setWidth ( this->size().width () );

} // resizeEvent


void Lamp::slotReadyRead ()
{
	// анализ данных происходит побайтово
	// если пришел тип дальше ожидаем поле "длина"
	// если поле "длина" больше 0, ожидаем данные
	// когда команда собрана, запускается ее выполнение
	if ( m_pTcpSocket && m_pTcpSocket->bytesAvailable() )
	{ 
		QByteArray bytes = m_pTcpSocket->readAll ();
		for ( int idx = 0; idx < bytes.count (); ++idx )
		{
			// QByteArray вроде как по умолчанию BigEndian
			unsigned char data = bytes[ idx ];
			MessageType messType = m_command.getMessageType ();
			switch ( messType )
			{
				case COMMAND_MESSAGE:
					if ( isMyCommand ( data )  )
					{
						m_command.setType ( data );
					}
					break;
				case LENGHT_MESSAGE:
					m_command.setLenght ( data );
					break;
				case VALUE_MESSAGE:
					m_command.addData ( data );

			} // switch ( messType )

			if ( m_command.isFull() )
			{
				executeCommand();
				m_command.reset();
			}

		} // for ( int idx

	} // if ( m_pTcpSocket

} // slotReadyRead


void Lamp::slotConnected ()
{
} // slotConnected


void Lamp::slotError ( QAbstractSocket::SocketError socketError )
{
	switch ( socketError )
	{
		case QAbstractSocket::RemoteHostClosedError:
			break;
		case QAbstractSocket::HostNotFoundError:
			QMessageBox::information ( this, tr ( "Lamp" ), tr ( "The host was not found" ) );
			break;
		default:
			QMessageBox::information ( this, tr ( "Lamp" ), tr ( "The following error occured: %1." ).arg ( m_pTcpSocket->errorString () ) );
	}

} // slotError


bool Lamp::isMyCommand ( char type )
{
	bool bRes = false;
	if ( ( type > START_COMMAND && type < FINISH_COMMAND ) || type == CHANGE_COLOR_COMMAND )
	{
		bRes = true;
	}

	return bRes;

} // isMyCommand

bool Lamp::executeCommand ()
{
	bool bRes = false;
	char cTypeCommand = m_command.getType ();
	if ( isMyCommand ( cTypeCommand ) )
	{
		switch ( cTypeCommand )
		{
			case ON_COMMAND:
				on ();
				break;
			case OFF_COMMAND:
				off ();
				break;
			case CHANGE_COLOR_COMMAND:
				changeColor ();
				break;

		} // switch ( cTypeCommand )

	} // f ( isMyCommand

} // executeCommand


bool Lamp::on ()
{
	bool bRes = false;
	if ( !isOn )
	{
		this->setBrush ( QBrush ( m_onColor ) );
		update();
		bRes = true;
		isOn = true;
	}
	
	return bRes;

} // on


bool Lamp::off ()
{
	bool bRes = false;
	if ( isOn )
	{
		this->setBrush ( QBrush ( Qt::NoBrush ) );
		update();
		bRes = true;
		isOn = false;
	}
	
	return bRes;

} // off


bool Lamp::changeColor ()
{
	bool bRes = false;
	if ( isOn )
	{
		unsigned const char* rgb = m_command.getValue ();
		if ( m_command.getLenght() == 3 && rgb )
		{
			this->setBrush( QBrush ( QColor ( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ] ) ) );
			update();
			bRes = true;
			// QMessageBox::information ( this, tr ( "Lamp" ), tr ( "Change Color" ) );
		}
		
	} // if ( isOn )

	return bRes;

} // changeColor
