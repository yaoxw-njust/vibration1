#ifndef IMPORTDLLCLASS_H
#define IMPORTDLLCLASS_H

#include "..\DealFile\ini_helper.h"
#include "..\DealFile\log_helper.h"

#include "..\DealFile\mysql_helper.h"


#include <QObject>
#include <QLibrary>


class  ImportDllClass
{
public:
	 ImportDllClass();
	 ~ImportDllClass();


public:

	IniHelper *m_pIniHelper = NULL;
	LogHelper *m_pLogHelper = NULL;

	MySqlHelper *m_pMySqlHelper = NULL;

	static ImportDllClass *_instance;
	static ImportDllClass *GetInstance();

};


#endif // !IMPORTDLLCLASS_H
