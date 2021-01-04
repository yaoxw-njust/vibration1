#pragma once

#include "dealalgorithm_global.h"
#include <Windows.h>
#include <QObject>
#include <iostream>
#include "algorithm_operate_thread.h"
#include "..\DealFile\ini_helper.h"
#include "..\DealFile\mysql_helper.h"
#include "..\DealFile\log_helper.h"
#include <QSemaphore>
#include <vector>
#include <QVector>

class DEALALGORITHM_EXPORT AlgorithmBase : public QObject
{
	Q_OBJECT

public:
	AlgorithmBase(QObject *parent=Q_NULLPTR);
	~AlgorithmBase();

	virtual void initialETHDLL();
	virtual void openDevice();
	virtual void startGrab(QString strTrainRunNumber="20201121125815");
	virtual void stopGrab(QString strTrainNumber = "098099", QString strLineNumber = "07", 
		QString strControlPort = "0");
	virtual void closeDevice();
	virtual void simulateTrigger(QString strTrainNumber);

	void dataCombination();

private:
	void initialData();
	void analysisAndSaveData();
	void saveDataToTxt(std::vector<double> data, QString dataName, QString fileRootPath);
	void operateData(std::vector<double> &v1, std::vector<double> &v2, std::vector<double> &v3, std::vector<double> &v4,
		std::vector<double> &v5, std::vector<double> &v6, std::vector<double> &v7, std::vector<double> &v8,
		std::vector<double> &g1, std::vector<double> &g2, std::vector<double> &g3, std::vector<double> &g4,
		std::vector<double> &t1, std::vector<double> &t2, std::vector<double> &t3, std::vector<double> &t4);
	void readDataFromTxt(QString strPath, std::vector<double> &vec1);
	void writeDataToSql(std::vector<std::vector<double>> vibrateDataL, std::vector<std::vector<double>> axleDataL,
		std::vector<std::vector<double>> electricalDataL, std::vector<std::vector<double>> vibrateDataR,
		std::vector<std::vector<double>> axleDataR, std::vector<std::vector<double>> electricalDataR);


	//private slots:
	//void getAndSendResultData(int directionFlag);


signals:
	void algorithmSendMsg(QString msg, int flag);
	void sendVibrationData();
	void sendAxleData();
	void sendElectricalData();

private:
	//定义ETHDLL需要用到的函数指针
#pragma region 定义ETHDLL需要用到的函数指针
	int(__stdcall *SysInit)(unsigned int *ipbuff, unsigned int num);

	unsigned int(__stdcall *IP_StrToInt)(char *strIp);

	int(__stdcall *IP_IntToStr)(char *strIp, unsigned int ip);

	int(__stdcall *ConnectCreate)(unsigned int inst_ip, unsigned short inst_port, unsigned short local_port,
		unsigned int type, unsigned int outtime, unsigned int num);

	int(__stdcall *ConnectDel)(unsigned int socket_num);

	int(__stdcall *ADSyncParaWrite)(unsigned int  ad_frequency, unsigned int  ad_range, unsigned int  ain_select,
		unsigned short ch_enabled, unsigned short masterflag, unsigned int socket_num);

	int(__stdcall *ADGainWrite)(unsigned short gain_ch0, unsigned short gain_ch1, unsigned short gain_ch2,
		unsigned short gain_ch3, unsigned short gain_ch4, unsigned short gain_ch5, unsigned short gain_ch6,
		unsigned short gain_ch7, unsigned short gain_ch8, unsigned short gain_ch9, unsigned short gain_ch10,
		unsigned short gain_ch11, unsigned short gain_ch12, unsigned short gain_ch13, unsigned short gain_ch14,
		unsigned short gain_ch15, unsigned int socket_num);

	int(__stdcall *ADTriggerExWrite)(unsigned short enabled, unsigned short A0_trig_type, unsigned short A1_trig_type,
		unsigned short D0_trig_type, unsigned short D1_trig_type, unsigned short A0_trig_ch_sel, unsigned short A1_trig_ch_sel,
		unsigned int   trig_num, int   A0_trig_vol, int   A1_trig_vol, unsigned int socket_num);

	int(__stdcall *ADTriggerWrite) (unsigned char enabled, unsigned char type, unsigned short spring_clk,
		unsigned short spring_num, unsigned short spring_vol, unsigned int socket_num);

	int(__stdcall *ADStart)(unsigned int socket_num);

	int(__stdcall *AdStatusRead)(unsigned int *ad_status, unsigned int *fifo_status, unsigned int *fifoff_num,
		unsigned int *cherr_num, unsigned int *fifodata_num, unsigned int *ad_real_fre, unsigned int *ad_num,
		unsigned int *ch_num, unsigned int socket_num);

	int(__stdcall *ADDataRead)(short *pData, unsigned int nCount, unsigned int socket_num);

	int(__stdcall *ADStop)(unsigned int socket_num);
#pragma endregion
	////////////////////////////////


public:
	HINSTANCE ETHDLLHandel = NULL;       //DLL载入句柄
	unsigned int int_ip = 0;
	int socket_index = -1;
	QString m_strConnectIP = "";         //控制盒IP
	QString m_strFileSavePathRoot = "";  //数据存储地址

	bool m_bIsOpen = false;              //是否打开设备
	bool m_bIsGrab = false;              //是否允许开启采集

	bool m_bIsRunAlg = false;

	std::vector<double> allGrabData;

	AlgorithmOperate *algotrithOperateRight = NULL; //计算右侧数据
	AlgorithmOperate *algotrithOperateLeft = NULL;  //计算左侧数据

private:
	MySqlHelper *m_pMySqlHelper = NULL;
	LogHelper *m_pLogHelper = NULL;


	bool m_bIsGetRightData = false;      //获得右侧数据
	bool m_bIsGetLeftData = false;       //获得左侧数据

	std::vector<double> v1, v2, v3, v4, v5, v6, v7, v8;  //8路振动信号
	std::vector<double> g1, g2, g3, g4;
	std::vector<double> t1, t2, t3, t4;

	QString m_strTrainRunNumber = "20201121125815";
	QString m_strTrainNumber = "098099";
	QString m_strLineNumber = "07";
	QString m_strMainControlPort = "0";
	QString m_strStation = QStringLiteral("南站");


public:
	std::vector<std::vector<double>> vibrationDataRight;
	std::vector<std::vector<double>> axleDataRight;
	std::vector<std::vector<double>> electricalDataRight;
	std::vector<std::vector<double>> vibrationDataLeft;
	std::vector<std::vector<double>> axleDataLeft;
	std::vector<std::vector<double>> electricalDataLeft;

	double m_dTrainSpeed = 0.0;
	double m_dEnvironmentTemp = 0.0;

	QSemaphore m_Semaphore;

public:
	 
	static AlgorithmBase* instance;

};

extern "C"
{
	DEALALGORITHM_EXPORT AlgorithmBase* getAlgorithmInstance();
}