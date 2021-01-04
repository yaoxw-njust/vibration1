#ifndef TRAYMENU_H
#define TRAYMENU_H


#include <QMenu>
#include <QObject>

class TrayMenu : public QMenu
{
	Q_OBJECT

public:
	explicit TrayMenu(QWidget *parent = 0);
	~TrayMenu();

signals:
	// 自定义信号
	void closeWindow();

private:
	// 添加菜单项
	void initActions();

private:
	//QAction *m_pShowAction;
	QAction *m_pQuitAction;
};


#endif // !TRAYMENU_H