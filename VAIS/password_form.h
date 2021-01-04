#pragma once

#include <QtWidgets>
#include <QLabel>
#include <QLineEdit>

class PasswordForm : public QWidget
{
	Q_OBJECT

public:
	PasswordForm(QWidget *parent = Q_NULLPTR);
	~PasswordForm();

private:
	void __setupUI();
	void keyReleaseEvent(QKeyEvent *event);
	private slots:
	void __confirmPassword();

signals:
	void confirmPassword();

private:
	QLabel *m_pPassInfo;
	QLineEdit *m_pPassEdit;


};
