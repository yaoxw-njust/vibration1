
#include "mysql_helper.h"
#include "..\DealFile\ini_helper.h"
#include "..\DealCommon\path_define_file.h"

MySqlHelper::MySqlHelper()
{
	m_pLogHelper = new LogHelper();
}

MySqlHelper::~MySqlHelper()
{
	if (NULL != m_pModel)
	{
		delete m_pModel;
	}
	m_pModel = NULL;


}

bool MySqlHelper::openSql()
{
	try
	{
		IniHelper *inihelper = IniInstance();
		QString mySqlHostName = inihelper->readIniStr("system", "sql_host_name", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
		QString mySqlDatabaseName = inihelper->readIniStr("system", "sql_database_name", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
		QString mySqlAccount = inihelper->readIniStr("system", "sql_account", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
		QString mySqlPassword = inihelper->readIniStr("system", "sql_password", m_strAllSoftGrabParamPath + "LogicParam\\SystemParam.ini");
		m_Database = QSqlDatabase::addDatabase("QMYSQL");
		m_Database.setHostName(mySqlHostName);
		m_Database.setDatabaseName(mySqlDatabaseName);
		m_Database.setUserName(mySqlAccount);
		m_Database.setPassword(mySqlPassword);
		if (m_Database.open())
		{
			m_pModel = new QSqlTableModel(this, m_Database);
			return true;
		}
		else
		{
			m_pLogHelper->writeError("MySqlErr", m_Database.lastError().text());
			return false;
		}
	}
	catch (...)
	{
		return false;
	}
}

bool MySqlHelper::writeSqlData(QString sheetName, QStringList key, QStringList value)
{

	if (NULL == m_pModel)
	{
		return false;
	}
	m_pModel->setTable(sheetName);
	QSqlRecord m_record = m_pModel->record();
	for (int i = 0; i < key.length(); i++)
	{
		m_record.setValue(key[i], value[i]);
	}
	if (!m_pModel->insertRecord(-1, m_record))
	{
		return false;
	}
	if (m_pModel->submitAll())
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool MySqlHelper::readSqlValue(QString sheetName, QString key, QString &value, QString filter)
{
	if (NULL == m_pModel)
	{
		return false;
	}
	try
	{
		m_pModel->setTable(sheetName);
		if ("" != filter)
		{
			m_pModel->setFilter(filter);
		}
		QSqlRecord m_record = m_pModel->record();
		m_pModel->select();
		int nCount = m_pModel->rowCount();
		value = m_pModel->record(nCount-1).value(key).toString();
		return true;
	}
	catch (...)
	{
		return false;
	}

}

bool MySqlHelper::updataValue(QString sheetName, QStringList key, QStringList value, QString filter)
{
	if (NULL == m_pModel)
	{
		return false;
	}
	try
	{
		m_pModel->setTable(sheetName);
		if ("" != filter)
		{
			m_pModel->setFilter(filter);
		}
		m_pModel->select();
		int nCount = m_pModel->rowCount();
		QSqlRecord m_record = m_pModel->record(nCount - 1);
		for (int i = 0; i < key.length(); i++)
		{
			m_record.setValue(key[i], value[i]);
		}
		if (!m_pModel->setRecord(nCount - 1, m_record))
		{
			return false;
		}
		if (m_pModel->submitAll())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	catch (...)
	{
		return false;
	}
}

bool MySqlHelper::closeSql()
{
	try
	{
		m_Database.close();
		if (NULL != m_pModel)
		{
			delete m_pModel;
		}
		m_pModel = NULL;
		return true;
	}
	catch (...)
	{
		return false;
	}
}


MySqlHelper *MySqlHelper::_instance = NULL;
DEALFILE_EXPORT MySqlHelper *MySqlInstance()
{
	if (MySqlHelper::_instance == NULL)
	{
		MySqlHelper::_instance = new MySqlHelper();
	}
	return MySqlHelper::_instance;
}