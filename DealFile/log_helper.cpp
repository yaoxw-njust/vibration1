#include "log_helper.h"

#include <QDateTime>
#include <QTime>
#include <Qdir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <iostream>



QString LogHelper::createDayFile(QString pathBase)
{
	mt_Log.lock();
	try
	{
		//��ȡ��ǰ����
		QDateTime dt = QDateTime::currentDateTime();
		//��ʱ������
		QString Year = QString::number(dt.date().year());
		QString Month = QString::number(dt.date().month());
		if (Month.length() < 2)
		{
			Month = "0" + Month;
		}
		QString Day = QString::number(dt.date().day());
		if (Day.length() < 2)
		{
			Day = "0" + Day;
		}
		QString path = pathBase + Year + "-" + Month + "-" + Day + "\\";

		//�����༶�ļ���
		__makeMultiDir(path);
		mt_Log.unlock();
		return path;
	}
	catch (const std::exception &ex)
	{
		writeError("LogWrite", ex.what());
		mt_Log.unlock();
		return "";
	}

}

QString LogHelper::createHourFile(QString pathBase)
{
	mt_Log.lock();
	try
	{
		//��ȡ��ǰ����
		QTime dt = QTime::currentTime();
		//��ʱ������
		QString Hour = QString::number(dt.hour());
		if (Hour.length() < 2)
		{
			Hour = "0" + Hour;
		}
		QString path = pathBase + Hour + "\\";

		//�����༶�ļ���
		__makeMultiDir(path);
		mt_Log.unlock();
		return path;
	}
	catch (const std::exception &ex)
	{
		writeError("LogWrite", ex.what());
		mt_Log.unlock();
		return "";
	}
	
}

QString LogHelper::createAllTimeFile(QString pathBase)
{
	QString dataPath = createDayFile(pathBase);
	QString hourPath = createHourFile(dataPath);
	return hourPath;
}

void  LogHelper::writeError(QString strName, QString strError)
{
	mt_WriteError.lock();
	try
	{
		//��ȡ��ǰ����
		QDateTime dt = QDateTime::currentDateTime();
		QString strPath = createAllTimeFile(m_strAllSoftGrabLogPath);
		//�ļ�
		QString strFileLog = strPath + strName + ".log";
		QFile file;
		QTextStream textStream;
		file.setFileName(strFileLog);
		file.open(QIODevice::WriteOnly | QIODevice::Append);
		textStream.setDevice(&file);
		textStream << dt.toString("yyyy-MM-dd hh:mm:ss:zzz") << "  " << strError << endl;
		file.close();
		mt_WriteError.unlock();
	}
	catch (const std::exception & ex)
	{
		mt_WriteError.unlock();
		return;
	}
}

QString LogHelper::__makeMultiDir(QString dirPath)
{
	QDir dir(dirPath);
	if (dir.exists(dirPath)) 
	{
		return dirPath;
	}
	QString parentDir = __makeMultiDir(dirPath.mid(0, dirPath.lastIndexOf('\\')));
	QString dirname = dirPath.mid(dirPath.lastIndexOf('\\') + 1);
	QDir parentPath(parentDir);
	if (!dirname.isEmpty())
		parentPath.mkpath(dirname);
	return parentDir + "\\" + dirname;;
}



LogHelper* LogHelper::_instance = NULL;
DEALFILE_EXPORT LogHelper *LogInstance()
{
	if (LogHelper::_instance == NULL)
	{
		LogHelper::_instance = new LogHelper();
	}
	return LogHelper::_instance;
}

