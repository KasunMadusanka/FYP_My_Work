/********************************************************************************
** Form generated from reading UI file 'qttesting.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTTESTING_H
#define UI_QTTESTING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtTestingClass
{
public:
    QWidget *centralWidget;
    QLabel *lblCamera1;
    QLabel *lblCamera2;
    QLabel *lblCamera3;
    QLabel *lblCamera4;
    QLabel *lblCamera5;
    QLabel *lblCamera6;
    QLabel *lblCamera7;
    QLabel *lblCamera8;
    QLabel *label;
    QLabel *lblGlobalFrameCounter;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtTestingClass)
    {
        if (QtTestingClass->objectName().isEmpty())
            QtTestingClass->setObjectName(QStringLiteral("QtTestingClass"));
        QtTestingClass->resize(1360, 600);
        centralWidget = new QWidget(QtTestingClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        lblCamera1 = new QLabel(centralWidget);
        lblCamera1->setObjectName(QStringLiteral("lblCamera1"));
        lblCamera1->setGeometry(QRect(0, 35, 340, 255));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblCamera1->sizePolicy().hasHeightForWidth());
        lblCamera1->setSizePolicy(sizePolicy);
        lblCamera1->setAutoFillBackground(true);
        lblCamera1->setFrameShape(QFrame::Box);
        lblCamera2 = new QLabel(centralWidget);
        lblCamera2->setObjectName(QStringLiteral("lblCamera2"));
        lblCamera2->setGeometry(QRect(340, 35, 340, 255));
        sizePolicy.setHeightForWidth(lblCamera2->sizePolicy().hasHeightForWidth());
        lblCamera2->setSizePolicy(sizePolicy);
        lblCamera2->setAutoFillBackground(true);
        lblCamera2->setFrameShape(QFrame::Box);
        lblCamera3 = new QLabel(centralWidget);
        lblCamera3->setObjectName(QStringLiteral("lblCamera3"));
        lblCamera3->setGeometry(QRect(680, 35, 340, 255));
        sizePolicy.setHeightForWidth(lblCamera3->sizePolicy().hasHeightForWidth());
        lblCamera3->setSizePolicy(sizePolicy);
        lblCamera3->setAutoFillBackground(true);
        lblCamera3->setFrameShape(QFrame::Box);
        lblCamera4 = new QLabel(centralWidget);
        lblCamera4->setObjectName(QStringLiteral("lblCamera4"));
        lblCamera4->setGeometry(QRect(1020, 35, 340, 255));
        sizePolicy.setHeightForWidth(lblCamera4->sizePolicy().hasHeightForWidth());
        lblCamera4->setSizePolicy(sizePolicy);
        lblCamera4->setAutoFillBackground(true);
        lblCamera4->setFrameShape(QFrame::Box);
        lblCamera5 = new QLabel(centralWidget);
        lblCamera5->setObjectName(QStringLiteral("lblCamera5"));
        lblCamera5->setGeometry(QRect(0, 290, 340, 255));
        sizePolicy.setHeightForWidth(lblCamera5->sizePolicy().hasHeightForWidth());
        lblCamera5->setSizePolicy(sizePolicy);
        lblCamera5->setAutoFillBackground(true);
        lblCamera5->setFrameShape(QFrame::Box);
        lblCamera6 = new QLabel(centralWidget);
        lblCamera6->setObjectName(QStringLiteral("lblCamera6"));
        lblCamera6->setGeometry(QRect(340, 290, 340, 255));
        sizePolicy.setHeightForWidth(lblCamera6->sizePolicy().hasHeightForWidth());
        lblCamera6->setSizePolicy(sizePolicy);
        lblCamera6->setAutoFillBackground(true);
        lblCamera6->setFrameShape(QFrame::Box);
        lblCamera7 = new QLabel(centralWidget);
        lblCamera7->setObjectName(QStringLiteral("lblCamera7"));
        lblCamera7->setGeometry(QRect(680, 290, 340, 255));
        sizePolicy.setHeightForWidth(lblCamera7->sizePolicy().hasHeightForWidth());
        lblCamera7->setSizePolicy(sizePolicy);
        lblCamera7->setAutoFillBackground(true);
        lblCamera7->setFrameShape(QFrame::Box);
        lblCamera8 = new QLabel(centralWidget);
        lblCamera8->setObjectName(QStringLiteral("lblCamera8"));
        lblCamera8->setGeometry(QRect(1020, 290, 340, 255));
        sizePolicy.setHeightForWidth(lblCamera8->sizePolicy().hasHeightForWidth());
        lblCamera8->setSizePolicy(sizePolicy);
        lblCamera8->setAutoFillBackground(true);
        lblCamera8->setFrameShape(QFrame::Box);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(6, -1, 671, 31));
        QFont font;
        font.setFamily(QStringLiteral("MS Shell Dlg 2"));
        font.setPointSize(11);
        font.setBold(false);
        font.setWeight(50);
        label->setFont(font);
        lblGlobalFrameCounter = new QLabel(centralWidget);
        lblGlobalFrameCounter->setObjectName(QStringLiteral("lblGlobalFrameCounter"));
        lblGlobalFrameCounter->setGeometry(QRect(1150, 10, 201, 21));
        lblGlobalFrameCounter->setAutoFillBackground(true);
        QtTestingClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtTestingClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1360, 21));
        QtTestingClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtTestingClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtTestingClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtTestingClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtTestingClass->setStatusBar(statusBar);

        retranslateUi(QtTestingClass);

        QMetaObject::connectSlotsByName(QtTestingClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtTestingClass)
    {
        QtTestingClass->setWindowTitle(QApplication::translate("QtTestingClass", "QtTesting", 0));
        lblCamera1->setText(QString());
        lblCamera2->setText(QString());
        lblCamera3->setText(QString());
        lblCamera4->setText(QString());
        lblCamera5->setText(QString());
        lblCamera6->setText(QString());
        lblCamera7->setText(QString());
        lblCamera8->setText(QString());
        label->setText(QApplication::translate("QtTestingClass", "Nephelium Inc. - A GPU based implementation for human reidentification in Camera Networks", 0));
        lblGlobalFrameCounter->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class QtTestingClass: public Ui_QtTestingClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTTESTING_H
