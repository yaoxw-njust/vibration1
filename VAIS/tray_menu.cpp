
#include "tray_menu.h"

TrayMenu::TrayMenu(QWidget *parent)
	: QMenu(parent)
{
	initActions();
}

TrayMenu::~TrayMenu()
{

}

// 添加菜单项
void TrayMenu::initActions()
{
	// 创建菜单项
	//m_pShowAction = new QAction( QString::fromLocal8Bit("显示"), this);
	m_pQuitAction = new QAction( QString::fromLocal8Bit("退出"), this);

	// 添加菜单项
	//addAction(m_pShowAction);
	//addSeparator();
	addAction(m_pQuitAction);

	// 连接信号-信号（仅列举连接显示窗口的信号）
	connect(m_pQuitAction, SIGNAL(triggered(bool)), this, SIGNAL(closeWindow()));
}