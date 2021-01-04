#pragma once

#include "password_form.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

PasswordForm::PasswordForm(QWidget *parent)
	: QWidget(parent)
{
	__setupUI();
}

PasswordForm::~PasswordForm()
{
}

void PasswordForm::__setupUI()
{
	this->setFixedSize(350, 200);
	this->setWindowTitle(QStringLiteral("退出密码"));
	QFont m_Font = this->font();
	m_Font.setPixelSize(15);
	this->setFont(m_Font);

	QImage *mainImage = new QImage();
	mainImage->load(":/image/password.ico");
	QIcon mainFormIcon(QPixmap::fromImage(*mainImage));
	this->setWindowIcon(mainFormIcon);

	m_pPassInfo = new QLabel();
	m_pPassInfo->setMaximumHeight(25);
	QLabel *m_pLabel1 = new QLabel();
	m_pLabel1->setText(QStringLiteral("密码："));
	m_pPassEdit = new QLineEdit();
	m_Font.setPixelSize(23);
	QPushButton *m_pConfirmBtn = new QPushButton();
	m_pConfirmBtn->setText(QStringLiteral("确认"));
	m_pConfirmBtn->setFont(m_Font);
	QPushButton *m_pCancelBtn = new QPushButton();
	m_pCancelBtn->setText(QStringLiteral("取消"));
	m_pCancelBtn->setFont(m_Font);

	QHBoxLayout *m_pHBoxLayout1 = new QHBoxLayout();
	m_pHBoxLayout1->addWidget(m_pPassInfo);
	QHBoxLayout *m_pHBoxLayout2 = new QHBoxLayout();
	m_pHBoxLayout2->addWidget(m_pLabel1);
	m_pHBoxLayout2->addWidget(m_pPassEdit);
	QHBoxLayout *m_pHBoxLayout3 = new QHBoxLayout();
	m_pHBoxLayout3->addWidget(m_pConfirmBtn);
	m_pHBoxLayout3->addSpacing(30);
	m_pHBoxLayout3->addWidget(m_pCancelBtn);
	QVBoxLayout *m_pVBoxLayout = new QVBoxLayout();
	m_pVBoxLayout->addLayout(m_pHBoxLayout1);
	m_pVBoxLayout->addLayout(m_pHBoxLayout2);
	m_pVBoxLayout->addLayout(m_pHBoxLayout3);
	m_pVBoxLayout->setStretchFactor(m_pHBoxLayout1, 1);
	m_pVBoxLayout->setStretchFactor(m_pHBoxLayout2, 1);
	m_pVBoxLayout->setStretchFactor(m_pHBoxLayout3, 1);
	this->setLayout(m_pVBoxLayout);
	m_pPassEdit->setEchoMode(QLineEdit::Password);
	connect(m_pCancelBtn, SIGNAL(clicked()), this, SLOT(close()));
	connect(m_pConfirmBtn, SIGNAL(clicked()), this, SLOT(__confirmPassword()));


}

void PasswordForm::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
	{
		__confirmPassword();
	}
}

void PasswordForm::__confirmPassword()
{
	if (m_pPassEdit->text() == "YUNDA")
	{
		emit confirmPassword();
		this->close();
	}
	else
	{
		m_pPassEdit->setText("");
		m_pPassInfo->setText(QStringLiteral("密码错误，请重新输入！"));
		m_pPassInfo->setStyleSheet("font-size:25px;color:red;");
	}
}

