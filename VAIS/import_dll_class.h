#ifndef IMPORTDLLCLASS_H
#define IMPORTDLLCLASS_H

#include "..\DealFile\ini_helper.h"
#include "..\DealFile\log_helper.h"

#include "..\DealLogic\main_logic_tcp.h"

#include "..\DealAlgorithm\algorithm_base.h"



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

	MainLogicTcp *m_pMainLogicTcp = NULL;

	AlgorithmBase *m_pAlgorithmBase = NULL;


	static ImportDllClass *_instance;
	static ImportDllClass *GetInstance();

};


#endif // !IMPORTDLLCLASS_H
