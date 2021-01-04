#pragma once

#include <QtWidgets>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QPushButton>
#include <QSpinBox>

class SimulateTriggerUI : public QWidget
{
	Q_OBJECT

public:
	SimulateTriggerUI(QWidget *parent = Q_NULLPTR);
	~SimulateTriggerUI();

	void getAllSimuData();
	static SimulateTriggerUI* getInstance();
	void closeWindow();

private:
	void __setupUI();
	void __initialTableView();
	void __connectFuntion();
	
	private slots:
	void __onceRunBtnClicked();
	


private:
	QTableView *m_pTableView = NULL;
	QStandardItemModel *m_pTableModel = NULL;

	QSpinBox *m_pTimeSpinBox = NULL;
	QPushButton *m_pOnceBtn = NULL;
	QPushButton *m_pContinousBtn = NULL;
	QPushButton *m_pCircleBtn = NULL;
	QPushButton *m_pStopBtn = NULL;

	static SimulateTriggerUI *_instance;

};
