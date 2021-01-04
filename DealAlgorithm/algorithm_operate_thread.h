#pragma once

#include <QObject>
#include <QThread>

#include <iostream>

#include "..\DealFile\log_helper.h"

class AlgorithmOperate : public QThread
{
	Q_OBJECT

public:
	AlgorithmOperate(QObject *parent=Q_NULLPTR);
	~AlgorithmOperate();

	void initialData(std::vector<double>v1, std::vector<double>v2, std::vector<double> v3, std::vector<double>v4,
		std::vector<double>g2, std::vector<double>g3, std::vector<double> g4, std::vector<double>t1,
		std::vector<double> t2, std::vector<double> s, int flag);
	void run();
	void runSingle();

private:
	void preprocess();  //磁钢预处理
	void temperatureCalculate();  //温度计算
	void speedCalculate();  //速度计算
	void electricalTemperatureCalculate();  //电机温度计算
	void vibrationCalculate(); //振动计算
	void vibrationDivideWheel(const long int*, const long int*, const size_t);   //分轮处理
	void calculateEigenvalue(std::vector<double> data, std::vector<double> &RMS_4, std::vector<double> &Peak_4, std::vector<double> &Crest_factor_4,
		std::vector<double> &Kurtosis_4, std::vector<double> &Kurtosis_factor_4, std::vector<double> &SS, int nn);    //计算特征值
	void clearData(); //清除数据，释放缓存

signals:
	void sendResultData(int directionFlag);

private:
	std::vector<double> L1;     //4路振动传感器信号
	std::vector<double> L2;
	std::vector<double> L3;
	std::vector<double> L4;

	std::vector<double> G2;     //3路磁钢信号
	std::vector<double> G3;
	std::vector<double> G4;

	std::vector<double> T1;     //2路温度信号
	std::vector<double> T2;

	std::vector<double> S;       //振动传感器灵敏度

	int m_nFlag;                 //左右侧标志位


	std::vector<std::vector<double>> bearPalteSignalDivided;         //分轮后的轴承端盖温度信号（24 * x）
	
	std::vector<std::vector<double>> electricalFloorSignalDivided;   //分轮后的电机底板温度信号（8 * x）
	

	std::vector<std::vector<double>> vibrationDivided1;              //分轮后的振动信号（24 * x）
	std::vector<std::vector<double>> vibrationDivided2;
	std::vector<std::vector<double>> vibrationDivided3;
	std::vector<std::vector<double>> vibrationDivided4;


	std::vector<long int > m_nAxleMarkG2;  //G2,G3,G4磁钢轴位提取
	std::vector<long int > m_nAxleMarkG3;
	std::vector<long int > m_nAxleMarkG4;
	std::vector<long int > m_nAxleMarkG2tmp;

	LogHelper *logHelper = NULL;

public:
	std::vector<std::vector<double>> vibrationEigenValue;            //振动特征值 5 * 24（均方根、峰值、峰值因子、峭度、峭度因子）
	std::vector<std::vector<double>> bearPalteTemperatureDivided;    //轴承端盖温度值 3 * 24（均值、峰值、轴温升）
	std::vector<std::vector<double>> electricalTemperatureDivided;   //电机温度值 3 * 8（均值、峰值、电机温升）
	double m_dTrainSpeed = 0.0;
	double m_dEnvironmentTemp = 0.0;
};

