#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include "..\DealFile\log_helper.h"


class TcpServer : public QObject
{
	Q_OBJECT

public:
	TcpServer();
	~TcpServer();

	bool initialParam(int port);
	bool onSendData(QString msg, int numberDecimal=10);

private:
	QByteArray QString2Hex(QString str);
	char ConvertHexChar(char ch);

	private slots:
	void onServerNewConnection();

signals:
	void pushSocketData(QByteArray bytes);
	void sendStateMsg(QString msg, int flag);

private:
	QTcpServer *m_pTcpSever;
	LogHelper *m_pLogHelper;
	int m_nListenPort;

public:
	QList<QTcpSocket *> m_pQListSocket;

};

class TcpClient : public QObject
{
	Q_OBJECT

public:
	TcpClient();
	~TcpClient();

	bool initialParam(QString strServerIP, int nServerPort);
	bool onSendData(QString msg, int numberDecimal=10);

private:
	QByteArray QString2Hex(QString str);
	char ConvertHexChar(char ch);

	private slots:
	void onClientRecvData();
	void onReconnectServer();
	void onServerDisConnection();
	void onHeartBeat();
	void onConnectServer();

signals:
	void pushSocketData(QByteArray bytes);
	void sendStateMsg(QString msg, int flag);

private:

	QTcpSocket *m_pTcpSoket;
	QString m_strServerIP;
	int m_nServerPort;
	LogHelper *m_pLogHelper;

	QTimer *m_pHeartTimer;
	QTimer *m_pDisconnectTimer;
	QTimer *m_pConnectTimer;
};

class TcpClientSocket : public QObject
{
	Q_OBJECT
public:
	TcpClientSocket(QTcpSocket *m_pSocket);
	~TcpClientSocket();

	private slots:
	void onServerReadData();
	void onServerDisConnection();

signals:
	void pushSocketData(QByteArray bytes);

private:
	QTcpSocket *m_pTcpSocket;
	LogHelper *m_pLogHelper;
};
