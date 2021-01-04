#include "main_logic_tcp.h"
#include "..\DealFile\ini_helper.h"
#include "import_dll_class.h"
#include "..\DealCommon\path_define_file.h"
#include "..\DealFile\mysql_helper.h"
#include "..\DealAlgorithm\algorithm_base.h"

#include <QThread>
#include <QtConcurrent\QtConcurrent>


MainLogicTcp::MainLogicTcp(QObject *parent): QObject(parent)
{
	m_pAlgorithmBase = ImportDllClass::GetInstance()->m_pAlgorithmBase;
	connect(m_pAlgorithmBase, SIGNAL(algorithmSendMsg(QString , int)), this, SIGNAL(mainLogicSendMsg(QString, int)));
	connect(m_pAlgorithmBase, SIGNAL(sendVibrationData()), this, SIGNAL(sendVibrationData()));
	connect(m_pAlgorithmBase, SIGNAL(sendAxleData()),this, SIGNAL(sendAxleData()));
	connect(m_pAlgorithmBase, SIGNAL(sendElectricalData()), this, SIGNAL(sendElectricalData()));

}

MainLogicTcp::~MainLogicTcp()
{
	if (NULL != m_pTcpServer)
	{
		delete m_pTcpServer;
	}
	m_pTcpServer = NULL;

}

void MainLogicTcp::initialConnection()
{
	try
	{
		IniHelper *inihelper = ImportDllClass::GetInstance()->m_pIniHelper;
		QString m_strLocalIP = inihelper->readIniStr("system", "local_ip", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
		emit mainLogicSendMsg(QStringLiteral("本地IP：") + m_strLocalIP, 0);
		int m_nLocalPort = inihelper->readIniInt("system", "local_port", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
		emit mainLogicSendMsg(QStringLiteral("本地端口号：") + QString::number(m_nLocalPort), 0);

		m_pTcpServer = new TcpServer();
		m_pTcpServer->initialParam(m_nLocalPort);

		emit mainLogicSendMsg(QStringLiteral("通讯模块设置成功"), 0);
		connect(m_pTcpServer, SIGNAL(pushSocketData(QByteArray)), this, SLOT(mainLogicRun(QByteArray)));
		connect(m_pTcpServer, SIGNAL(sendStateMsg(QString, int)), this, SIGNAL(mainLogicSendMsg(QString, int)));

		m_pAlgorithmBase->initialETHDLL();
		m_pAlgorithmBase->openDevice();
	}
	catch (...)
	{
		emit mainLogicSendMsg(QStringLiteral("通讯模块设置失败！"), 1);
	}
}

void MainLogicTcp::mainLogicRun(QByteArray bytes)
{
	if (m_bAllThreadNeadRun)
	{
		QString msg = QString(bytes);
		msg.replace("/*", "");
		if (msg.length()==0)
		{
			return;
		}
		QStringList strDataList = msg.split("+");
		if (strDataList.count() < 3)
		{
			emit mainLogicSendMsg(QStringLiteral("通信数据不全！"), 1);
			return;
		}
		else
		{
			if (strDataList[0] == "" || strDataList[1] == "" || strDataList[2] == "")
			{
				emit mainLogicSendMsg(QStringLiteral("通信数据存在空值！"), 1);
				return;
			}
		}
		int nFlag = strDataList[0].toInt();
		switch (nFlag)
		{
		case 0:
			//收到设备端发送的来车信号，0+过车流水号+0
			emit mainLogicSendMsg(QStringLiteral("收到来车信号，流水号：") + strDataList[1] + QStringLiteral("，所有设备开始采集"), 0);
			runStartGrabData(strDataList[1]);
			break;
		case 1:
			if (strDataList.count() < 5)
			{
				emit mainLogicSendMsg(QStringLiteral("通信数据不全！"), 1);
				return;
			}
			//收到设备端发送的离车信号，1+过车流水号+线号+车号+主控端
			emit mainLogicSendMsg(QStringLiteral("收到离车信号，流水号：") + strDataList[1] + QStringLiteral("，线号：")
				+ strDataList[2] + QStringLiteral("，车号：") + strDataList[3] + QStringLiteral("，主控端：") + 
				strDataList[4] + QStringLiteral("，所有设备停止采集"), 0);
			runStopGrabData(strDataList[3], strDataList[2], strDataList[4]);
			break;
		case 2:
			//其他信息
			break;
		}
	}
}

void MainLogicTcp::runStartGrabData(QString strTrainRunNumber)
{	
	m_pAlgorithmBase->startGrab(strTrainRunNumber);
}

void MainLogicTcp::runStopGrabData(QString strTrainNumber, QString strLineNumber, QString strControlPort)
{
	m_pAlgorithmBase->stopGrab(strTrainNumber, strLineNumber, strControlPort);
}

void MainLogicTcp::simulateTrigger(QString strTrainNumber)
{
	m_pAlgorithmBase->simulateTrigger(strTrainNumber);

}


MainLogicTcp* MainLogicTcp::_instance = NULL;
DEALLOGIC_EXPORT MainLogicTcp *getMainLogicTcpInstance()
{
	if (NULL == MainLogicTcp::_instance)
	{
		MainLogicTcp::_instance = new MainLogicTcp();
	}
	return MainLogicTcp::_instance;
}