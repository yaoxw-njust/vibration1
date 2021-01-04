
#include "algorithm_base.h"
#include "..\DealFile\ini_helper.h"
#include "import_dll_class.h"
#include <QLibrary>
#include <QtConcurrent\QtConcurrent>
#include <QFile>
#include <QDir>

#include <winnt.rh>
#include <winbase.h>

#pragma comment(lib, "ws2_32.lib") 	

AlgorithmBase::AlgorithmBase(QObject *parent) : QObject(parent)
{
	m_pMySqlHelper = ImportDllClass::GetInstance()->m_pMySqlHelper;
}



AlgorithmBase::~AlgorithmBase()
{
	if (NULL != ETHDLLHandel)
	{
		FreeLibrary(ETHDLLHandel);
	}
	ETHDLLHandel = NULL;

	if (NULL != m_pMySqlHelper)
	{
		delete m_pMySqlHelper;
	}
	m_pMySqlHelper = NULL;
}

void AlgorithmBase::initialETHDLL()
{

	ETHDLLHandel = LoadLibrary(TEXT("ETHDLL.dll"));
	if (NULL != ETHDLLHandel)
	{
		SysInit = (int(__stdcall *)(unsigned int *ipbuff, unsigned int num))GetProcAddress(ETHDLLHandel, "SysInit");
		if (!SysInit)
		{
			algorithmSendMsg(QStringLiteral("SysInit函数初始化异常"), 1);
		}

		IP_StrToInt = (unsigned int(__stdcall *)(char *strIp))GetProcAddress(ETHDLLHandel, "IP_StrToInt");
		if (!IP_StrToInt)
		{
			algorithmSendMsg(QStringLiteral("IP_StrToInt函数初始化异常"), 1);
		}

		IP_IntToStr = (int(__stdcall *)(char *strIp, unsigned int ip))GetProcAddress(ETHDLLHandel, "IP_IntToStr");
		if (!IP_IntToStr)
		{
			algorithmSendMsg(QStringLiteral("IP_IntToStr函数初始化异常"), 1);
		}

		ConnectCreate = (int(__stdcall *)(unsigned int inst_ip, unsigned short inst_port, unsigned short local_port,
			unsigned int type, unsigned int outtime, unsigned int num))GetProcAddress(ETHDLLHandel, "ConnectCreate");
		if (!ConnectCreate)
		{
			algorithmSendMsg(QStringLiteral("ConnectCreate函数初始化异常"), 1);
		}

		ConnectDel = (int(__stdcall *)(unsigned int socket_num))GetProcAddress(ETHDLLHandel, "ConnectDel");
		if (!ConnectDel)
		{
			algorithmSendMsg(QStringLiteral("ConnectDel函数初始化异常"), 1);
		}

		ADSyncParaWrite = (int(__stdcall *)(unsigned int  ad_frequency, unsigned int  ad_range, unsigned int  ain_select,
			unsigned short ch_enabled, unsigned short masterflag, unsigned int socket_num))GetProcAddress(ETHDLLHandel, "ADSyncParaWrite");
		if (!ADSyncParaWrite)
		{
			algorithmSendMsg(QStringLiteral("ADSyncParaWrite函数初始化异常"), 1);
		}

		ADGainWrite = (int(__stdcall *)(unsigned short gain_ch0, unsigned short gain_ch1, unsigned short gain_ch2,
			unsigned short gain_ch3, unsigned short gain_ch4, unsigned short gain_ch5, unsigned short gain_ch6,
			unsigned short gain_ch7, unsigned short gain_ch8, unsigned short gain_ch9, unsigned short gain_ch10,
			unsigned short gain_ch11, unsigned short gain_ch12, unsigned short gain_ch13, unsigned short gain_ch14,
			unsigned short gain_ch15, unsigned int socket_num))GetProcAddress(ETHDLLHandel, "ADGainWrite");
		if (!ADGainWrite)
		{
			algorithmSendMsg(QStringLiteral("ADGainWrite函数初始化异常"), 1);
		}

		ADTriggerExWrite = (int(__stdcall *)(unsigned short enabled, unsigned short A0_trig_type, unsigned short A1_trig_type,
			unsigned short D0_trig_type, unsigned short D1_trig_type, unsigned short A0_trig_ch_sel, unsigned short A1_trig_ch_sel,
			unsigned int   trig_num, int   A0_trig_vol, int   A1_trig_vol, unsigned int socket_num))GetProcAddress(ETHDLLHandel, "ADTriggerExWrite");
		if (!ADTriggerExWrite)
		{
			algorithmSendMsg(QStringLiteral("ADTriggerExWrite函数初始化异常"), 1);
		}

		ADTriggerWrite = (int(__stdcall *) (unsigned char enabled, unsigned char type, unsigned short spring_clk,
			unsigned short spring_num, unsigned short spring_vol, unsigned int socket_num))GetProcAddress(ETHDLLHandel, "ADTriggerWrite");
		if (!ADTriggerWrite)
		{
			algorithmSendMsg(QStringLiteral("ADTriggerWrite函数初始化异常"), 1);
		}

		ADStart = (int(__stdcall *)(unsigned int socket_num))GetProcAddress(ETHDLLHandel, "ADStart");
		if (!ADStart)
		{
			algorithmSendMsg(QStringLiteral("ADStart函数初始化异常"), 1);
		}

		ADDataRead = (int(__stdcall *)(short *pData, unsigned int nCount, unsigned int socket_num))GetProcAddress(ETHDLLHandel, "ADDataRead");
		if (!ADDataRead)
		{
			algorithmSendMsg(QStringLiteral("ADDataRead函数初始化异常"), 1);
		}

		ADStop = (int(__stdcall *)(unsigned int socket_num))GetProcAddress(ETHDLLHandel, "ADStop");
		if (!ADStop)
		{
			algorithmSendMsg(QStringLiteral("ADStop函数初始化异常"), 1);
		}

		AdStatusRead = (int(__stdcall *)(unsigned int *ad_status, unsigned int *fifo_status, unsigned int *fifoff_num,
			unsigned int *cherr_num, unsigned int *fifodata_num, unsigned int *ad_real_fre, unsigned int *ad_num,
			unsigned int *ch_num, unsigned int socket_num))GetProcAddress(ETHDLLHandel, "AdStatusRead");
		if (!AdStatusRead)
		{
			algorithmSendMsg(QStringLiteral("ADStatusRead函数初始化异常"), 1);
		}

	}
	else
	{
		algorithmSendMsg(QStringLiteral("DLL载入出现异常"), 1);
	}

	m_pLogHelper = ImportDllClass::GetInstance()->m_pLogHelper;
	IniHelper *iniHelper = ImportDllClass::GetInstance()->m_pIniHelper;
	m_strConnectIP = iniHelper->readIniStr("system", "data_box_ip", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
	algorithmSendMsg(QStringLiteral("综合数据盒连接IP：") + m_strConnectIP, 0);
	m_strFileSavePathRoot = iniHelper->readIniStr("system", "file_root_path", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
	m_strStation = iniHelper->readIniStr("system", "station", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");

	//实例化算法对象
	algotrithOperateRight = new AlgorithmOperate();
	algotrithOperateLeft = new AlgorithmOperate();

	//初始化信号量的数量
	m_Semaphore.release(16);

}

void AlgorithmBase::openDevice()
{
	m_bIsOpen = true;
	algorithmSendMsg(QStringLiteral("正在打开设备..."), 0);
	QtConcurrent::run([=](){
		QByteArray byte_ip = m_strConnectIP.toLatin1();
		int_ip = IP_StrToInt(byte_ip.data());
		int ret = SysInit(&int_ip, 1);
		if (-1 == ret)
		{
			m_bIsOpen = false;
			algorithmSendMsg(QStringLiteral("连接初始化异常"), 1);
		}

		socket_index = ConnectCreate(int_ip, 1600, 0, 1, 500, 3);
		if (socket_index < 0)
		{
			m_bIsOpen = false;
			algorithmSendMsg(QStringLiteral("创建连接出现异常，错误代码为：") + QString::number(socket_index), 1);
		}

		ret = ADSyncParaWrite(10000, 1, 0, 0xFFFF, 0, socket_index);
		if (-1 == ret)
		{
			m_bIsOpen = false;
			algorithmSendMsg(QStringLiteral("同步采集仪设置异常"), 1);
		}

		ret = ADGainWrite(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, socket_index);
		if (-1 == ret)
		{
			m_bIsOpen = false;
			algorithmSendMsg(QStringLiteral("采集仪增益设置异常"), 1);
		}

		if (m_bIsOpen)
		{
			algorithmSendMsg(QStringLiteral("设备打开成功"), 0);
		}

		ret = ConnectDel(socket_index);
		if (ret)
		{
			algorithmSendMsg(QStringLiteral("设备断开连接"), 0);
		}
	});
}

void AlgorithmBase::startGrab(QString strTrainRunNumber)
{
	initialData();
	m_strTrainRunNumber = strTrainRunNumber;

	QtConcurrent::run([=](){
		short* arrayData = NULL;
		m_bIsOpen = true;

		QByteArray byte_ip = m_strConnectIP.toLatin1();
		int_ip = IP_StrToInt(byte_ip.data());
		int ret = SysInit(&int_ip, 1);
		if (-1 == ret)
		{
			m_bIsOpen = false;
			algorithmSendMsg(QStringLiteral("连接初始化异常"), 1);
		}

		socket_index = ConnectCreate(int_ip, 1600, 0, 1, 500, 3);
		if (socket_index < 0)
		{
			m_bIsOpen = false;
			algorithmSendMsg(QStringLiteral("创建连接出现异常，错误代码为：") + QString::number(socket_index), 1);
		}

		ret = ADSyncParaWrite(10000, 1, 0, 0xFFFF, 0, socket_index);
		if (-1 == ret)
		{
			m_bIsOpen = false;
			algorithmSendMsg(QStringLiteral("同步采集仪设置异常"), 1);
		}

		ret = ADGainWrite(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, socket_index);
		if (-1 == ret)
		{
			m_bIsOpen = false;
			algorithmSendMsg(QStringLiteral("采集仪增益设置异常"), 1);
		}

		if (m_bIsOpen)
		{
			try
			{
				int ret = ADStart(socket_index);
				if (-1 == ret)
				{
					algorithmSendMsg(QStringLiteral("开启A/D采集失败"), 1);
					m_bIsGrab = false;
					return;
				}
				m_bIsGrab = true;
				algorithmSendMsg(QStringLiteral("开启A/D采集"), 0);
				QThread::msleep(100);  //防止读取数据时卡死

				while (m_bIsGrab)
				{
					arrayData = new short[720];
					int ret = ADDataRead(arrayData, 720, socket_index);
					for (int i = 0; i < 720; i++)
					{
						allGrabData.push_back((*(arrayData + i))*10.0 / 0x7fff);
					}
					delete[] arrayData;
					arrayData = NULL;
				}
			}
			catch (...)
			{
				algorithmSendMsg(QStringLiteral("数据采集中出现错误"), 1);
			}
			if (NULL != arrayData)
			{
				delete[] arrayData;
			}
			arrayData = NULL;
			int ret = ADStop(socket_index);
			if (ret > 0)
			{
				algorithmSendMsg(QStringLiteral("停止采集数据"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("停止采集数据异常"), 1);
			}
			ret = ConnectDel(socket_index);
			if (ret > 0)
			{
				algorithmSendMsg(QStringLiteral("采集连接已断开"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("采集连接已断开异常"), 1);
			}
			algorithmSendMsg(QStringLiteral("开始数据分析和落盘"), 0);
			analysisAndSaveData();
		}
		else
		{
			algorithmSendMsg(QStringLiteral("设备未打开，无法开启采集"), 1);
		}
	});

}

void AlgorithmBase::stopGrab(QString strTrainNumber, QString strLineNumber, QString strControlPort)
{
	m_strTrainNumber = strTrainNumber;
	m_strLineNumber = strLineNumber;
	m_strMainControlPort = strControlPort;
	algorithmSendMsg(QStringLiteral("收到停止采集命令，开始停止采集"), 0);
	m_bIsGrab = false;
}

void AlgorithmBase::closeDevice()
{
	m_bIsOpen = false;
	int ret = ConnectDel(socket_index);
	if (-1 == ret)
	{
		algorithmSendMsg(QStringLiteral("停止连接失败"), 1);
	}

}

void AlgorithmBase::initialData()
{
	m_bIsGetRightData = false;
	m_bIsGetLeftData = false;
	std::vector<double>().swap(allGrabData);
	std::vector<std::vector<double>>().swap(vibrationDataRight);
	std::vector<std::vector<double>>().swap(axleDataRight);
	std::vector<std::vector<double>>().swap(electricalDataRight);
	std::vector<std::vector<double>>().swap(vibrationDataLeft);
	std::vector<std::vector<double>>().swap(axleDataLeft);
	std::vector<std::vector<double>>().swap(electricalDataLeft);
	std::vector<double>().swap(v1);       //8路振动信号
	std::vector<double>().swap(v2);
	std::vector<double>().swap(v3);
	std::vector<double>().swap(v4);
	std::vector<double>().swap(v5);
	std::vector<double>().swap(v6);
	std::vector<double>().swap(v7);
	std::vector<double>().swap(v8);
	std::vector<double>().swap(g1);
	std::vector<double>().swap(g2);
	std::vector<double>().swap(g3);
	std::vector<double>().swap(g4);
	std::vector<double>().swap(t1);
	std::vector<double>().swap(t2);
	std::vector<double>().swap(t3);
	std::vector<double>().swap(t4);

}

void AlgorithmBase::analysisAndSaveData()
{

	//数据归类
	for (size_t i = 0; i < allGrabData.size() / 16; i++)
	{
		v1.push_back(allGrabData[i * 16 + 0]); v2.push_back(allGrabData[i * 16 + 1]); v3.push_back(allGrabData[i * 16 + 2]);
		v4.push_back(allGrabData[i * 16 + 3]); v5.push_back(allGrabData[i * 16 + 4]); v6.push_back(allGrabData[i * 16 + 5]);
		v7.push_back(allGrabData[i * 16 + 6]); v8.push_back(allGrabData[i * 16 + 7]);

		g2.push_back(allGrabData[i * 16 + 8]); g1.push_back(allGrabData[i * 16 + 9]); g3.push_back(allGrabData[i * 16 + 10]);
		g4.push_back(allGrabData[i * 16 + 11]);

		t1.push_back(allGrabData[i * 16 + 12]); t2.push_back(allGrabData[i * 16 + 13]); t3.push_back(allGrabData[i * 16 + 14]);
		t4.push_back(allGrabData[i * 16 + 15]);
	}

	//数据存储
	QString strSavePath = m_strFileSavePathRoot + m_strTrainRunNumber + "\\VAIS\\";
	strSavePath.replace("/", "\\");

	//保存本地数据不采用多线程处理，防止后面运行异常后数据丢失，无法复现程序错误原因
	//写入振动数据
	saveDataToTxt(v1, "vibration1", strSavePath);  //储存振动1的数据
	algorithmSendMsg(QStringLiteral("vibration1完成落盘"), 0);
	saveDataToTxt(v2, "vibration2", strSavePath);  //储存振动2的数据
	algorithmSendMsg(QStringLiteral("vibration2完成落盘"), 0);
	saveDataToTxt(v3, "vibration3", strSavePath);  //储存振动3的数据
	algorithmSendMsg(QStringLiteral("vibration3完成落盘"), 0);
	saveDataToTxt(v4, "vibration4", strSavePath);  //储存振动4的数据
	algorithmSendMsg(QStringLiteral("vibration4完成落盘"), 0);
	saveDataToTxt(v5, "vibration5", strSavePath);  //储存振动5的数据
	algorithmSendMsg(QStringLiteral("vibration5完成落盘"), 0);
	saveDataToTxt(v6, "vibration6", strSavePath);  //储存振动6的数据
	algorithmSendMsg(QStringLiteral("vibration6完成落盘"), 0);
	saveDataToTxt(v7, "vibration7", strSavePath);  //储存振动7的数据
	algorithmSendMsg(QStringLiteral("vibration7完成落盘"), 0);
	saveDataToTxt(v8, "vibration8", strSavePath);  //储存振动8的数据
	algorithmSendMsg(QStringLiteral("vibration8完成落盘"), 0);

	//写入磁钢数据
	saveDataToTxt(g1, "magnetic1", strSavePath);   //储存磁钢1的数据
	algorithmSendMsg(QStringLiteral("magnetic1完成落盘"), 0);
	saveDataToTxt(g2, "magnetic2", strSavePath);   //储存磁钢2的数据
	algorithmSendMsg(QStringLiteral("magnetic2完成落盘"), 0);
	saveDataToTxt(g3, "magnetic3", strSavePath);   //储存磁钢3的数据
	algorithmSendMsg(QStringLiteral("magnetic3完成落盘"), 0);
	saveDataToTxt(g4, "magnetic4", strSavePath);   //储存磁钢4的数据
	algorithmSendMsg(QStringLiteral("magnetic4完成落盘"), 0);

	//写入温度数据
	saveDataToTxt(t1, "temperate1", strSavePath);   //储存温度1的数据
	algorithmSendMsg(QStringLiteral("temperate1完成落盘"), 0);
	saveDataToTxt(t2, "temperate2", strSavePath);   //储存温度2的数据
	algorithmSendMsg(QStringLiteral("temperate2完成落盘"), 0);
	saveDataToTxt(t3, "temperate3", strSavePath);   //储存温度3的数据
	algorithmSendMsg(QStringLiteral("temperate3完成落盘"), 0);
	saveDataToTxt(t4, "temperate4", strSavePath);   //储存温度4的数据
	algorithmSendMsg(QStringLiteral("temperate4完成落盘"), 0);

	//处理数据
	operateData(v1, v2, v3, v4, v5, v6, v7, v8, g1, g2, g3, g4, t1, t2, t3, t4);
}

void AlgorithmBase::saveDataToTxt(std::vector<double> data, QString dataName, QString fileRootPath)
{
	try
	{
		QDir dir;
		if (!dir.exists(fileRootPath))
		{
			dir.mkpath(fileRootPath);
		}

		QFile file(fileRootPath + dataName + ".txt");
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			algorithmSendMsg(dataName + QStringLiteral(".txt文件创建失败"), 1);
		}
		QTextStream in(&file);
		for (int i = 0; i < data.size(); i++)
		{
			in << data[i] << endl;
		}
	}
	catch (...)
	{
		algorithmSendMsg(dataName + QStringLiteral("数据写入出错，请检查"), 1);
	}

}

void AlgorithmBase::operateData(std::vector<double> &v1, std::vector<double> &v2, std::vector<double> &v3, std::vector<double> &v4,
	std::vector<double> &v5, std::vector<double> &v6, std::vector<double> &v7, std::vector<double> &v8,
	std::vector<double> &g1, std::vector<double> &g2, std::vector<double> &g3, std::vector<double> &g4,
	std::vector<double> &t1, std::vector<double> &t2, std::vector<double> &t3, std::vector<double> &t4)
{
	algorithmSendMsg(QStringLiteral("开始处理数据"), 0);
	//处理数据
	std::vector<double> sensitivity1 = { 0.0099, 0.0099, 0.010, 0.0101 };    //振动传感器灵敏度
	std::vector<double> sensitivity2 = { 0.0098, 0.010, 0.0097, 0.0098 };

	QtConcurrent::run([=](){
		if (!m_bIsRunAlg)
		{
			m_bIsRunAlg = true;
			algotrithOperateRight->initialData(v1, v2, v3, v4, g2, g3, g4, t1, t3, sensitivity1, 1);
			algotrithOperateRight->runSingle();

			vibrationDataRight = algotrithOperateRight->vibrationEigenValue;
			axleDataRight = algotrithOperateRight->bearPalteTemperatureDivided;
			electricalDataRight = algotrithOperateRight->electricalTemperatureDivided;
			m_dTrainSpeed = algotrithOperateRight->m_dTrainSpeed;
			m_dEnvironmentTemp = algotrithOperateRight->m_dEnvironmentTemp;
			algorithmSendMsg(QStringLiteral("右侧计算完成"), 0);

			algotrithOperateLeft->initialData(v5, v6, v7, v8, g2, g3, g4, t2, t4, sensitivity2, 0);
			algotrithOperateLeft->runSingle();
			vibrationDataLeft = algotrithOperateLeft->vibrationEigenValue;
			axleDataLeft = algotrithOperateLeft->bearPalteTemperatureDivided;
			electricalDataLeft = algotrithOperateLeft->electricalTemperatureDivided;
			algorithmSendMsg(QStringLiteral("左侧计算完成"), 0);

			dataCombination(); //进入数据综合处理，方式因数据缺失导致程序后续运行异常

		}
		else
		{
			algorithmSendMsg(QStringLiteral("右侧前一次计算还未结束，请勿重复计算"), 1);
		}

	});
}


void AlgorithmBase::writeDataToSql(std::vector<std::vector<double>> vibrateDataL, std::vector<std::vector<double>> axleDataL,
	std::vector<std::vector<double>> electricalDataL, std::vector<std::vector<double>> vibrateDataR,
	std::vector<std::vector<double>> axleDataR, std::vector<std::vector<double>> electricalDataR)
{
	//从表"train_param"查询7个特征值的报警、预警值
	double jfg_warn; int jfg_warn_num = 0;
	double jfg_alarm; int jfg_alarm_num = 0;
	double fz_warn; int fz_warn_num = 0;
	double fz_alarm; int fz_alarm_num = 0;
	double fzyz_warn; int fzyz_warn_num = 0;
	double fzyz_alarm; int fzyz_alarm_num = 0;
	double qd_warn; int qd_warn_num = 0;
	double qd_alarm; int qd_alarm_num = 0;
	double qdyz_warn; int qdyz_warn_num = 0;
	double qdyz_alarm; int qdyz_alarm_num = 0;
	double zw_warn; int zw_warn_num = 0;
	double zw_alarm; int zw_alarm_num = 0;
	double zwinc_warn; int zwinc_warn_num = 0;
	double zwinc_alarm; int zwinc_alarm_num = 0;
	double zw_max;
	double zwinc_max;
	double motortemp_warn; int motortemp_warn_num = 0;
	double motortemp_alarm; int motortemp_alarm_num = 0;
	double motortempinc_warn; int motortempinc_warn_num = 0;
	double motortempinc_alarm; int motortempinc_alarm_num = 0;
	double motortemp_max;
	double motortempinc_max;

	if (m_pMySqlHelper->openSql())
	{
#pragma region 从数据库读取预警和报警参数
		QString strSqlRead = "";
		m_pMySqlHelper->readSqlValue("train_param", "jfg_warn", strSqlRead);
		jfg_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "jfg_alarm", strSqlRead);
		jfg_alarm = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "fz_warn", strSqlRead);
		fz_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "fz_alarm", strSqlRead);
		fz_alarm = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "fzyz_warn", strSqlRead);
		fzyz_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "fzyz_alarm", strSqlRead);
		fzyz_alarm = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "qd_warn", strSqlRead);
		qd_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "qd_alarm", strSqlRead);
		qd_alarm = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "qdyz_warn", strSqlRead);
		qdyz_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "qdyz_alarm", strSqlRead);
		qdyz_alarm = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "zw_warn", strSqlRead);
		zw_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "zw_alarm", strSqlRead);
		zw_alarm = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "motortemp_warn", strSqlRead);
		motortemp_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "motortemp_alarm", strSqlRead);
		motortemp_alarm = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "zwinc_warn", strSqlRead);
		zwinc_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "zwinc_alarm", strSqlRead);
		zwinc_alarm = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "motortempinc_warn", strSqlRead);
		motortempinc_warn = strSqlRead.toFloat();
		m_pMySqlHelper->readSqlValue("train_param", "motortempinc_alarm", strSqlRead);
		motortempinc_alarm = strSqlRead.toFloat();

		algorithmSendMsg(QStringLiteral("预警及报警参数加载完成"), 0);

#pragma endregion

#pragma region 计算左侧报警数
		for (int i = 0; i < 5; i++)
		{
			for (std::vector<double>::iterator it = vibrateDataL[i].begin(); it != vibrateDataL[i].end(); it++)
			{
				switch (i)
				{
				case 0:
					if (*it >= jfg_alarm)
					{
						jfg_alarm_num++;
					}
					else if ((*it < jfg_alarm) && (*it >= jfg_warn))
					{
						jfg_warn_num++;
					}
					break;
				case 1:
					if (*it >= fz_alarm)
					{
						fz_alarm_num++;
					}
					else if ((*it < fz_alarm) && (*it >= fz_warn))
					{
						fz_warn_num++;
					}
				case 2:
					if (*it >= fzyz_alarm)
					{
						fzyz_alarm_num++;
					}
					else if ((*it < fzyz_alarm) && (*it >= fzyz_warn))
					{
						fzyz_warn_num++;
					}
					break;
				case 3:
					if (*it >= qd_alarm)
					{
						fzyz_alarm_num++;
					}
					else if ((*it < qd_alarm) && (*it >= qd_warn))
					{
						qd_warn_num++;
					}
					break;
				case 4:
					if (*it >= qdyz_alarm)
					{
						qdyz_alarm_num++;
					}
					else if ((*it < qdyz_alarm) && (*it >= qdyz_warn))
					{
						qdyz_warn_num++;
					}
				}
			}
		}
#pragma endregion

#pragma region 计算右侧报警数
		for (int i = 0; i < 5; i++)
		{
			for (std::vector<double>::iterator it = vibrateDataR[i].begin(); it != vibrateDataR[i].end(); it++)
			{
				switch (i)
				{
				case 0:
					if (*it >= jfg_alarm)
					{
						jfg_alarm_num++;
					}
					else if ((*it < jfg_alarm) && (*it >= jfg_warn))
					{
						jfg_warn_num++;
					}
					break;
				case 1:
					if (*it >= fz_alarm)
					{
						fz_alarm_num++;
					}
					else if ((*it < fz_alarm) && (*it >= fzyz_warn))
					{
						fz_warn_num++;
					}
					break;
				case 2:
					if (*it >= fzyz_alarm)
					{
						fzyz_alarm_num++;
					}
					else if ((*it < fzyz_alarm) && (*it >= fzyz_warn))
					{
						fzyz_warn_num++;
					}
					break;
				case 3:
					if (*it >= qd_alarm)
					{
						qd_alarm_num++;
					}
					else if ((*it < qd_alarm) && (*it >= qd_warn))
					{
						qd_warn_num++;
					}
					break;
				case 4:
					if (*it >= qdyz_alarm)
					{
						qdyz_alarm_num++;
					}
					else if ((*it < qdyz_alarm) && (*it >= qdyz_warn))
					{
						qdyz_warn_num++;
					}
					break;
				}
			}

		}

#pragma endregion

#pragma region 计算左侧轴温报警和预警数
		for (std::vector<double>::iterator it = axleDataL[1].begin(); it != axleDataL[1].end(); it++)
		{
			if (*it >= zw_alarm)
			{
				zw_alarm_num++;
			}
			else if ((*it < zw_alarm) && (*it >= zw_warn))
			{
				zw_warn_num++;
			}
		}

#pragma endregion 

#pragma region 计算左侧轴温升报警和预警数
		for (std::vector<double>::iterator it = axleDataL[2].begin(); it != axleDataL[2].end(); it++)
		{
			if (*it >= zwinc_alarm)
			{
				zwinc_alarm_num++;
			}
			else if ((*it < zwinc_alarm) && (*it >= zwinc_warn))
			{
				zwinc_warn_num++;
			}
		}

#pragma endregion 

#pragma region 计算右侧轴温报警和预警数
		for (std::vector<double>::iterator it = axleDataR[1].begin(); it != axleDataR[1].end(); it++)
		{
			if (*it >= zw_alarm)
			{
				zw_alarm_num++;
			}
			else if ((*it < zw_alarm) && (*it >= zw_warn))
			{
				zw_warn_num++;
			}
		}

#pragma endregion 

#pragma region 计算右侧轴温升报警和预警数
		for (std::vector<double>::iterator it = axleDataR[2].begin(); it != axleDataR[2].end(); it++)
		{
			if (*it >= zwinc_alarm)
			{
				zwinc_alarm_num++;
			}
			else if ((*it < zwinc_alarm) && (*it >= zwinc_warn))
			{
				zwinc_warn_num++;
			}
		}

#pragma endregion 

#pragma region 计算左侧电机温度报警和预警数
		for (std::vector<double>::iterator it = electricalDataL[1].begin(); it != electricalDataL[1].end(); it++)
		{
			if (*it >= motortemp_alarm)
			{
				motortemp_alarm_num++;
			}
			else if ((*it < motortemp_alarm) && (*it >= motortemp_warn))
			{
				motortemp_warn_num++;
			}
		}

#pragma endregion

#pragma region 计算左侧电机温度升报警和预警数
		for (std::vector<double>::iterator it = electricalDataL[2].begin(); it != electricalDataL[2].end(); it++)
		{
			if (*it >= motortempinc_alarm)
			{
				motortempinc_alarm_num++;
			}
			else if ((*it < motortempinc_alarm) && (*it >= motortempinc_warn))
			{
				motortempinc_warn_num++;
			}
		}

#pragma endregion

#pragma region 计算右侧电机温度报警和预警数
		for (std::vector<double>::iterator it = electricalDataR[1].begin(); it != electricalDataR[1].end(); it++)
		{
			if (*it >= motortemp_alarm)
			{
				motortemp_alarm_num++;
			}
			else if ((*it < motortemp_alarm) && (*it >= motortemp_warn))
			{
				motortemp_warn_num++;
			}
		}

#pragma endregion

#pragma region 计算右侧电机温度升报警和预警数
		for (std::vector<double>::iterator it = electricalDataR[2].begin(); it != electricalDataR[2].end(); it++)
		{
			if (*it >= motortempinc_alarm)
			{
				motortempinc_alarm_num++;
			}
			else if ((*it < motortempinc_alarm) && (*it >= motortempinc_warn))
			{
				motortempinc_warn_num++;
			}

		}

#pragma endregion

		//轴温最大值
		zw_max = (*std::max_element(axleDataL[1].begin(), axleDataL[1].end()) > *std::max_element(axleDataR[1].begin(), axleDataR[1].end()) ?
			*std::max_element(axleDataL[1].begin(), axleDataL[1].end()) : *std::max_element(axleDataR[1].begin(), axleDataR[1].end()));
		//z轴温升最大值
		zwinc_max = (*std::max_element(axleDataL[2].begin(), axleDataL[2].end()) > *std::max_element(axleDataR[2].begin(), axleDataR[2].end()) ?
			*std::max_element(axleDataL[2].begin(), axleDataL[2].end()) : *std::max_element(axleDataR[2].begin(), axleDataR[2].end()));
		//电机温度最大值
		motortemp_max = (*std::max_element(electricalDataL[1].begin(), electricalDataL[1].end()) > *std::max_element(electricalDataR[1].begin(), electricalDataR[1].end()) ?
			*std::max_element(electricalDataL[1].begin(), electricalDataL[1].end()) : *std::max_element(electricalDataR[1].begin(), electricalDataR[1].end()));
		//电机温升最大值
		motortempinc_max = (*std::max_element(electricalDataL[2].begin(), electricalDataL[2].end()) > *std::max_element(electricalDataR[2].begin(), electricalDataR[2].end()) ?
			*std::max_element(electricalDataL[2].begin(), electricalDataL[2].end()) : *std::max_element(electricalDataR[2].begin(), electricalDataR[2].end()));

		int state = 4;
		if (jfg_alarm_num > 0 || fz_alarm_num > 0 || fzyz_alarm_num > 0 || qdyz_alarm_num > 0 || zw_alarm_num > 0 || motortemp_alarm_num > 0 || qd_alarm_num > 0 || zwinc_alarm_num > 0 || motortempinc_alarm_num > 0)
		{
			state = 1;
		}
		else if (jfg_warn_num > 0 || fz_warn_num > 0 || fzyz_warn_num > 0 || qdyz_warn_num > 0 || zw_warn_num > 0 || motortemp_warn_num > 0 || qd_warn_num > 0 || zwinc_warn_num > 0 || motortempinc_warn_num > 0)
		{
			state = 2;
		}



		algorithmSendMsg(QStringLiteral("数据库打开成功"), 0);
		QStringList key, value;
		key << "line_id" << "train_id" << "train_station" << "train_direction" << "train_state" << "train_speed" << "train_date";
		value << m_strLineNumber << m_strTrainNumber << m_strStation << m_strMainControlPort << QString::number(state) << QString::number(m_dTrainSpeed) << m_strTrainRunNumber;
		bool ret = m_pMySqlHelper->writeSqlData("train_info", key, value);
		if (ret)
		{
			algorithmSendMsg(QStringLiteral("train_info数据表写入成功"), 0);
		}
		else
		{
			algorithmSendMsg(QStringLiteral("train_info数据表写入失败"), 1);
		}
		QString m_strOnlyID = "";
		ret = m_pMySqlHelper->readSqlValue("train_info", "train_onlyid", m_strOnlyID);
		if (ret)
		{
			algorithmSendMsg(QStringLiteral("train_onlyid读取成功"), 0);
		}
		else
		{
			algorithmSendMsg(QStringLiteral("train_onlyid读取失败"), 1);
		}

		key.clear(); value.clear();
		key << "train_onlyid" << "fz_warn" << "fz_alarm" << "jfg_warn" << "jfg_alarm" << "fzyz_warn" <<
			"fzyz_alarm" << "qd_warn" << "qd_alarm" << "qdyz_warn" << "qdyz_alarm" << "zx_temp" <<
			"motor_temp" << "zx_tempinc" << "motor_tempinc" << "train_date";
		value << m_strOnlyID << QString::number(fz_warn_num) << QString::number(fz_alarm_num) << QString::number(jfg_warn_num) <<
			QString::number(jfg_alarm_num) << QString::number(fzyz_warn_num) << QString::number(fzyz_alarm_num) << QString::number(qd_warn_num) <<
			QString::number(qd_alarm_num) << QString::number(qdyz_warn_num) << QString::number(qdyz_alarm_num) <<
			QString::number(zw_max) << QString::number(motortemp_max) << QString::number(zwinc_max) <<
			QString::number(motortempinc_max) << m_strTrainRunNumber;
		ret = m_pMySqlHelper->writeSqlData("train_data", key, value);
		if (ret)
		{
			algorithmSendMsg(QStringLiteral("train_data数据表写入成功"), 0);
		}
		else
		{
			algorithmSendMsg(QStringLiteral("train_data数据表写入失败"), 1);
		}


		if (jfg_alarm_num > 0 || fz_alarm_num > 0 || fzyz_alarm_num > 0 || qdyz_alarm_num > 0 || qd_alarm_num > 0)
		{
			key.clear(); value.clear();
			key << "train_onlyid" << "fault_type" << "fault_rank" << "train_date";
			value << m_strOnlyID << "1" << "1" << m_strTrainRunNumber;
			ret = m_pMySqlHelper->writeSqlData("train_fault", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入1、1成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表第一次写入1、1失败"), 1);
			}

		}
		else if (jfg_warn_num > 0 || fz_warn_num > 0 || fzyz_warn_num > 0 || qdyz_warn_num > 0 || qd_warn_num > 0)
		{
			key.clear(); value.clear();
			key << "train_onlyid" << "fault_type" << "fault_rank" << "train_date";
			value << m_strOnlyID << "1" << "2" << m_strTrainRunNumber;
			ret = m_pMySqlHelper->writeSqlData("train_fault", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入1、2成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入1、2失败"), 1);
			}
		}

		if (zw_alarm_num > 0 || zwinc_alarm_num > 0)
		{
			key.clear(); value.clear();
			key << "train_onlyid" << "fault_type" << "fault_rank" << "train_date";
			value << m_strOnlyID << "2" << "1" << m_strTrainRunNumber;
			ret = m_pMySqlHelper->writeSqlData("train_fault", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入2、1成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入2、1失败"), 1);
			}
		}
		else if (zw_warn_num > 0 || zwinc_warn_num > 0)
		{
			key.clear(); value.clear();
			key << "train_onlyid" << "fault_type" << "fault_rank" << "train_date";
			value << m_strOnlyID << "2" << "2" << m_strTrainRunNumber;
			ret = m_pMySqlHelper->writeSqlData("train_fault", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入2、2成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入2、2失败"), 1);
			}
		}

		if (motortemp_alarm_num > 0 || motortempinc_alarm_num > 0)
		{
			key.clear(); value.clear();
			key << "train_onlyid" << "fault_type" << "fault_rank" << "train_date";
			value << m_strOnlyID << "3" << "1" << m_strTrainRunNumber;
			ret = m_pMySqlHelper->writeSqlData("train_fault", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入3、1成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入3、1失败"), 1);
			}

		}
		else if (motortemp_warn_num > 0 || motortempinc_warn_num > 0)
		{
			key.clear(); value.clear();
			key << "train_onlyid" << "fault_type" << "fault_rank" << "train_date";
			value << m_strOnlyID << "3" << "2" << m_strTrainRunNumber;
			ret = m_pMySqlHelper->writeSqlData("train_fault", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入3、2成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("train_fault数据表写入3、2失败"), 1);
			}
		}

		QStringList qsl;
		QStringList qsl1;
		qsl << "IA" << "IB" << "IC" << "IIC" << "IIB" << "IIA";
		qsl1 << "IIA" << "IIB" << "IIC" << "IC" << "IB" << "IA";

		//分奇偶车，取奇车
		key.clear();
		key << "train_onlyid" << "carriage_num" << "wheel_num" << "jfg_value" << "fz_value" << "fzyz_value" <<
			"qd_value" << "qdyz_value" << "zx_temp" << "zx_tempinc" << "train_date" << "hj_temp";
		for (int i = 0; i < 24; i++)
		{
			value.clear();
			value << m_strOnlyID;
			int y = i / 8;
			int z = i / 2;
			int wheel_num = (i % 8) + 1;    // 车轮号

			if (i % 2 == 0)
			{
				if (m_strMainControlPort.toInt() % 2 == 1)
				{
					value << qsl[y];
				}
				else
				{
					value << qsl1[y];
				}

				value << QString::number(wheel_num) << QString::number(vibrateDataL[0][z]) <<
					QString::number(vibrateDataL[1][z]) << QString::number(vibrateDataL[2][z]) <<
					QString::number(vibrateDataL[3][z]) << QString::number(vibrateDataL[4][z]) <<
					QString::number(axleDataL[1][z]) << QString::number(axleDataL[2][z]) << m_strTrainRunNumber
					<< QString::number(m_dEnvironmentTemp);
			}
			else
			{
				if (m_strMainControlPort.toInt() % 2 == 1)
				{
					value << qsl[y];
				}
				else
				{
					value << qsl1[y];
				}
				value << QString::number(wheel_num) << QString::number(vibrateDataR[0][z]) <<
					QString::number(vibrateDataR[1][z]) << QString::number(vibrateDataR[2][z]) <<
					QString::number(vibrateDataR[3][z]) << QString::number(vibrateDataR[4][z]) <<
					QString::number(axleDataR[1][z]) << QString::number(axleDataR[2][z]) << m_strTrainRunNumber
					<< QString::number(m_dEnvironmentTemp);
			}
			bool ret = m_pMySqlHelper->writeSqlData("vt_info", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("vt_info数据表，第") + QString::number(i) + QStringLiteral("次写入成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("vt_info数据表，第") + QString::number(i) + QStringLiteral("次写入失败"), 1);
			}
		}

		//分奇偶车，取偶车
		for (int i = 24; i < 48; ++i)
		{
			value.clear();
			value << m_strOnlyID;
			int y = i / 8;
			int z = i / 2;
			int wheel_num = 8 - (i % 8);    // 车轮号
			if (i % 2 == 0)
			{
				if (m_strMainControlPort.toInt() % 2 == 1)
				{
					value << qsl[y];
				}
				else
				{
					value << qsl1[y];
				}

				value << QString::number(wheel_num) << QString::number(vibrateDataL[0][z]) <<
					QString::number(vibrateDataL[1][z]) << QString::number(vibrateDataL[2][z]) <<
					QString::number(vibrateDataL[3][z]) << QString::number(vibrateDataL[4][z]) <<
					QString::number(axleDataL[1][z]) << QString::number(axleDataL[2][z]) << m_strTrainRunNumber
					<< QString::number(m_dEnvironmentTemp);
			}
			else
			{
				if (m_strMainControlPort.toInt() % 2 == 1)
				{
					value << qsl[y];
				}
				else
				{
					value << qsl1[y];
				}
				value << QString::number(wheel_num) << QString::number(vibrateDataR[0][z]) <<
					QString::number(vibrateDataR[1][z]) << QString::number(vibrateDataR[2][z]) <<
					QString::number(vibrateDataR[3][z]) << QString::number(vibrateDataR[4][z]) <<
					QString::number(axleDataR[1][z]) << QString::number(axleDataR[2][z]) << m_strTrainRunNumber
					<< QString::number(m_dEnvironmentTemp);
			}

			bool ret = m_pMySqlHelper->writeSqlData("vt_info", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("vt_info数据表，第") + QString::number(i) + QStringLiteral("次写入成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("vt_info数据表，第") + QString::number(i) + QStringLiteral("次写入失败"), 1);
			}
		}

		key.clear();
		key << "train_onlyid" << "carriage_num" << "motor_num" << "motor_temp" << "motor_tempinc" << "train_date";

		//分奇偶车，取奇车
		for (int i = 0; i < 8; ++i)
		{
			value.clear();
			value << m_strOnlyID;

			int y = i / 4 + 1;
			int z = i / 2;
			int motor_num = (i % 4) + 1;    // 电机号
			if (i % 2 == 0)
			{
				if (m_strMainControlPort.toInt() % 2 == 1)
				{
					value << qsl[y];
				}
				else
				{
					value << qsl1[y];
				}
				value << QString::number(motor_num) << QString::number(electricalDataR[1][z]) <<
					QString::number(electricalDataR[2][z]) << m_strTrainRunNumber;
			}
			else
			{
				if (m_strMainControlPort.toInt() % 2 == 1)
				{
					value << qsl[y];
				}
				else
				{
					value << qsl1[y];
				}
				value << QString::number(motor_num) << QString::number(electricalDataL[1][z]) <<
					QString::number(electricalDataL[2][z]) << m_strTrainRunNumber;
			}
			bool ret = m_pMySqlHelper->writeSqlData("motor_info", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("motor_info数据表，第") + QString::number(i) + QStringLiteral("次写入成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("motor_info数据表，第") + QString::number(i) + QStringLiteral("次写入失败"), 1);
			}

		}

		//分奇偶车，取偶车
		for (int i = 8; i < 16; ++i)
		{
			value.clear();
			value << m_strOnlyID;

			int y = i / 4 + 1;
			int z = i / 2;
			int motor_num = 4 - (i % 4);    //电机号

			if (i % 2 == 0)
			{
				if (m_strMainControlPort.toInt() % 2 == 1)
				{
					value << qsl[y];
				}
				else
				{
					value << qsl1[y];
				}
				value << QString::number(motor_num) << QString::number(electricalDataR[1][z]) <<
					QString::number(electricalDataR[2][z]) << m_strTrainRunNumber;
			}
			else
			{
				if (m_strMainControlPort.toInt() % 2 == 1)
				{
					value << qsl[y];
				}
				else
				{
					value << qsl1[y];
				}
				value << QString::number(motor_num) << QString::number(electricalDataL[1][z]) <<
					QString::number(electricalDataL[2][z]) << m_strTrainRunNumber;
			}
			bool ret = m_pMySqlHelper->writeSqlData("motor_info", key, value);
			if (ret)
			{
				algorithmSendMsg(QStringLiteral("motor_info数据表，第") + QString::number(i) + QStringLiteral("次写入成功"), 0);
			}
			else
			{
				algorithmSendMsg(QStringLiteral("motor_info数据表，第") + QString::number(i) + QStringLiteral("次写入失败"), 1);
			}
		}

		if (m_pMySqlHelper->closeSql())
		{
			emit algorithmSendMsg(QStringLiteral("数据库关闭！"), 0);
		}
		else
		{
			emit algorithmSendMsg(QStringLiteral("数据库关闭失败！"), 1);
		}
	}
	else
	{
		algorithmSendMsg(QStringLiteral("数据库打开失败"), 1);
	}

}

void AlgorithmBase::readDataFromTxt(QString strPath, std::vector<double> &vec1)
{
	m_Semaphore.acquire(1);

	QFile file(strPath);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		algorithmSendMsg(strPath + QStringLiteral("文件数据读取失败"), 1);
	}
	else
	{
		QTextStream in(&file);
		QString line = in.readLine();
		while (!line.isNull())
		{
			vec1.push_back(line.toFloat());
			line = in.readLine();
		}
	}
	m_Semaphore.release(1);
}

void AlgorithmBase::simulateTrigger(QString strTrainNumber)
{
	algorithmSendMsg(QStringLiteral("正在读取检测数据..."), 0);
	initialData();
	m_strFileSavePathRoot.replace("/", "\\");
	QString  strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\vibration1.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, v1); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\vibration2.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, v2); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\vibration3.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, v3); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\vibration4.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, v4); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\vibration5.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, v5); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\vibration6.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, v6); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\vibration7.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, v7); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\vibration8.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, v8); });

	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\magnetic1.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, g1); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\magnetic2.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, g2); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\magnetic3.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, g3); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\magnetic4.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, g4); });

	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\temperate1.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, t1); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\temperate2.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, t2); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\temperate3.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, t3); });
	strfialePath = m_strFileSavePathRoot + "simulate\\" + strTrainNumber + "\\VAIS\\temperate4.txt";
	QtConcurrent::run([=](){ readDataFromTxt(strfialePath, t4); });

	m_Semaphore.acquire(16);

	if (!v1.size() || !v2.size() || !v3.size() || !v4.size() || !v5.size() || !v6.size() || !v7.size() || !v8.size()
		|| !g1.size() || !g2.size() || !g3.size() || !g4.size() || !t1.size() || !t2.size() || !t3.size() || !t4.size())
	{
		algorithmSendMsg(QStringLiteral("存在数据为空，请检查数据源"), 1);
		return;
	}
	m_Semaphore.release(16);
	operateData(v1, v2, v3, v4, v5, v6, v7, v8, g1, g2, g3, g4, t1, t2, t3, t4);
}

void AlgorithmBase::dataCombination()
{
	algorithmSendMsg(QStringLiteral("检查结果数据"), 0);

#pragma region 检查数据，对缺失数据进行补全
	for (int i = 0; i < vibrationDataLeft.size(); i++)
	{
		if (vibrationDataLeft[i].size() < 24)
		{
			int n = 24 - vibrationDataLeft[i].size();
			for (int j = 0; j < n; j++)
			{
				vibrationDataLeft[i].push_back(-1000);
			}
		}

		if (vibrationDataRight[i].size() < 24)
		{
			int n = 24 - vibrationDataRight[i].size();
			for (int j = 0; j < n; j++)
			{
				vibrationDataRight[i].push_back(-1000);
			}
		}
	}

	for (int i = 0; i < axleDataLeft.size(); i++)
	{
		if (axleDataLeft[i].size() < 24)
		{
			int n = 24 - axleDataLeft[i].size();
			for (int j = 0; j < n; j++)
			{
				axleDataLeft[i].push_back(-1000);
			}
		}

		if (axleDataRight[i].size() < 24)
		{
			int n = 24 - axleDataRight[i].size();
			for (int j = 0; j < n; j++)
			{
				axleDataRight[i].push_back(-1000);
			}
		}
	}

	for (int i = 0; i < electricalDataLeft.size(); i++)
	{
		if (electricalDataLeft[i].size() < 8)
		{
			int n = 8 - electricalDataLeft[i].size();
			for (int j = 0; j < n; j++)
			{
				electricalDataLeft[i].push_back(-1000);
			}
		}

		if (electricalDataRight[i].size() < 8)
		{
			int n = 8 - electricalDataRight[i].size();
			for (int j = 0; j < n; j++)
			{
				electricalDataRight[i].push_back(-1000);
			}
		}
	}
#pragma endregion

	algorithmSendMsg(QStringLiteral("通知桌面显示数据"), 0);
	emit sendVibrationData();
	emit sendAxleData();
	emit sendElectricalData();
	algorithmSendMsg(QStringLiteral("开始写入数据库"), 0);
	writeDataToSql(vibrationDataLeft, axleDataLeft, electricalDataLeft, vibrationDataRight, axleDataRight, electricalDataRight);
	m_bIsRunAlg = false;
}

AlgorithmBase* AlgorithmBase::instance = NULL;
DEALALGORITHM_EXPORT AlgorithmBase* getAlgorithmInstance()
{
	if (NULL == AlgorithmBase::instance)
	{
		AlgorithmBase::instance = new AlgorithmBase();
	}
	return AlgorithmBase::instance;
}
