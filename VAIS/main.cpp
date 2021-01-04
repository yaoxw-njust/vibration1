#include "vais_main_form.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle(QStyleFactory::create("Fusion"));
	qApp->setStyleSheet("QPushButton{color:#ffffff;border: 3px solid #8f8f91;border-radius: 8px;background-color: #2a2a2b;}"
		"QPushButton:pressed{color:#000000;background-color:#00c0ff;}");
	QSharedMemory shared_memory;
	shared_memory.setKey(QString("YUNDA-VAIS"));//设置固定共享内存段的key值
	if (shared_memory.attach())   //尝试将进程附加到该共享内存段
	{
		return 0;
	}

	if (shared_memory.create(1)) //创建1byte的共享内存段
	{
		VAISMainForm w;
		w.showMaximized();
		return a.exec();
	}
	return 0;
}
