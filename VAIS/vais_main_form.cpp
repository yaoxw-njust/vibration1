
#include "vais_main_form.h"
#include "import_dll_class.h"
#include "tray_menu.h"
#include "..\DealCommon\path_define_file.h"
#include <Windows.h>

#include <QVBoxLayout>
#include <QGridLayout>


#pragma comment(lib, "version.lib")

VAISMainForm::VAISMainForm(QWidget *parent) : QWidget(parent)
{
	__setupUI();
	__initialTableViewVibrate();
	__initialTableViewAxle();
	__initialTableViewElectrical();
	__initialTableViewLog();

	__initialLogicAndDevice();
	__connectFuntion();

}

VAISMainForm::~VAISMainForm()
{
}

void VAISMainForm::__setupUI()
{
	SetGlobalParamPath(QApplication::applicationDirPath());
	this->setMinimumSize(1200, 800);

	QImage *mainImage = new QImage();
	mainImage->load(":/image/VAIS.ico");
	QIcon mainFormIcon(QPixmap::fromImage(*mainImage));
	this->setWindowIcon(mainFormIcon);
	QString processName = "VAIS V" + __getAppVersion();
	this->setWindowTitle(processName);
	m_pSystemTrayIcon = new QSystemTrayIcon(this);
	m_pSystemTrayIcon->setToolTip(this->windowTitle());
	m_pSystemTrayIcon->setIcon(mainFormIcon);
	TrayMenu *m_pTrayMenu = new TrayMenu();

	// 设置系统托盘的上下文菜单
	m_pSystemTrayIcon->setContextMenu(m_pTrayMenu);

	// 连接信号槽
	connect(m_pTrayMenu, SIGNAL(closeWindow()), this, SLOT(close()));
	connect(m_pSystemTrayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
	m_pSystemTrayIcon->show();

	QLabel *m_pImageLable = new QLabel();
	QImage *img = new QImage();
	img->load(":/image/yunda.png");
	m_pImageLable->setPixmap(QPixmap::fromImage(*img));

	QLabel *m_pTitleLabel = new QLabel();
	m_pTitleLabel->setText(QStringLiteral("振 动 及 轴 温 检 测 系 统（VAIS）"));
	m_pTitleLabel->setAlignment(Qt::AlignCenter);
	QFont m_Font;;
	m_Font.setBold(true);
	m_Font.setPixelSize(30);
	m_pTitleLabel->setFont(m_Font);


	QLabel *m_pTitleLabelRight = new QLabel();

	QHBoxLayout *m_pHBoxLayoutTop = new QHBoxLayout();
	m_pHBoxLayoutTop->addWidget(m_pImageLable);
	m_pHBoxLayoutTop->addWidget(m_pTitleLabel);
	m_pHBoxLayoutTop->addWidget(m_pTitleLabelRight);
	m_pHBoxLayoutTop->setStretchFactor(m_pImageLable, 1);
	m_pHBoxLayoutTop->setStretchFactor(m_pTitleLabel, 3);
	m_pHBoxLayoutTop->setStretchFactor(m_pTitleLabelRight, 1);


	m_pTableViewVibrate = new QTableView();
	m_pTableViewAxle = new QTableView();
	m_pTableViewElectrical = new QTableView();

	m_Font.setPixelSize(15);
	QVBoxLayout *m_pVBoxLayoutMiddleL = new QVBoxLayout();
	QLabel *m_bLabel1 = new QLabel();
	m_bLabel1->setText(QStringLiteral("振动特征值："));
	m_bLabel1->setFont(m_Font);
	m_pVBoxLayoutMiddleL->addWidget(m_bLabel1, Qt::AlignLeft);
	m_pVBoxLayoutMiddleL->addWidget(m_pTableViewVibrate);
	m_pVBoxLayoutMiddleL->setStretchFactor(m_bLabel1, 1);
	m_pVBoxLayoutMiddleL->setStretchFactor(m_pTableViewVibrate, 1000);

	QVBoxLayout *m_pVBoxLayoutMiddleM = new QVBoxLayout();
	QLabel *m_bLabel2 = new QLabel();
	m_bLabel2->setText(QStringLiteral("轴承端盖温度特征值："));
	m_bLabel2->setFont(m_Font);
	m_pVBoxLayoutMiddleM->addWidget(m_bLabel2, Qt::AlignLeft);
	m_pVBoxLayoutMiddleM->addWidget(m_pTableViewAxle);
	m_pVBoxLayoutMiddleM->setStretchFactor(m_bLabel2, 1);
	m_pVBoxLayoutMiddleM->setStretchFactor(m_pTableViewAxle, 1000);

	QVBoxLayout *m_pVBoxLayoutMiddleR = new QVBoxLayout();
	QLabel *m_bLabel3 = new QLabel();
	m_bLabel3->setText(QStringLiteral("电机底板温度特征值："));
	m_bLabel3->setFont(m_Font);
	m_pVBoxLayoutMiddleR->addWidget(m_bLabel3, Qt::AlignLeft);
	m_pVBoxLayoutMiddleR->addWidget(m_pTableViewElectrical);
	m_pVBoxLayoutMiddleR->setStretchFactor(m_bLabel3, 1);
	m_pVBoxLayoutMiddleR->setStretchFactor(m_pTableViewElectrical, 1000);

	QHBoxLayout *m_pHBoxLayoutMiddle = new QHBoxLayout();
	m_pHBoxLayoutMiddle->addLayout(m_pVBoxLayoutMiddleL);
	m_pHBoxLayoutMiddle->addLayout(m_pVBoxLayoutMiddleM);
	m_pHBoxLayoutMiddle->addLayout(m_pVBoxLayoutMiddleR);
	m_pHBoxLayoutMiddle->setStretchFactor(m_pVBoxLayoutMiddleL, 2);
	m_pHBoxLayoutMiddle->setStretchFactor(m_pVBoxLayoutMiddleM, 1);
	m_pHBoxLayoutMiddle->setStretchFactor(m_pVBoxLayoutMiddleR, 1);

	m_pTableViewLog = new QTableView();
	QVBoxLayout *m_pVBoxLayoutBottom = new QVBoxLayout();
	QLabel *m_bLabel4 = new QLabel();
	m_bLabel4->setText(QStringLiteral("运行日志："));
	m_bLabel4->setFont(m_Font);
	m_pVBoxLayoutBottom->addWidget(m_bLabel4, Qt::AlignLeft);
	m_pVBoxLayoutBottom->addWidget(m_pTableViewLog);
	m_pVBoxLayoutBottom->setStretchFactor(m_bLabel4, 1);
	m_pVBoxLayoutBottom->setStretchFactor(m_pTableViewLog, 1000);

	m_pSimuBtn = new QPushButton();
	m_pSimuBtn->setText(QStringLiteral("模拟触发"));
	m_Font.setBold(true);
	m_Font.setPixelSize(25);
	m_pSimuBtn->setFont(m_Font);
	QHBoxLayout *m_pHBoxLayoutBottom = new QHBoxLayout();
	m_pHBoxLayoutBottom->addLayout(m_pVBoxLayoutBottom);
	m_pHBoxLayoutBottom->addWidget(m_pSimuBtn);

	m_Font.setBold(false);
	m_Font.setPixelSize(15);
	m_pTableViewVibrate->setFont(m_Font);
	m_pTableViewVibrate->horizontalHeader()->setFont(m_Font);
	m_pTableViewAxle->setFont(m_Font);
	m_pTableViewAxle->horizontalHeader()->setFont(m_Font);
	m_pTableViewElectrical->setFont(m_Font);
	m_pTableViewElectrical->horizontalHeader()->setFont(m_Font);
	m_pTableViewLog->setFont(m_Font);
	m_pTableViewLog->horizontalHeader()->setFont(m_Font);

	QVBoxLayout *m_pVBoxLayoutAll = new QVBoxLayout();
	m_pVBoxLayoutAll->addLayout(m_pHBoxLayoutTop);
	m_pVBoxLayoutAll->addLayout(m_pHBoxLayoutMiddle);
	m_pVBoxLayoutAll->addLayout(m_pHBoxLayoutBottom);
	m_pVBoxLayoutAll->setStretchFactor(m_pHBoxLayoutTop, 1);
	m_pVBoxLayoutAll->setStretchFactor(m_pHBoxLayoutMiddle, 50);
	m_pVBoxLayoutAll->setStretchFactor(m_pHBoxLayoutBottom, 30);

	this->setLayout(m_pVBoxLayoutAll);

}

void VAISMainForm::__initialTableViewVibrate()
{
	//准备数据模型
	m_pTableModelVibrate = new QStandardItemModel();
	m_pTableModelVibrate->setHorizontalHeaderItem(0, new QStandardItem(QStringLiteral("名称")));
	m_pTableModelVibrate->setHorizontalHeaderItem(1, new QStandardItem(QStringLiteral("均值")));
	m_pTableModelVibrate->setHorizontalHeaderItem(2, new QStandardItem(QStringLiteral("峰值")));
	m_pTableModelVibrate->setHorizontalHeaderItem(3, new QStandardItem(QStringLiteral("峰值因子")));
	m_pTableModelVibrate->setHorizontalHeaderItem(4, new QStandardItem(QStringLiteral("峭度")));
	m_pTableModelVibrate->setHorizontalHeaderItem(5, new QStandardItem(QStringLiteral("峭度因子")));

	//利用setModel()方法将数据模型与QTableView绑定
	m_pTableViewVibrate->setModel(m_pTableModelVibrate);
	m_pTableViewVibrate->verticalHeader()->hide();
	m_pTableViewVibrate->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_pTableViewVibrate->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_pTableViewVibrate->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	m_pTableViewVibrate->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	m_pTableViewVibrate->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
	m_pTableViewVibrate->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
	m_pTableViewVibrate->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTableViewVibrate->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void VAISMainForm::__initialTableViewAxle()
{
	//准备数据模型
	m_pTableModelAxle = new QStandardItemModel();
	m_pTableModelAxle->setHorizontalHeaderItem(0, new QStandardItem(QStringLiteral("名称")));
	m_pTableModelAxle->setHorizontalHeaderItem(1, new QStandardItem(QStringLiteral("均值")));
	m_pTableModelAxle->setHorizontalHeaderItem(2, new QStandardItem(QStringLiteral("峰值")));
	m_pTableModelAxle->setHorizontalHeaderItem(3, new QStandardItem(QStringLiteral("轴温升")));

	//利用setModel()方法将数据模型与QTableView绑定
	m_pTableViewAxle->setModel(m_pTableModelAxle);
	m_pTableViewAxle->verticalHeader()->hide();
	m_pTableViewAxle->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_pTableViewAxle->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_pTableViewAxle->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	m_pTableViewAxle->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	m_pTableViewAxle->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTableViewAxle->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void VAISMainForm::__initialTableViewElectrical()
{
	//准备数据模型
	m_pTableModelElectrical = new QStandardItemModel();
	m_pTableModelElectrical->setHorizontalHeaderItem(0, new QStandardItem(QStringLiteral("名称")));
	m_pTableModelElectrical->setHorizontalHeaderItem(1, new QStandardItem(QStringLiteral("均值")));
	m_pTableModelElectrical->setHorizontalHeaderItem(2, new QStandardItem(QStringLiteral("峰值")));
	m_pTableModelElectrical->setHorizontalHeaderItem(3, new QStandardItem(QStringLiteral("电机温升")));

	//利用setModel()方法将数据模型与QTableView绑定
	m_pTableViewElectrical->setModel(m_pTableModelElectrical);
	m_pTableViewElectrical->verticalHeader()->hide();
	m_pTableViewElectrical->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_pTableViewElectrical->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_pTableViewElectrical->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	m_pTableViewElectrical->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	m_pTableViewElectrical->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTableViewAxle->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void VAISMainForm::__initialTableViewLog()
{
	//准备数据模型
	m_pTableModelLog = new QStandardItemModel();
	m_pTableModelLog->setHorizontalHeaderItem(0, new QStandardItem(QStringLiteral("时间")));
	m_pTableModelLog->setHorizontalHeaderItem(1, new QStandardItem(QStringLiteral("内容")));

	//利用setModel()方法将数据模型与QTableView绑定
	m_pTableViewLog->setModel(m_pTableModelLog);
	m_pTableViewLog->verticalHeader()->hide();
	m_pTableViewLog->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_pTableViewLog->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_pTableViewLog->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTableViewLog->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void VAISMainForm::__updataVibrateData()
{
	m_pTableViewVibrate->model()->removeRows(0, m_pTableViewVibrate->model()->rowCount());
	QStringList strName;
	strName << "IA1" << "IA2" << "IA3" << "IA4" << "IA5" << "IA6" << "IA7" << "IA8" << "IB1" << "IB2" << "IB3"
		<< "IB4" << "IB5" << "IB6" << "IB7" << "IB8" << "IC1" << "IC2" << "IC3" << "IC4" << "IC5" << "IC6"
		<< "IC7" << "IC8" << "IIC8" << "IIC7" << "IIC6" << "IIC5" << "IIC4" << "IIC3" << "IIC2" << "IIC1" << "IIB8"
		<< "IIB7" << "IIB6" << "IIB5" << "IIB4" << "IIB3" << "IIB2" << "IIB1" << "IIA8" << "IIA7" << "IIA6" << "IIA5"
		<< "IIA4" << "IIA3" << "IIA2" << "IIA1";
	try
	{

		std::vector<std::vector<double>> vibrateDataL(m_pAlgorithmBase->vibrationDataLeft);
		std::vector<std::vector<double>> vibrateDataR(m_pAlgorithmBase->vibrationDataRight);
		for (int i = 0; i < vibrateDataL[0].size(); i ++)
		{
			QList<QStandardItem *> m_Standard = QList<QStandardItem *>();
			QStandardItem *m_pObjName = new QStandardItem();
			if (i*2 < strName.size())
			{
				m_pObjName->setText(strName[i*2]);
			}
			else
			{
				m_pObjName->setText("888");
			}
			m_pObjName->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pAverageValue = new QStandardItem();
			m_pAverageValue->setText(QString::number(vibrateDataL[0][i], 'f', 2));
			m_pAverageValue->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValue = new QStandardItem();
			m_pPeakValue->setText(QString::number(vibrateDataL[1][i], 'f', 2));
			m_pPeakValue->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValueFactor = new QStandardItem();
			m_pPeakValueFactor->setText(QString::number(vibrateDataL[2][i], 'f', 2));
			m_pPeakValueFactor->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pKurtosisValue = new QStandardItem();
			m_pKurtosisValue->setText(QString::number(vibrateDataL[3][i], 'f', 2));
			m_pKurtosisValue->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pKurtosisFactorValue = new QStandardItem();
			m_pKurtosisFactorValue->setText(QString::number(vibrateDataL[4][i], 'f', 2));
			m_pKurtosisFactorValue->setTextAlignment(Qt::AlignCenter);
			m_Standard << m_pObjName << m_pAverageValue << m_pPeakValue << m_pPeakValueFactor << m_pKurtosisValue << m_pKurtosisFactorValue;
			m_pTableModelVibrate->appendRow(m_Standard);
	
			QList<QStandardItem *> m_Standard1 = QList<QStandardItem *>();
			QStandardItem *m_pObjName1 = new QStandardItem();
			if (i * 2 < strName.size())
			{
				m_pObjName1->setText(strName[i * 2+1]);
			}
			else
			{
				m_pObjName1->setText("888");
			}
			m_pObjName1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pAverageValue1 = new QStandardItem();
			m_pAverageValue1->setText(QString::number(vibrateDataR[0][i], 'f', 2));
			m_pAverageValue1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValue1 = new QStandardItem();
			m_pPeakValue1->setText(QString::number(vibrateDataR[1][i], 'f', 2));
			m_pPeakValue1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValueFactor1 = new QStandardItem();
			m_pPeakValueFactor1->setText(QString::number(vibrateDataR[2][i], 'f', 2));
			m_pPeakValueFactor1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pKurtosisValue1 = new QStandardItem();
			m_pKurtosisValue1->setText(QString::number(vibrateDataR[3][i], 'f', 2));
			m_pKurtosisValue1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pKurtosisFactorValue1 = new QStandardItem();
			m_pKurtosisFactorValue1->setText(QString::number(vibrateDataR[4][i], 'f', 2));
			m_pKurtosisFactorValue1->setTextAlignment(Qt::AlignCenter);
			m_Standard1 << m_pObjName1 << m_pAverageValue1 << m_pPeakValue1 << m_pPeakValueFactor1 << m_pKurtosisValue1 << m_pKurtosisFactorValue1;
			m_pTableModelVibrate->appendRow(m_Standard1);
		}
	}
	catch (...)
	{
		__reportMsg(QStringLiteral("振动数据写入出错！"), 1);
	}
	qApp->processEvents();
}

void VAISMainForm::__updataAxleData()
{
	m_pTableViewAxle->model()->removeRows(0, m_pTableViewAxle->model()->rowCount());

	QStringList strName;
	strName << "IA1" << "IA2" << "IA3" << "IA4" << "IA5" << "IA6" << "IA7" << "IA8" << "IB1" << "IB2" << "IB3"
		<< "IB4" << "IB5" << "IB6" << "IB7" << "IB8" << "IC1" << "IC2" << "IC3" << "IC4" << "IC5" << "IC6"
		<< "IC7" << "IC8" << "IIC8" << "IIC7" << "IIC6" << "IIC5" << "IIC4" << "IIC3" << "IIC2" << "IIC1" << "IIB8"
		<< "IIB7" << "IIB6" << "IIB5" << "IIB4" << "IIB3" << "IIB2" << "IIB1" << "IIA8" << "IIA7" << "IIA6" << "IIA5"
		<< "IIA4" << "IIA3" << "IIA2" << "IIA1";
	try
	{
		std::vector<std::vector<double>> AxleDataL = m_pAlgorithmBase->axleDataLeft;
		std::vector<std::vector<double>> AxleDataR = m_pAlgorithmBase->axleDataRight;
		for (int i = 0; i < AxleDataL[0].size(); i++)
		{
			QList<QStandardItem *> m_Standard = QList<QStandardItem *>();
			QStandardItem *m_pObjName = new QStandardItem();
			if (i*2 < strName.size())
			{
				m_pObjName->setText(strName[i*2]);
			}
			else
			{
				m_pObjName->setText("888");
			}
			m_pObjName->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pAverageValue = new QStandardItem();
			m_pAverageValue->setText(QString::number(AxleDataL[0][i], 'f', 2));
			m_pAverageValue->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValue = new QStandardItem();
			m_pPeakValue->setText(QString::number(AxleDataL[1][i], 'f', 2));
			m_pPeakValue->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValueFactor = new QStandardItem();
			m_pPeakValueFactor->setText(QString::number(AxleDataL[2][i], 'f', 2));
			m_pPeakValueFactor->setTextAlignment(Qt::AlignCenter);
			m_Standard << m_pObjName << m_pAverageValue << m_pPeakValue << m_pPeakValueFactor;
			m_pTableModelAxle->appendRow(m_Standard);


			QList<QStandardItem *> m_Standard1 = QList<QStandardItem *>();
			QStandardItem *m_pObjName1 = new QStandardItem();
			if (i*2 < strName.size())
			{
				m_pObjName1->setText(strName[i*2+1]);
			}
			else
			{
				m_pObjName1->setText("888");
			}
			m_pObjName1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pAverageValue1 = new QStandardItem();
			m_pAverageValue1->setText(QString::number(AxleDataR[0][i], 'f', 2));
			m_pAverageValue1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValue1 = new QStandardItem();
			m_pPeakValue1->setText(QString::number(AxleDataR[1][i], 'f', 2));
			m_pPeakValue1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValueFactor1 = new QStandardItem();
			m_pPeakValueFactor1->setText(QString::number(AxleDataR[2][i], 'f', 2));
			m_pPeakValueFactor1->setTextAlignment(Qt::AlignCenter);
			m_Standard1 << m_pObjName1 << m_pAverageValue1 << m_pPeakValue1 << m_pPeakValueFactor1;
			m_pTableModelAxle->appendRow(m_Standard1);

		}
	}
	catch (...)
	{
		__reportMsg(QStringLiteral("轴温数据写入出错！"), 1);
	}
	qApp->processEvents();

}

void VAISMainForm::__updataElectricalData()
{
	m_pTableViewElectrical->model()->removeRows(0, m_pTableViewElectrical->model()->rowCount());

	QStringList strName;
	strName << "IB1" << "IB2" << "IB3" << "IB4" << "IC1" << "IC2" << "IC3" << "IC4" << "IIC4" << "IIC3" 
		<< "IIC2" << "IIC1"  << "IIB4" << "IIB3" << "IIB2" << "IIB1" ;
	try
	{
		std::vector<std::vector<double>> electriccalDataL = m_pAlgorithmBase->electricalDataLeft;
		std::vector<std::vector<double>> electriccalDataR = m_pAlgorithmBase->electricalDataRight;
		for (int i = 0; i < electriccalDataL[0].size(); i++)
		{
			QList<QStandardItem *> m_Standard = QList<QStandardItem *>();
			QStandardItem *m_pObjName = new QStandardItem();
			if (i*2 < strName.size())
			{
				m_pObjName->setText(strName[i*2]);
			}
			else
			{
				m_pObjName->setText("888");
			}
			m_pObjName->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pAverageValue = new QStandardItem();
			m_pAverageValue->setText(QString::number(electriccalDataL[0][i], 'f', 2));
			m_pAverageValue->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValue = new QStandardItem();
			m_pPeakValue->setText(QString::number(electriccalDataL[1][i], 'f', 2));
			m_pPeakValue->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pElectricalTemperateUp = new QStandardItem();
			m_pElectricalTemperateUp->setText(QString::number(electriccalDataL[2][i], 'f', 2));
			m_pElectricalTemperateUp->setTextAlignment(Qt::AlignCenter);
			m_Standard << m_pObjName << m_pAverageValue << m_pPeakValue << m_pElectricalTemperateUp;
			m_pTableModelElectrical->appendRow(m_Standard);


			QList<QStandardItem *> m_Standard1 = QList<QStandardItem *>();
			QStandardItem *m_pObjName1 = new QStandardItem();
			if (i*2 < strName.size())
			{
				m_pObjName1->setText(strName[i*2+1]);
			}
			else
			{
				m_pObjName1->setText("888");
			}
			m_pObjName1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pAverageValue1 = new QStandardItem();
			m_pAverageValue1->setText(QString::number(electriccalDataR[0][i], 'f', 2));
			m_pAverageValue1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pPeakValue1 = new QStandardItem();
			m_pPeakValue1->setText(QString::number(electriccalDataR[1][i], 'f', 2));
			m_pPeakValue1->setTextAlignment(Qt::AlignCenter);
			QStandardItem *m_pElectricalTemperateUp1 = new QStandardItem();
			m_pElectricalTemperateUp1->setText(QString::number(electriccalDataR[2][i], 'f', 2));
			m_pElectricalTemperateUp1->setTextAlignment(Qt::AlignCenter);
			m_Standard1 << m_pObjName1 << m_pAverageValue1 << m_pPeakValue1 << m_pElectricalTemperateUp1;
			m_pTableModelElectrical->appendRow(m_Standard1);

		}
	}
	catch (...)
	{
		__reportMsg(QStringLiteral("电机温度数据写入出错！"), 1);
	}
	qApp->processEvents();
}

void VAISMainForm::__reportMsg(QString msg, int alamFlag)
{
	QDateTime current_time = QDateTime::currentDateTime();
	QString strTime = current_time.toString("yyyy-MM-dd hh:mm:ss:zzz");
	QList<QStandardItem *> m_Standard = QList<QStandardItem *>() << new QStandardItem(strTime) << new QStandardItem(msg);
	m_pTableModelLog->appendRow(m_Standard);
	int tableViewCount = m_pTableViewLog->model()->rowCount();
	if (1 == alamFlag)
	{
		m_pTableModelLog->item(tableViewCount - 1, 0)->setForeground(QBrush(QColor(255, 0, 0)));
		m_pTableModelLog->item(tableViewCount - 1, 1)->setForeground(QBrush(QColor(255, 0, 0)));
	}

	if (tableViewCount > 500)
	{
		m_pTableViewLog->model()->removeRow(0);
	}
	m_pTableViewLog->scrollToBottom();
	LogHelper *logHelper = ImportDllClass::GetInstance()->m_pLogHelper;
	logHelper->writeError("Logic", msg);
	qApp->processEvents();
}

void VAISMainForm::__connectFuntion()
{
	connect(m_pTableViewLog, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(__useNotepadShowLog(const QModelIndex &)));
	connect(m_pSimuBtn, SIGNAL(clicked()), this, SLOT(__simulateBtnClicked()));

}

void VAISMainForm::__initialLogicAndDevice()
{
	m_pAlgorithmBase = ImportDllClass::GetInstance()->m_pAlgorithmBase;
	MainLogicTcp *mainLogicTcp = ImportDllClass::GetInstance()->m_pMainLogicTcp;

	connect(mainLogicTcp, SIGNAL(mainLogicSendMsg(QString, int)), this, SLOT(__reportMsg(QString, int)));
	connect(mainLogicTcp, SIGNAL(sendVibrationData()),this, SLOT(__updataVibrateData()));
	connect(mainLogicTcp, SIGNAL(sendAxleData()), this, SLOT(__updataAxleData()));
	connect(mainLogicTcp, SIGNAL(sendElectricalData()), this, SLOT(__updataElectricalData()));
	mainLogicTcp->initialConnection();
}

void VAISMainForm::__simulateBtnClicked()
{
	m_pSimulateTriggerUI = SimulateTriggerUI::getInstance();
	m_pSimulateTriggerUI->showNormal();
	m_pSimulateTriggerUI->activateWindow();
	m_pSimulateTriggerUI->getAllSimuData();
}

void VAISMainForm::__useNotepadShowLog(const QModelIndex &Index)
{
	if (Index.isValid())
	{
		QAbstractItemModel* m = (QAbstractItemModel *)Index.model();
		QModelIndex x = m->index(Index.row(), 0);
		QString s = x.data().toString();
		QStringList sl0 = s.split(" ");
		QStringList sl1 = sl0[1].split(":");
		QString logPath = QCoreApplication::applicationDirPath() + "\\Log\\" + sl0[0] + "\\" + sl1[0] + "\\Logic.log";
		logPath.replace("/", "\\");
		QProcess *notepadProc = new QProcess();
		QStringList qsl;
		qsl.append(logPath);
		notepadProc->start("notepad.exe", qsl);
	}
}

// 激活系统托盘
void VAISMainForm::onActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
		// 单击托盘显示窗口
	case QSystemTrayIcon::Trigger:
	{
		if (this->windowState() == Qt::WindowMinimized)
		{
			this->showMaximized();
			this->raise();
			this->activateWindow();
			this->setWindowState(Qt::WindowMaximized);
		}
		else
		{
			this->showMinimized();
			this->raise();
			this->activateWindow();
			this->setWindowState(Qt::WindowMinimized);
		}
		break;
	}
	// 双击
	case QSystemTrayIcon::DoubleClick:
	{
		// ...
		break;
	}
	default:
		break;
	}
}

void VAISMainForm::closeEvent(QCloseEvent *event)
{
	if (m_bIsCloseWindow)
	{
		m_bAllThreadNeadRun = false;
		if (NULL != m_pSimulateTriggerUI)
		{
			m_pSimulateTriggerUI->closeWindow();
		}
		event->accept();
	}
	else
	{
		if (m_pPasswordForm == NULL)
		{
			m_pPasswordForm = new PasswordForm();
			connect(m_pPasswordForm, SIGNAL(confirmPassword()), this, SLOT(__quitApp()));
			m_pPasswordForm->setWindowModality(Qt::ApplicationModal);
		}
		m_pPasswordForm->show();
		m_pPasswordForm->topLevelWidget();
		event->ignore();
	}

}

void VAISMainForm::__quitApp()
{
	m_bIsCloseWindow = true;
	this->close();
}

QString VAISMainForm::__getAppVersion()
{

	std::string r = "";
	TCHAR exepath[MAX_PATH] = { 0 };
	DWORD dwRet = GetModuleFileName(NULL, exepath, MAX_PATH);

	UINT sz = GetFileVersionInfoSize(exepath, 0);
	if (sz != 0) {
		r.resize(sz, 0);
		char *pBuf = NULL;
		pBuf = new char[sz];
		VS_FIXEDFILEINFO *pVsInfo;
		if (GetFileVersionInfo(exepath, 0, sz, pBuf)) {
			if (VerQueryValue(pBuf, L"\\", (void **)&pVsInfo, &sz)) {
				sprintf(pBuf, "%d.%d.%d.%d", HIWORD(pVsInfo->dwFileVersionMS), LOWORD(pVsInfo->dwFileVersionMS), HIWORD(pVsInfo->dwFileVersionLS), LOWORD(pVsInfo->dwFileVersionLS));
				r = pBuf;
			}
		}
		delete pBuf;
	}

	return QString::fromStdString(r);

}
