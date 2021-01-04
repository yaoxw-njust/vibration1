#include "vais_main_form.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle(QStyleFactory::create("Fusion"));
	qApp->setStyleSheet("QPushButton{color:#ffffff;border: 3px solid #8f8f91;border-radius: 8px;background-color: #2a2a2b;}"
		"QPushButton:pressed{color:#000000;background-color:#00c0ff;}");
	QSharedMemory shared_memory;
	shared_memory.setKey(QString("YUNDA-VAIS"));//���ù̶������ڴ�ε�keyֵ
	if (shared_memory.attach())   //���Խ����̸��ӵ��ù����ڴ��
	{
		return 0;
	}

	if (shared_memory.create(1)) //����1byte�Ĺ����ڴ��
	{
		VAISMainForm w;
		w.showMaximized();
		return a.exec();
	}
	return 0;
}
