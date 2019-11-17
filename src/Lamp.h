#ifndef LAMP_H
#define LAMP_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QTcpSocket>

//! Типы команд фонаря
enum CommandType
{
	START_COMMAND = 0x11,
	ON_COMMAND = 0x12,
	OFF_COMMAND = 0x13,
	FINISH_COMMAND = 0x14,
	CHANGE_COLOR_COMMAND = 0x20
};

//! Тип сообщения
enum MessageType
{
	COMMAND_MESSAGE = 0,
	LENGHT_MESSAGE,
	VALUE_MESSAGE
};

//!	@brief	Класс для представления команды
class Command
{
public:
	Command ();
	~Command ();
	//!	@brief	Получить тип команды
	unsigned char getType () { return m_commandType; };
	//!	@brief	Получить длину команды
	unsigned char getLenght () { return m_lenght; };
	//!	@brief	Получить указатель на данные команды
	unsigned const char* getValue () { return m_value; };
	//!	@brief	Получить тип сообщения, которое ожидается для последущего приема
	MessageType getMessageType ()  { return m_waitingMessageType; };
	//!	@brief	Установить тип команды
	void setType ( unsigned char type );
	//!	@brief	Установить длину команды
	void setLenght ( unsigned char length );	
	//!	@brief	Сбросить все параметры
	void reset ();
	//!	@brief	Проверить все ли данные для команды заданы
	bool isFull ();
	//!	@brief	Добавить данные в m_value
	void addData ( unsigned char data );

protected:
	//!	тип команды
	unsigned char m_commandType;
	//!	длина
	unsigned char m_lenght;
	//!	значение
	unsigned char* m_value;
	//!	ожидаемый тип сообщения
	MessageType m_waitingMessageType;
	//!	текущая позиция байта в m_value
	int m_currentDataPos;
};


//!	класс для представления фонаря
class Lamp : public QWidget
{
	Q_OBJECT

public:
	Lamp (  QString& sHostAddress, int iHostPort, QWidget* parent = 0 );
	virtual ~Lamp();
	
	QSize minimumSizeHint () const;
	QSize sizeHint () const;
	
protected:

	//!	перо для рисования
	QPen m_pen;
	//!	толщина линии
	int m_penWidth;
	//!	текущая кисть
	QBrush m_brush;
	//!	размер фонаря
	QRect m_lampSize;
	
	//!	признак включен ли фонарь
	bool isOn;
	//!	цвет по умолчанию при включении
	QColor m_onColor;
	//!	сокет
	QTcpSocket* m_pTcpSocket;
	//!	текущая выполняемая команда
	Command m_command;
	
	//!	@brief	Установить кисть
	void setBrush ( const QBrush &brush );
	//!	@brief	Установить перо
	void setPen ( const QPen &pen );
	
	virtual void paintEvent ( QPaintEvent *event );
	virtual void resizeEvent ( QResizeEvent *event );
	
	//!	@brief Нарисовать фонарь
	void drawLamp ();
	//!	@brief Проверить является ли команда командой фонаря
	bool isMyCommand ( char type );
	//!	@brief	Выполнить команду
	bool executeCommand ();
	//!	@brief	Включить фонарь
	bool on ();
	//!	@brief	Выключить фонарь
	bool off ();
	//!	@brief	Сменить цвет
	bool changeColor ();
	
private slots:
	void slotReadyRead ();
	void slotError ( QAbstractSocket::SocketError );
	void slotConnected();
};

#endif // LAMP_H