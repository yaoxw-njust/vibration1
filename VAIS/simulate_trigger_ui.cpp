#include "simulate_trigger_ui.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDir>
#include "import_dll_class.h"
#include "..\DealFile\ini_helper.h"
#include "..\DealCommon\path_define_file.h"
#include "..\DealLogic\main_logic_tcp.h"

SimulateTriggerUI::SimulateTriggerUI(QWidget *parent)
	: QWidget(parent)
{ 
	__setupUI();
	__connectFuntion();
}

SimulateTriggerUI::~SimulateTriggerUI()
{
	if (NULL != m_pTableView)
	{
		delete m_pTableView;
	}
	m_pTableView = NULL;

	if (NULL != m_pTableModel)
	{
		delete m_pTableModel;
	}
	m_pTableModel = NULL;
}

void SimulateTriggerUI::__setupUI()
{
	SetGlobalParamPath(QApplication::applicationDirPath());
	QFont font;
	font.setBold(true);
	font.setPixelSize(15);
	this->setFont(font);
	this->resize(QSize(350, 350));
	this->setMinimumSize(QSize(300, 300));
	QPoint m_point;
	QRect rect = QApplication::desktop()->availableGeometry(); 
	m_point.setX(rect.width() - width()- 20);
	m_point.setY(rect.height() - height()-35);
	move(m_point.x(), m_point.y());

	QImage *mainImage = new QImage();
	mainImage->load(":/image/simu.net.ico");
	QIcon mainFormIcon(QPixmap::fromImage(*mainImage));
	this->setWindowIcon(mainFormIcon);
	this->setWindowTitle(QStringLiteral("模拟触发"));

	QHBoxLayout *m_pHLayout = new QHBoxLayout();
	QVBoxLayout *m_pVLeftLayout = new QVBoxLayout();
	QVBoxLayout *m_pVRightLayout = new QVBoxLayout();
	m_pTableView = new QTableView();
	font.setBold(false);
	m_pTableView->setFont(font);
	m_pVLeftLayout->addWidget(m_pTableView);
	__initialTableView();

	QLabel *m_pQLabel = new QLabel();
	m_pQLabel->setText(QStringLiteral("间隔时间(s)："));
	m_pQLabel->setFont(font);
	m_pVRightLayout->addSpacing(1000);
	m_pVRightLayout->addWidget(m_pQLabel);
	m_pTimeSpinBox = new QSpinBox();
	m_pTimeSpinBox->setFont(font);
	m_pTimeSpinBox->setFocusPolicy(Qt::NoFocus);
	m_pTimeSpinBox->setMinimum(0);
	m_pTimeSpinBox->setValue(2);     //初始值2
	m_pVRightLayout->addWidget(m_pTimeSpinBox);
	m_pVRightLayout->addSpacing(1000);
	m_pOnceBtn = new QPushButton();
	m_pOnceBtn->setText(QStringLiteral("单次执行"));
	m_pOnceBtn->setFont(font);
	m_pVRightLayout->addWidget(m_pOnceBtn);
	m_pVRightLayout->addSpacing(1000);
	m_pContinousBtn = new QPushButton();
	m_pContinousBtn->setText(QStringLiteral("连续执行"));
	m_pContinousBtn->setFont(font);
	m_pVRightLayout->addWidget(m_pContinousBtn);
	m_pVRightLayout->addSpacing(1000);
	m_pCircleBtn = new QPushButton();
	m_pCircleBtn->setText(QStringLiteral("循环执行"));
	m_pCircleBtn->setFont(font);
	m_pVRightLayout->addWidget(m_pCircleBtn);
	m_pVRightLayout->addSpacing(1000);
	m_pStopBtn = new QPushButton();
	m_pStopBtn->setText(QStringLiteral("停  止"));
	m_pStopBtn->setFont(font);
	m_pVRightLayout->addWidget(m_pStopBtn);
	m_pVRightLayout->addSpacing(1000);
	m_pHLayout->addLayout(m_pVLeftLayout);
	m_pHLayout->addLayout(m_pVRightLayout);
	this->setLayout(m_pHLayout);

}

void SimulateTriggerUI::__initialTableView()
{
	//准备数据模型
	m_pTableModel = new QStandardItemModel();
	m_pTableModel->setHorizontalHeaderItem(0, new QStandardItem(QStringLiteral("流水号文件夹")));
	
	//利用setModel()方法将数据模型与QTableView绑定
	m_pTableView->setModel(m_pTableModel);
	m_pTableView->verticalHeader()->hide();
	m_pTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	m_pTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

void SimulateTriggerUI::getAllSimuData()
{
	if (m_pTableView->model()->rowCount() > 0)
	{
		m_pTableView->model()->removeRows(0, m_pTableView->model()->rowCount());
	}
	IniHelper *inihelper = ImportDllClass::GetInstance()->m_pIniHelper;
	QString rootPath = inihelper->readIniStr("system", "file_root_path", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
	rootPath = rootPath + "simulate\\";
	QStringList m_foldelNamesLists;
	QDir dir(rootPath);
	rootPath = dir.fromNativeSeparators(rootPath);//  "\\"转为"/" 
	if (!dir.exists()) m_foldelNamesLists = QStringList("");
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	m_foldelNamesLists = dir.entryList();
	for (int i = 0; i < m_foldelNamesLists.size(); ++i)
	{
		QList<QStandardItem *> m_Standard = QList<QStandardItem *>() << new QStandardItem(m_foldelNamesLists[i]);
		m_pTableModel->appendRow(m_Standard);
	}
	m_pTableView->selectRow(0);
}

void SimulateTriggerUI::__connectFuntion()
{
	connect(m_pOnceBtn, SIGNAL(clicked()), this, SLOT(__onceRunBtnClicked()));
}

void SimulateTriggerUI::__onceRunBtnClicked()
{
	int selectRow = -1;
	selectRow = m_pTableView->currentIndex().row();
	if (-1 != selectRow)
	{
		QAbstractItemModel *model = m_pTableView->model();
		QModelIndex index = model->index(selectRow, 0);  
		QString strTrainNum = model->data(index).toString();
		if (selectRow < m_pTableView->model()->rowCount() - 1)
		{
			m_pTableView->selectRow(selectRow + 1);
		}
		else
		{
			m_pTableView->selectRow(0);
		}
		MainLogicTcp *mainLogic = ImportDllClass::GetInstance()->m_pMainLogicTcp;
		mainLogic->simulateTrigger(strTrainNum);
	}
}

void SimulateTriggerUI::closeWindow()
{
	this->close();
}


SimulateTriggerUI* SimulateTriggerUI::_instance = NULL;
SimulateTriggerUI* SimulateTriggerUI::getInstance()
{
	if (NULL == _instance)
	{
		_instance = new SimulateTriggerUI();
	}
	return _instance;
}