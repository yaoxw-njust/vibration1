#pragma once

#include "deallogic_global.h"
#include "tcp_logic.h"
#include "..\DealAlgorithm\algorithm_base.h"


#include <QObject>
#include <QTimer>
#include <iostream>

class DEALLOGIC_EXPORT MainLogicTcp : public QObject
{
	Q_OBJECT

public:
	MainLogicTcp(QObject *parent = Q_NULLPTR);
	~MainLogicTcp();

	virtual void initialConnection();
	virtual void simulateTrigger(QString strTrainNumber);

private:
	void runStartGrabData(QString strTrainRunNumber);
	void runStopGrabData(QString strTrainNumber, QString strLineNumber, QString strControlPort);


	private slots:
	void mainLogicRun(QByteArray bytes);


signals:
	void mainLogicSendMsg(QString msg, int msgFlag);

	void sendVibrationData();
	void sendAxleData();
	void sendElectricalData();


public:
	static MainLogicTcp *_instance;
	TcpServer *m_pTcpServer = NULL;
	
private:
	AlgorithmBase *m_pAlgorithmBase = NULL;

};

extern "C"
{
	DEALLOGIC_EXPORT MainLogicTcp *getMainLogicTcpInstance();
}