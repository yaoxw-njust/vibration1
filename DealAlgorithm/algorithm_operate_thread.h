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
	void preprocess();  //�Ÿ�Ԥ����
	void temperatureCalculate();  //�¶ȼ���
	void speedCalculate();  //�ٶȼ���
	void electricalTemperatureCalculate();  //����¶ȼ���
	void vibrationCalculate(); //�񶯼���
	void vibrationDivideWheel(const long int*, const long int*, const size_t);   //���ִ���
	void calculateEigenvalue(std::vector<double> data, std::vector<double> &RMS_4, std::vector<double> &Peak_4, std::vector<double> &Crest_factor_4,
		std::vector<double> &Kurtosis_4, std::vector<double> &Kurtosis_factor_4, std::vector<double> &SS, int nn);    //��������ֵ
	void clearData(); //������ݣ��ͷŻ���

signals:
	void sendResultData(int directionFlag);

private:
	std::vector<double> L1;     //4·�񶯴������ź�
	std::vector<double> L2;
	std::vector<double> L3;
	std::vector<double> L4;

	std::vector<double> G2;     //3·�Ÿ��ź�
	std::vector<double> G3;
	std::vector<double> G4;

	std::vector<double> T1;     //2·�¶��ź�
	std::vector<double> T2;

	std::vector<double> S;       //�񶯴�����������

	int m_nFlag;                 //���Ҳ��־λ


	std::vector<std::vector<double>> bearPalteSignalDivided;         //���ֺ����ж˸��¶��źţ�24 * x��
	
	std::vector<std::vector<double>> electricalFloorSignalDivided;   //���ֺ�ĵ���װ��¶��źţ�8 * x��
	

	std::vector<std::vector<double>> vibrationDivided1;              //���ֺ�����źţ�24 * x��
	std::vector<std::vector<double>> vibrationDivided2;
	std::vector<std::vector<double>> vibrationDivided3;
	std::vector<std::vector<double>> vibrationDivided4;


	std::vector<long int > m_nAxleMarkG2;  //G2,G3,G4�Ÿ���λ��ȡ
	std::vector<long int > m_nAxleMarkG3;
	std::vector<long int > m_nAxleMarkG4;
	std::vector<long int > m_nAxleMarkG2tmp;

	LogHelper *logHelper = NULL;

public:
	std::vector<std::vector<double>> vibrationEigenValue;            //������ֵ 5 * 24������������ֵ����ֵ���ӡ��Ͷȡ��Ͷ����ӣ�
	std::vector<std::vector<double>> bearPalteTemperatureDivided;    //��ж˸��¶�ֵ 3 * 24����ֵ����ֵ����������
	std::vector<std::vector<double>> electricalTemperatureDivided;   //����¶�ֵ 3 * 8����ֵ����ֵ�����������
	double m_dTrainSpeed = 0.0;
	double m_dEnvironmentTemp = 0.0;
};

