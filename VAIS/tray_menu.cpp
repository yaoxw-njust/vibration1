
#include "tray_menu.h"

TrayMenu::TrayMenu(QWidget *parent)
	: QMenu(parent)
{
	initActions();
}

TrayMenu::~TrayMenu()
{

}

// ��Ӳ˵���
void TrayMenu::initActions()
{
	// �����˵���
	//m_pShowAction = new QAction( QString::fromLocal8Bit("��ʾ"), this);
	m_pQuitAction = new QAction( QString::fromLocal8Bit("�˳�"), this);

	// ��Ӳ˵���
	//addAction(m_pShowAction);
	//addSeparator();
	addAction(m_pQuitAction);

	// �����ź�-�źţ����о�������ʾ���ڵ��źţ�
	connect(m_pQuitAction, SIGNAL(triggered(bool)), this, SIGNAL(closeWindow()));
}