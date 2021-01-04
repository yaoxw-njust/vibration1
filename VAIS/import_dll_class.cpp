
#include "import_dll_class.h"

ImportDllClass::ImportDllClass()
{
	typedef IniHelper* (*IniHelperFun)();
	typedef LogHelper* (*LogHelperFun)();
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
	}

	typedef MainLogicTcp* (*MainLogicTcpFun)();
	QLibrary *DealLogicDll = new QLibrary("DealLogic.dll");
	if (DealLogicDll->load())
	{
		MainLogicTcpFun mainLogicTcp = (MainLogicTcpFun)DealLogicDll->resolve("getMainLogicTcpInstance");
		if (mainLogicTcp)
		{
			m_pMainLogicTcp = mainLogicTcp();
		}
	}

	typedef AlgorithmBase* (*AlgorithmBaseFun)();
	QLibrary *DealAlgorithmDll = new QLibrary("DealAlgorithm.dll");
	if (DealAlgorithmDll->load())
	{
		AlgorithmBaseFun algorithmBase = (AlgorithmBaseFun)DealAlgorithmDll->resolve("getAlgorithmInstance");
		if (algorithmBase)
		{
			m_pAlgorithmBase = algorithmBase();
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