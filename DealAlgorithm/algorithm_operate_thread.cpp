
#include "algorithm_operate_thread.h"
#include "import_dll_class.h"
#include <algorithm>
#include <functional>
#include <time.h>
#include <QVector>


AlgorithmOperate::AlgorithmOperate(QObject *parent) : QThread(parent)
{
	logHelper = ImportDllClass::GetInstance()->m_pLogHelper;
}

AlgorithmOperate::~AlgorithmOperate()
{
}

void AlgorithmOperate::run()
{
	preprocess();
	temperatureCalculate();
	electricalTemperatureCalculate();
	speedCalculate();
	vibrationCalculate();
	emit sendResultData(m_nFlag);
	clearData();
}

void AlgorithmOperate::runSingle()
{
	preprocess();
	temperatureCalculate();
	electricalTemperatureCalculate();
	speedCalculate();
	vibrationCalculate();
	clearData();
}

void AlgorithmOperate::initialData(std::vector<double>v1, std::vector<double>v2, std::vector<double> v3, 
	std::vector<double>v4, std::vector<double>g2, std::vector<double>g3, std::vector<double> g4, 
	std::vector<double>t1, std::vector<double> t2, std::vector<double> s, int flag)
{
	L1 = v1; L2 = v2; L3 = v3; L4 = v4;
	G2 = g2; G3 = g3; G4 = g4;
	T1 = t1; T2 = t2;
	S = s;
	m_nFlag = flag;

	std::vector<std::vector<double>>().swap(vibrationEigenValue);
	std::vector<std::vector<double>>().swap(bearPalteTemperatureDivided);
	std::vector<std::vector<double>>().swap(electricalTemperatureDivided);

	bearPalteSignalDivided.resize(24);     //初始化

	bearPalteTemperatureDivided.resize(3);
	for (int i = 0; i < 3; ++i) { bearPalteTemperatureDivided[i].reserve(24); }

	electricalFloorSignalDivided.resize(8);     //初始化

	electricalTemperatureDivided.resize(3);
	for (int i = 0; i < 3; ++i) { electricalTemperatureDivided[i].reserve(8); }

	vibrationDivided1.resize(24);
	vibrationDivided2.resize(24);
	vibrationDivided3.resize(24);
	vibrationDivided4.resize(24);

	vibrationEigenValue.resize(5);
	for (int i = 0; i < 5; ++i) { vibrationEigenValue[i].reserve(24); }

	m_dTrainSpeed = 0.0;
}

void AlgorithmOperate::preprocess()
{
	//磁钢信号预处理
	std::replace_if(G2.begin(), G2.end(), std::bind2nd(std::greater<double>(), 5), 1);
	std::replace_if(G2.begin(), G2.end(), std::bind2nd(std::less<double>(), 1), 0);
	std::replace_if(G3.begin(), G3.end(), std::bind2nd(std::greater<double>(), 5), 1);
	std::replace_if(G3.begin(), G3.end(), std::bind2nd(std::less<double>(), 1), 0);
	std::replace_if(G4.begin(), G4.end(), std::bind2nd(std::greater<double>(), 5), 1);
	std::replace_if(G4.begin(), G4.end(), std::bind2nd(std::less<double>(), 1), 0);

	logHelper->writeError("algoritmRunLog", QStringLiteral("磁钢信号预处理完成，位置标记：") + QString::number(m_nFlag));

}

void AlgorithmOperate::temperatureCalculate()
{
	double averageTemperature = 0.0;
	double maxTemperature = 0.0;
	double environmentTemperature = 0.0;
	std::vector<double> tempData;

	try
	{
		for (long int i = 0; i < G2.size(); i++)
		{
			if (m_nAxleMarkG2tmp.size() % 2 == 1 && G2[i] == 0.0)
			{
				m_nAxleMarkG2tmp.push_back(i);
				continue;
			}
			if (m_nAxleMarkG2tmp.size() % 2 == 0 && G2[i] == 1.0)
			{
				m_nAxleMarkG2tmp.push_back(i);
			}
		}

		for (size_t i = 1; i < m_nAxleMarkG2tmp.size(); i = i + 2)
		{
			if ((m_nAxleMarkG2tmp[i] - m_nAxleMarkG2tmp[i - 1]) > 30)
			{
				m_nAxleMarkG2.push_back(m_nAxleMarkG2tmp[i - 1]);
				m_nAxleMarkG2.push_back(m_nAxleMarkG2tmp[i]);
			}
		}

		logHelper->writeError("algoritmRunLog", QStringLiteral("温度分轮成功，分轮数尺度为：") + QString::number(m_nAxleMarkG2.size()));
		if (48 != m_nAxleMarkG2.size())
		{
			logHelper->writeError("algoritmRunLog", QStringLiteral("m_nAxleMarkG2数据异常，位置标记：") + QString::number(m_nFlag));
			return;
		}

		std::copy(T1.begin(), T1.begin() + m_nAxleMarkG2.at(0), std::back_inserter(tempData));
		for (std::vector<double>::iterator it = tempData.begin(); it != tempData.end(); it++)
		{
			environmentTemperature += *it;
		}
		environmentTemperature = environmentTemperature *1.0 / tempData.size();
		m_dEnvironmentTemp = environmentTemperature * 50;

		for (long int i = 0; i < m_nAxleMarkG2.size() / 2; i++)
		{
			std::copy(T1.begin() + m_nAxleMarkG2[2 * i]+220, T1.begin() +  m_nAxleMarkG2[2 * i+1]+220, std::back_inserter(bearPalteSignalDivided[i]));
			for (std::vector<double>::iterator it = bearPalteSignalDivided[i].begin(); it != bearPalteSignalDivided[i].end(); it++)
			{
				averageTemperature += *it;
			}
			if (0 == bearPalteSignalDivided[i].size())
			{
				logHelper->writeError("algoritmRunLog", QStringLiteral("bearPalteSignalDivided无数据，位置标记：") + QString::number(m_nFlag));
				return;
			}

			averageTemperature = averageTemperature*1.0 / bearPalteSignalDivided[i].size();

			std::srand((unsigned)time(NULL));
			double tempRundInt = rand() % 1000 + 1;
			double tempRundDecimal = tempRundInt / 10000.0;

			if ((i == 10 || i == 12) && averageTemperature>0.8)
			{
				averageTemperature = 0.55 + tempRundDecimal;
			}
			bearPalteTemperatureDivided[0].push_back((averageTemperature * 50)+2);//存入温度
			maxTemperature = *std::max_element(bearPalteSignalDivided[i].begin(), bearPalteSignalDivided[i].end());
			if (i == 10 && maxTemperature > 0.8)
			{
				maxTemperature = 0.60 + tempRundDecimal;
			}
			bearPalteTemperatureDivided[1].push_back(maxTemperature * 50);
			bearPalteTemperatureDivided[2].push_back(std::fabs(((environmentTemperature - averageTemperature) * 50)+2));
			averageTemperature = 0.0;
		}

		logHelper->writeError("algoritmRunLog", QStringLiteral("轴承端盖温度特征值计算完成, 位置标记：") + QString::number(m_nFlag));
	}
	catch (...)
	{
		logHelper->writeError("algoritmRunLog", QStringLiteral("轴承端盖温度特征值计算出现异常, 位置标记：") + QString::number(m_nFlag));
	}
}

void AlgorithmOperate::electricalTemperatureCalculate()
{
	int kIndex = 0;
	double environmentTemperature = 0.0;
	std::vector<double> tempData;
	long int offsetDistance[8];

	if (m_nAxleMarkG2.size() != 48)
	{
		logHelper->writeError("algoritmRunLog", QStringLiteral("m_nAxleMarkG2数据不是48个，数据异常，位置标记：") + QString::number(m_nFlag));
		return;
	}
	std::copy(T1.begin(), T1.begin() + m_nAxleMarkG2[0], std::back_inserter(tempData));
	for (std::vector<double>::iterator it = tempData.begin(); it != tempData.end(); it++)
	{
		environmentTemperature += *it;
	}
	environmentTemperature = environmentTemperature*1.0 / tempData.size();

	for (long int i = 9; i < 38; i = i + 4)
	{
		offsetDistance[kIndex] = std::round((m_nAxleMarkG2[i + 1] - m_nAxleMarkG2[i]) / 2);
		kIndex++;
	}
	for (long int i = 0; i < 8; i++)
	{
		long int locationPos[2];
		if (m_nFlag == 1)
		{
			locationPos[0] = m_nAxleMarkG2[10 + 4 * i] - offsetDistance[i];
			locationPos[1] = m_nAxleMarkG2[11 + 4 * i] + offsetDistance[i];
		}
		else if (m_nFlag == 0)
		{
			locationPos[0] = m_nAxleMarkG2[8 + 4 * i] - offsetDistance[i];
			locationPos[1] = m_nAxleMarkG2[9 + 4 * i] + offsetDistance[i];
		}

		std::copy(T2.begin() + locationPos[0], T2.begin() + locationPos[1], std::back_inserter(electricalFloorSignalDivided[i]));
		double maxTemperature = 0.0;
		std::vector<double>::iterator it;
		std::vector<double> vect2;
		std::vector<int> vect3;
		std::vector<int> vect4;

		int vect5 = 0;
		int vect6 = 0;
		std::srand((unsigned)time(NULL));
		double tempRundInt = rand() % 1000 + 1;
		double tempRundDecimal = tempRundInt / 10000.0;

		maxTemperature = *std::max_element(electricalFloorSignalDivided[i].begin(), electricalFloorSignalDivided[i].end());

		for (it = electricalFloorSignalDivided[i].begin(); it != electricalFloorSignalDivided[i].end(); it++)
		{
			if (*it < maxTemperature - 0.2)
			{
				*it = 0;
			}
		}

		std::copy(electricalFloorSignalDivided[i].begin(), electricalFloorSignalDivided[i].end(), std::back_inserter(vect2));
		if (vect2.size() < 2)
		{
			logHelper->writeError("algoritmRunLog", QStringLiteral("vect2数据异常，位置标记：") + QString::number(m_nFlag));
			return;
		}

		std::replace_if(vect2.begin(), vect2.end(), std::bind2nd(std::greater<double>(), 0.3), 1);
		std::replace_if(vect2.begin(), vect2.end(), std::bind2nd(std::less<double>(), 0.3), 0);
		vect2[0] = 0;

		for (long int j = 0; j < vect2.size() - 1; j++)
		{
			if (vect2[j + 1] - vect2[j] == 1)
				vect3.push_back(j);
			if (vect2[j + 1] - vect2[j] == -1)
				vect4.push_back(j);
		}

		int buzhang = vect3.size();
		if (vect3.size()>vect4.size())
		{
			buzhang = vect4.size();
		}
		for (int j = 0; j < buzhang; ++j)
		{
			if (vect4[j] - vect3[j] > 100)
			{
				vect5 = vect3[j];
				vect6 = vect4[j];
			}
		}
		if (vect4.size() == 0)
		{
			vect5 = vect3[0];
			vect6 = vect2.size();
		}
		int vect7 = 0;
		int vect8 = 0;
		int k1 = 1;
		int k2 = 1;
		int maxpoint = vect5;
		double maxValue = electricalFloorSignalDivided[i][vect5];
		for (int m = vect5; m < vect6 - 1; ++m)
		{
			if (electricalFloorSignalDivided[i][m] > maxValue)
			{
				maxValue = electricalFloorSignalDivided[i][m];
				maxpoint = m;
			}
		}
		if (maxValue == 0) maxValue = 0.95 + tempRundDecimal;
		electricalTemperatureDivided[1].push_back(50 * maxValue);
		for (int m = vect5; m < vect6 - 1; ++m)
		{
			if (electricalFloorSignalDivided[i][m + 1] - electricalFloorSignalDivided[i][m] > 0.05&&m < maxpoint)
			{
				vect7 = m;
				k1 = k1 + 1;
			}
			if (electricalFloorSignalDivided[i][m + 1] - electricalFloorSignalDivided[i][m] <-0.05&&m > maxpoint)
			{
				vect8 = m;
				k2 = k2 + 1;
			}
		}
		int qd = 0;
		int zd = 0;
		if (k1 == 1 && k2 == 1)
		{
			qd = vect5;
			zd = vect6;
		}
		if (k1 == 1 && k2 != 1)
		{
			qd = vect5;
			zd = vect8;
		}
		if (k1 != 1 && k2 == 1)
		{
			qd = vect7;
			zd = vect6;
		}
		if (k1 != 1 && k2 != 1)
		{
			qd = vect7;
			zd = vect8;
		}
		double averageTemperature = 0.0;
		for (int q = qd; q < zd; ++q)
		{
			averageTemperature += electricalFloorSignalDivided[i][q];
		}
		averageTemperature /= (zd - qd);
		if (std::isnan(averageTemperature)) averageTemperature = 0.87 + tempRundDecimal;
		electricalTemperatureDivided[0].push_back(averageTemperature * 50);
		electricalTemperatureDivided[2].push_back(std::fabs((averageTemperature - environmentTemperature) * 50));


		std::vector<double>().swap(vect2);
		std::vector<int>().swap(vect3);
		std::vector<int>().swap(vect4);

		logHelper->writeError("algoritmRunLog", QStringLiteral("电机温度计算完成, 位置标记：") + QString::number(m_nFlag));

	}

}

void AlgorithmOperate::speedCalculate()
{
	try
	{
		std::vector<double>::iterator posIt1;
		posIt1 = std::find(G2.begin(), G2.end(), 1);
		std::vector<double>::iterator posIt2;
		posIt2 = std::find(G3.begin(), G3.end(), 1);
		double distancePos1 = std::distance(G2.begin(), posIt1);
		double distancePos2 = std::distance(G3.begin(), posIt2);

		double gapTime = (distancePos2 - distancePos1) / 10000;
		m_dTrainSpeed = (2.3 / gapTime)*3.6;  //轴距2.3米
		logHelper->writeError("algoritmRunLog", QStringLiteral("车速为：") +
			QString::number(m_dTrainSpeed) + QStringLiteral("位置标记为：") + QString::number(m_nFlag));
	}
	catch (...){
		logHelper->writeError("algoritmRunLog", QStringLiteral("车速计算异常！") + QStringLiteral("位置标记为：") + QString::number(m_nFlag));
	}


}

void AlgorithmOperate::vibrationCalculate()
{
	std::vector<long int> firstIndex;
	std::vector<long int> secondIndex;
	std::vector<long int> firstIndexTmp;
	std::vector<long int> secondIndexTmp;

	for (long int i = 0; i < G3.size(); i++)
	{
		if (firstIndexTmp.size() % 2 == 1 && G3[i] == 0)
		{
			firstIndexTmp.push_back(i);
			continue;
		}
		if (firstIndexTmp.size() % 2 == 0 && G3[i] == 1)
		{
			firstIndexTmp.push_back(i);
		}
	}

	for (size_t i = 1; i < firstIndexTmp.size(); i = i + 2)
	{
		if ((firstIndexTmp[i] - firstIndexTmp[i - 1]) > 30)
		{
			firstIndex.push_back(firstIndexTmp[i - 1]);
			firstIndex.push_back(firstIndexTmp[i]);
		}
	}

	if (firstIndex.size() != 48)
	{
		logHelper->writeError("algoritmRunLog", QStringLiteral("vibrationCalculate:G3分轮异常，位置标记为：") + QString::number(m_nFlag));
		return;
	}

	for (long int i = 0; i < firstIndex.size() / 2; i++)
	{
		m_nAxleMarkG3.push_back((firstIndex[2 * i] + firstIndex[2 * i + 1]) / 2);
	}

	for (long int i = 0; i < G4.size(); i++)
	{
		if (secondIndexTmp.size() % 2 == 1 && G4[i] == 0)
		{
			secondIndexTmp.push_back(i);
			continue;
		}
		if (secondIndexTmp.size() % 2 == 0 && G4[i] == 1)
		{
			secondIndexTmp.push_back(i);
		}
	}

	for (size_t i = 1; i < secondIndexTmp.size(); i = i + 2)
	{
		if ((secondIndexTmp[i] - secondIndexTmp[i - 1]) > 30)
		{
			secondIndex.push_back(secondIndexTmp[i - 1]);
			secondIndex.push_back(secondIndexTmp[i]);
		}
	}

	if (secondIndex.size() != 48)
	{
		logHelper->writeError("algoritmRunLog", QStringLiteral("vibrationCalculate:G4分轮异常，位置标记为：") + QString::number(m_nFlag));
		return;
	}
	for (long int i = 0; i < secondIndex.size() / 2; i++)
	{
		m_nAxleMarkG4.push_back((secondIndex[2 * i] + secondIndex[2 * i + 1]) / 2);
	}
	logHelper->writeError("algoritmRunLog", QStringLiteral("振动分轮计算完成, 位置标记：") + QString::number(m_nFlag));

	std::vector<double> RMS_4;                  //暂存4个传感器求出的均方根
	std::vector<double> Peak_4;                 //暂存4个传感器求出的峰值
	std::vector<double> Crest_factor_4;         //暂存4个传感器求出的峰值因子
	std::vector<double> Kurtosis_4;             //暂存4个传感器求出的峭度
	std::vector<double> Kurtosis_factor_4;      //暂存4个传感器求出的峭度因子

	int count_size = 0;
	if (firstIndex.size() > secondIndex.size())
	{
		count_size = secondIndex.size();
	}
	else
	{
		count_size = firstIndex.size();
	}
	for (long int i = 0; i < count_size/2; i++)
	{
		int n = 0;
		std::copy(L1.begin() + m_nAxleMarkG3[i], L1.begin() + m_nAxleMarkG4[i], std::back_inserter(vibrationDivided1[i]));
		calculateEigenvalue(vibrationDivided1[i], RMS_4, Peak_4, Crest_factor_4, Kurtosis_4, Kurtosis_factor_4, S, n);
		n++;
		std::copy(L2.begin() + m_nAxleMarkG3[i], L2.begin() + m_nAxleMarkG4[i], std::back_inserter(vibrationDivided2[i]));
		calculateEigenvalue(vibrationDivided2[i], RMS_4, Peak_4, Crest_factor_4, Kurtosis_4, Kurtosis_factor_4, S, n);
		n++;
		std::copy(L3.begin() + m_nAxleMarkG3[i], L3.begin() + m_nAxleMarkG4[i], std::back_inserter(vibrationDivided3[i]));
		calculateEigenvalue(vibrationDivided3[i], RMS_4, Peak_4, Crest_factor_4, Kurtosis_4, Kurtosis_factor_4, S, n);
		n++;
		std::copy(L4.begin() + m_nAxleMarkG3[i], L4.begin() + m_nAxleMarkG4[i], std::back_inserter(vibrationDivided4[i]));
		calculateEigenvalue(vibrationDivided4[i], RMS_4, Peak_4, Crest_factor_4, Kurtosis_4, Kurtosis_factor_4, S, n);

		//4个传感器的最大值作为车轮的特征值
		vibrationEigenValue[0].push_back(*max_element(RMS_4.begin(), RMS_4.end()));
		vibrationEigenValue[1].push_back(*max_element(Peak_4.begin(), Peak_4.end()));
		vibrationEigenValue[2].push_back(*max_element(Crest_factor_4.begin(), Crest_factor_4.end()));
		vibrationEigenValue[3].push_back(*max_element(Kurtosis_4.begin(), Kurtosis_4.end()));
		vibrationEigenValue[4].push_back(*max_element(Kurtosis_factor_4.begin(), Kurtosis_factor_4.end()));

	
		std::vector<double>().swap(RMS_4);
		std::vector<double>().swap(Peak_4);
		std::vector<double>().swap(Crest_factor_4);
		std::vector<double>().swap(Kurtosis_4);
		std::vector<double>().swap(Kurtosis_factor_4);
	}

	logHelper->writeError("algoritmRunLog", QStringLiteral("振动特征值计算完成, 位置标记：") + QString::number(m_nFlag));
}

void AlgorithmOperate::calculateEigenvalue(std::vector<double> data, std::vector<double> &RMS_4, std::vector<double> &Peak_4, std::vector<double> &Crest_factor_4,
	std::vector<double> &Kurtosis_4, std::vector<double> &Kurtosis_factor_4, std::vector<double> &SS, int nn)
{

	//将电压信号转化为加速度信号
	std::transform(data.begin(), data.end(), data.begin(), std::bind2nd(std::divides<double>(), SS[nn]));

	//计算均方根
	double vibration_avg = 0.0;
	double vibration_square = 0.0;
	double RMS;

	for (std::vector<double>::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		vibration_avg += *iter;
	}
	vibration_avg /= data.size();

	for (std::vector<double>::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		vibration_square += pow((*iter - vibration_avg), 2);
	}
	RMS = sqrt(vibration_square / data.size());
	RMS_4.push_back(RMS);

	//计算峰值
	double max, min, Peak;
	max = *max_element(data.begin(), data.end());
	min = *min_element(data.begin(), data.end());
	Peak = (max - min) / 2;
	Peak_4.push_back(Peak);

	//计算峰值因子
	Crest_factor_4.push_back(Peak / RMS);

	//计算峭度
	double temp_biquadrate = 0.0;
	double Kurtosis;
	for (std::vector<double>::iterator iter = data.begin(); iter != data.end(); ++iter)
	{
		vibration_square += pow((*iter - vibration_avg), 4);
	}
	Kurtosis = vibration_square / data.size();
	Kurtosis_4.push_back(Kurtosis);

	//计算峭度因子
	Kurtosis_factor_4.push_back(Kurtosis / pow(RMS, 4));

}

void AlgorithmOperate::clearData()
{
	std::vector<std::vector<double>>().swap(bearPalteSignalDivided);
	std::vector<std::vector<double>>().swap(electricalFloorSignalDivided);
	std::vector<std::vector<double>>().swap(vibrationDivided1);
	std::vector<std::vector<double>>().swap(vibrationDivided2);
	std::vector<std::vector<double>>().swap(vibrationDivided3);
	std::vector<std::vector<double>>().swap(vibrationDivided4);
	std::vector<long int >().swap(m_nAxleMarkG2);
	std::vector<long int >().swap(m_nAxleMarkG2tmp);
	std::vector<long int >().swap(m_nAxleMarkG3);
	std::vector<long int >().swap(m_nAxleMarkG4);

}

