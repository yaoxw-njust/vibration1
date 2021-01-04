
#include "tcp_logic.h"
#include "import_dll_class.h"


#pragma region TcpServer 服务器部分

TcpServer::TcpServer(): QObject()
{	
	m_pLogHelper = ImportDllClass::GetInstance()->m_pLogHelper;
	
}

TcpServer::~TcpServer()
{
}

bool TcpServer::initialParam(int port)
{
	m_pTcpSever = new QTcpServer();
	m_nListenPort = port;
	if (!m_pTcpSever->listen(QHostAddress::Any, m_nListenPort))
	{
		m_pLogHelper->writeError("TcpServerError", QStringLiteral("服务器端监听失败！"));
		return false;
	}
	connect(m_pTcpSever, SIGNAL(newConnection()), this, SLOT(onServerNewConnection()));
	
	return true;
}

void TcpServer::onServerNewConnection()
{
	QTcpSocket *pTcpSocket = m_pTcpSever->nextPendingConnection();
	if (!pTcpSocket)
	{
		m_pLogHelper->writeError("TcpServerError", QStringLiteral("未正确获取客户端连接！"));
		return;
	}
	TcpClientSocket *m_TcpClientSocket = new TcpClientSocket(pTcpSocket);
	connect(m_TcpClientSocket, SIGNAL(pushSocketData(QByteArray)), this, SIGNAL(pushSocketData(QByteArray)));
	m_pQListSocket.append(pTcpSocket);
}

bool TcpServer::onSendData(QString msg, int numberDecimal)
{
	for (size_t i = 0; i < m_pQListSocket.count(); i++)
	{	
		if (msg.isEmpty())
		{
			m_pLogHelper->writeError("TcpServerError", QString::fromLocal8Bit("[%d]发送数据为空，请输入数据").arg(i));
			return false;
		}
		if (m_pQListSocket[i]->isValid())
		{
			QByteArray sendData;
			switch (numberDecimal)
			{
			case 10:
				sendData = msg.toUtf8();
				break;
			case 16:
				sendData = QString2Hex(msg);
				break;
			}
			int sendRe = m_pQListSocket[i]->write(sendData);
			if (-1 == sendRe)
			{
				m_pQListSocket.removeAt(i);
				i--;
				m_pLogHelper->writeError("TcpServerError", QString::fromLocal8Bit("[%d]服务端发送数据失败！").arg(i));
			}
		}
		else
		{
			m_pLogHelper->writeError("TcpServerError", QString::fromLocal8Bit("[%d]套接字无效！").arg(i));
		}
	}
	return true;
}

QByteArray TcpServer::QString2Hex(QString str)
{
	QByteArray senddata;
	int hexdata, lowhexdata;
	int hexdatalen = 0;
	int len = str.length();
	senddata.resize(len / 2);
	char lstr, hstr;
	for (int i = 0; i<len; )
	{
		hstr = str[i].toLatin1();
		if (hstr == ' ')
		{
			i++;
			continue;
		}
		i++;
		if (i >= len)
			break;
		lstr = str[i].toLatin1();
		hexdata = ConvertHexChar(hstr);
		lowhexdata = ConvertHexChar(lstr);
		if ((hexdata == 16) || (lowhexdata == 16))
			break;
		else
			hexdata = hexdata * 16 + lowhexdata;
		i++;
		senddata[hexdatalen] = (char)hexdata;
		hexdatalen++;
	}
	senddata.resize(hexdatalen);

	return senddata;
}

char TcpServer::ConvertHexChar(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - 0x30;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else return (-1);
}

#pragma endregion

#pragma region TcpClient 客户端部分

TcpClient::TcpClient() : QObject()
{
	m_pLogHelper = ImportDllClass::GetInstance()->m_pLogHelper;
	m_pDisconnectTimer = new QTimer();
	connect(m_pDisconnectTimer, SIGNAL(timeout()), this, SLOT(onReconnectServer()));
	m_pHeartTimer = new QTimer();
	connect(m_pHeartTimer, SIGNAL(timeout()), this, SLOT(onHeartBeat()));
	m_pConnectTimer = new QTimer();
	connect(m_pConnectTimer, SIGNAL(timeout()), this, SLOT(onConnectServer()));

}

TcpClient::~TcpClient()
{
}

bool TcpClient::initialParam(QString strServerIP, int nServerPort)
{
	m_strServerIP = strServerIP;
	m_nServerPort = nServerPort;
	m_pTcpSoket = new QTcpSocket();
	m_pConnectTimer->start(500);
	return true;
}

void TcpClient::onClientRecvData()
{
	QByteArray recvBytes = m_pTcpSoket->readAll();
	if (recvBytes.size()>0)
	{
		pushSocketData(recvBytes);

	}
	else
	{
		m_pLogHelper->writeError("TcpClientError", QStringLiteral("接收服务端数据失败！"));
	}
	
}

bool TcpClient::onSendData(QString msg, int numberDecimal)
{
	QByteArray sendData;
	switch (numberDecimal)
	{
	case 10:
		sendData = msg.toUtf8();
		break;
	case 16:
		sendData = QString2Hex(msg);
		break;
	}
	int sendRe = m_pTcpSoket->write(sendData);
	if (sendRe == -1)
	{
		m_pLogHelper->writeError("TcpClientError", QStringLiteral("向服务端发送数据失败！"));
		return false;
	}
	return true;
}

void TcpClient::onReconnectServer()
{
	m_pTcpSoket->connectToHost(m_strServerIP, m_nServerPort);
	if (m_pTcpSoket->waitForConnected(100))
	{
		emit sendStateMsg(QStringLiteral("服务器重新连接成功"), 0);
		m_pDisconnectTimer->stop();
	}
}

void TcpClient::onServerDisConnection()
{
	emit sendStateMsg(QStringLiteral("服务器连接掉线"), 1);
	m_pTcpSoket->disconnectFromHost();
	m_pDisconnectTimer->start(1000);
}

void TcpClient::onHeartBeat()
{
	char sendMsgChar[2] = { 0x2F, 0x2A };
	int sendRe = m_pTcpSoket->write(sendMsgChar, 2);
}

void TcpClient::onConnectServer()
{
	m_pTcpSoket->connectToHost(m_strServerIP, m_nServerPort);
	if (m_pTcpSoket->waitForConnected(100))
	{
		emit sendStateMsg(QStringLiteral("服务器连接成功"), 0);
		connect(m_pTcpSoket, SIGNAL(readyRead()), this, SLOT(onClientRecvData()));
		connect(m_pTcpSoket, SIGNAL(disconnected()), this, SLOT(onServerDisConnection()));
		m_pConnectTimer->stop();
		m_pHeartTimer->start(1000);
	}
}

QByteArray TcpClient::QString2Hex(QString str)
{
	QByteArray senddata;
	int hexdata, lowhexdata;
	int hexdatalen = 0;
	int len = str.length();
	senddata.resize(len / 2);
	char lstr, hstr;
	for (int i = 0; i<len; )
	{
		hstr = str[i].toLatin1();
		if (hstr == ' ')
		{
			i++;
			continue;
		}
		i++;
		if (i >= len)
			break;
		lstr = str[i].toLatin1();
		hexdata = ConvertHexChar(hstr);
		lowhexdata = ConvertHexChar(lstr);
		if ((hexdata == 16) || (lowhexdata == 16))
			break;
		else
			hexdata = hexdata * 16 + lowhexdata;
		i++;
		senddata[hexdatalen] = (char)hexdata;
		hexdatalen++;
	}
	senddata.resize(hexdatalen);

	return senddata;
}

char TcpClient::ConvertHexChar(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - 0x30;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else return (-1);
}

#pragma endregion

#pragma region TcpClientSocket 为服务器分配多个客户端的连接

TcpClientSocket::TcpClientSocket(QTcpSocket *m_pSocket) : QObject()
{
	m_pLogHelper = ImportDllClass::GetInstance()->m_pLogHelper;
	m_pTcpSocket = m_pSocket;
	connect(m_pTcpSocket, SIGNAL(readyRead()), this, SLOT(onServerReadData()));
	connect(m_pTcpSocket, SIGNAL(disconnected()), this, SLOT(onServerDisConnection()));
}

TcpClientSocket::~TcpClientSocket()
{
}

void TcpClientSocket::onServerReadData()
{
	QByteArray recvBytes = m_pTcpSocket->readAll();
	if (recvBytes.size() > 0)
	{
		emit pushSocketData(recvBytes);
	}
	else
	{
		m_pLogHelper->writeError("TcpServerError", QStringLiteral("未正确接受到数据"));
	}
}

void TcpClientSocket::onServerDisConnection()
{
	m_pLogHelper->writeError("TcpServerError", QStringLiteral("与客户端的连接断开！"));
}

#pragma endregion
