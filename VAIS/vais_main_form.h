#pragma once

#include <QtWidgets>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>

#include "password_form.h"
#include "simulate_trigger_ui.h"
#include <QVector>
#include "..\DealAlgorithm\algorithm_base.h"

class VAISMainForm : public QWidget
{
	Q_OBJECT

public:
	VAISMainForm(QWidget *parent=Q_NULLPTR);
	~VAISMainForm();

private:
	void __setupUI();
	QString __getAppVersion();
	void closeEvent(QCloseEvent *event);
	void __initialTableViewVibrate();
	void __initialTableViewAxle();
	void __initialTableViewElectrical();
	void __initialTableViewLog();
	void __connectFuntion();
	void __initialLogicAndDevice();

	private slots:
	void onActivated(QSystemTrayIcon::ActivationReason reason);
	void __quitApp();
	void __useNotepadShowLog(const QModelIndex &Index);
	void __reportMsg(QString msg, int alamFlag);
	void __updataVibrateData();
	void __updataAxleData();
	void __updataElectricalData();
	void __simulateBtnClicked();


signals:
	void sendMainFormClose();

private:
	QSystemTrayIcon *m_pSystemTrayIcon = NULL;
	QSystemTrayIcon *pSystemTray = NULL;
	PasswordForm *m_pPasswordForm = NULL;

	QPushButton *m_pSimuBtn = NULL;
	SimulateTriggerUI *m_pSimulateTriggerUI = NULL;

	//四个表格控件在主窗口负责显示
	QTableView *m_pTableViewVibrate = NULL;
	QStandardItemModel *m_pTableModelVibrate = NULL;
	QTableView *m_pTableViewAxle = NULL;
	QStandardItemModel *m_pTableModelAxle = NULL;
	QTableView *m_pTableViewElectrical = NULL;
	QStandardItemModel *m_pTableModelElectrical = NULL;
	QTableView *m_pTableViewLog = NULL;
	QStandardItemModel *m_pTableModelLog = NULL;
	//////////////////////////////////////////////

	AlgorithmBase *m_pAlgorithmBase = NULL;
	bool m_bIsCloseWindow = false;

};
