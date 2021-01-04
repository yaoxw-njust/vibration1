
#include "import_dll_class.h"

ImportDllClass::ImportDllClass()
{
	typedef IniHelper* (*IniHelperFun)();
	typedef LogHelper* (*LogHelperFun)();
	typedef MySqlHelper* (*MySqlHelperFun)();
	QLibrary *DealFileDll = new QLibrary("DealFile.dll");
	if (DealFileDll->load())
	{
		IniHelperFun iniHelperFun = (IniHelperFun)DealFileDll->resolve("IniInstance");
		if (iniHelperFun)
		{
			m_pIniHelper = iniHelperFun();
		}
		LogHelperFun logHelperFun = (LogHelperFun)DealFileDll->resolve("LogInstance");
		if (logHelperFun)
		{
			m_pLogHelper = logHelperFun();
		}
		MySqlHelperFun mysqlHelperFun = (MySqlHelperFun)DealFileDll->resolve("MySqlInstance");
		if (mysqlHelperFun)
		{
			m_pMySqlHelper = mysqlHelperFun();
		}
	}

}
ImportDllClass::~ImportDllClass()
{
}

ImportDllClass* ImportDllClass::_instance = NULL;
ImportDllClass* ImportDllClass::GetInstance()
{
	if (NULL == ImportDllClass::_instance)
	{
		ImportDllClass::_instance = new ImportDllClass();
	}
	return ImportDllClass::_instance;
}