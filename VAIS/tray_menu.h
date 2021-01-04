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
	// �Զ����ź�
	void closeWindow();

private:
	// ��Ӳ˵���
	void initActions();

private:
	//QAction *m_pShowAction;
	QAction *m_pQuitAction;
};


#endif // !TRAYMENU_H