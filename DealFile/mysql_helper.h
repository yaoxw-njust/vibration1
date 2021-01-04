#pragma once

#include "dealfile_global.h"
#include <QSqlDatabase>
#include <QtSql>
#include <QSqlDriver>
#include <QObject>
#include "..\DealFile\log_helper.h"

class DEALFILE_EXPORT MySqlHelper:QObject
{
	Q_OBJECT
public:
	MySqlHelper();
	~MySqlHelper();

	virtual bool openSql();
	virtual bool writeSqlData(QString sheetName, QStringList key, QStringList value);
	virtual bool closeSql();
	virtual bool readSqlValue(QString sheetName, QString key, QString &value, QString filter="");
	virtual bool updataValue(QString sheetName, QStringList key, QStringList value, QString filter="");


private:
	QSqlTableModel *m_pModel = NULL;
	QSqlDatabase m_Database;
	LogHelper *m_pLogHelper = NULL;


public:
	static MySqlHelper *_instance;
};

extern "C"
{

	DEALFILE_EXPORT MySqlHelper *MySqlInstance();
}
