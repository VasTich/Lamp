#ifndef TEST_SERVER_H
#define TEST_SERVER_H

#include <QWidget>

class QTcpServer;
class QTextEdit;
class QTcpSocket;

class TestServer : public QWidget
{
	Q_OBJECT
private:
	QTcpServer* m_pTcpServer;
	QTextEdit* m_textEdit;
	
	void sendRandomCommand ( QTcpSocket* pSocket );

public:
	TestServer ( QString& sHostAddress, int iHostPort, QWidget* pwgt = 0 );
	virtual ~TestServer ();

public slots:
	virtual void slotNewConnection ();
	
};

#endif // TEST_SERVER_H