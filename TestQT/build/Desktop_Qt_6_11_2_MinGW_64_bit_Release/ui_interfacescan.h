/********************************************************************************
** Form generated from reading UI file 'interfacescan.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERFACESCAN_H
#define UI_INTERFACESCAN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_InterfaceScan
{
public:
    QVBoxLayout *layoutPrincipal;
    QGroupBox *groupConnexion;
    QHBoxLayout *layoutConnexion;
    QLabel *labelIP;
    QLineEdit *lineEditIP;
    QLabel *labelPort;
    QSpinBox *spinPort;
    QGroupBox *groupCommandes;
    QGridLayout *layoutCommandes;
    QLabel *labelAngle;
    QSpinBox *spinAngle;
    QPushButton *btnAngle;
    QPushButton *btnDist;
    QLabel *labelDebut;
    QSpinBox *spinDebut;
    QSpinBox *spinFin;
    QSpinBox *spinPas;
    QPushButton *btnScan;
    QTableWidget *tableMesures;
    QPlainTextEdit *journal;

    void setupUi(QDialog *InterfaceScan)
    {
        if (InterfaceScan->objectName().isEmpty())
            InterfaceScan->setObjectName("InterfaceScan");
        InterfaceScan->resize(560, 560);
        layoutPrincipal = new QVBoxLayout(InterfaceScan);
        layoutPrincipal->setObjectName("layoutPrincipal");
        groupConnexion = new QGroupBox(InterfaceScan);
        groupConnexion->setObjectName("groupConnexion");
        layoutConnexion = new QHBoxLayout(groupConnexion);
        layoutConnexion->setObjectName("layoutConnexion");
        labelIP = new QLabel(groupConnexion);
        labelIP->setObjectName("labelIP");

        layoutConnexion->addWidget(labelIP);

        lineEditIP = new QLineEdit(groupConnexion);
        lineEditIP->setObjectName("lineEditIP");

        layoutConnexion->addWidget(lineEditIP);

        labelPort = new QLabel(groupConnexion);
        labelPort->setObjectName("labelPort");

        layoutConnexion->addWidget(labelPort);

        spinPort = new QSpinBox(groupConnexion);
        spinPort->setObjectName("spinPort");
        spinPort->setMinimum(1);
        spinPort->setMaximum(65535);
        spinPort->setValue(8000);

        layoutConnexion->addWidget(spinPort);


        layoutPrincipal->addWidget(groupConnexion);

        groupCommandes = new QGroupBox(InterfaceScan);
        groupCommandes->setObjectName("groupCommandes");
        layoutCommandes = new QGridLayout(groupCommandes);
        layoutCommandes->setObjectName("layoutCommandes");
        labelAngle = new QLabel(groupCommandes);
        labelAngle->setObjectName("labelAngle");

        layoutCommandes->addWidget(labelAngle, 0, 0, 1, 1);

        spinAngle = new QSpinBox(groupCommandes);
        spinAngle->setObjectName("spinAngle");
        spinAngle->setMaximum(180);
        spinAngle->setValue(90);

        layoutCommandes->addWidget(spinAngle, 0, 1, 1, 1);

        btnAngle = new QPushButton(groupCommandes);
        btnAngle->setObjectName("btnAngle");

        layoutCommandes->addWidget(btnAngle, 0, 2, 1, 1);

        btnDist = new QPushButton(groupCommandes);
        btnDist->setObjectName("btnDist");

        layoutCommandes->addWidget(btnDist, 0, 3, 1, 1);

        labelDebut = new QLabel(groupCommandes);
        labelDebut->setObjectName("labelDebut");

        layoutCommandes->addWidget(labelDebut, 1, 0, 1, 1);

        spinDebut = new QSpinBox(groupCommandes);
        spinDebut->setObjectName("spinDebut");
        spinDebut->setMaximum(180);
        spinDebut->setValue(0);

        layoutCommandes->addWidget(spinDebut, 1, 1, 1, 1);

        spinFin = new QSpinBox(groupCommandes);
        spinFin->setObjectName("spinFin");
        spinFin->setMaximum(180);
        spinFin->setValue(180);

        layoutCommandes->addWidget(spinFin, 1, 2, 1, 1);

        spinPas = new QSpinBox(groupCommandes);
        spinPas->setObjectName("spinPas");
        spinPas->setMinimum(1);
        spinPas->setMaximum(180);
        spinPas->setValue(10);

        layoutCommandes->addWidget(spinPas, 1, 3, 1, 1);

        btnScan = new QPushButton(groupCommandes);
        btnScan->setObjectName("btnScan");

        layoutCommandes->addWidget(btnScan, 2, 0, 1, 4);


        layoutPrincipal->addWidget(groupCommandes);

        tableMesures = new QTableWidget(InterfaceScan);
        if (tableMesures->columnCount() < 2)
            tableMesures->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableMesures->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableMesures->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableMesures->setObjectName("tableMesures");
        tableMesures->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableMesures->setColumnCount(2);

        layoutPrincipal->addWidget(tableMesures);

        journal = new QPlainTextEdit(InterfaceScan);
        journal->setObjectName("journal");
        journal->setMaximumSize(QSize(16777215, 110));
        journal->setReadOnly(true);

        layoutPrincipal->addWidget(journal);


        retranslateUi(InterfaceScan);

        QMetaObject::connectSlotsByName(InterfaceScan);
    } // setupUi

    void retranslateUi(QDialog *InterfaceScan)
    {
        InterfaceScan->setWindowTitle(QCoreApplication::translate("InterfaceScan", "Radar \303\240 ultrasons", nullptr));
        groupConnexion->setTitle(QCoreApplication::translate("InterfaceScan", "Routeur (c\303\264t\303\251 WAN)", nullptr));
        labelIP->setText(QCoreApplication::translate("InterfaceScan", "Adresse IP :", nullptr));
        lineEditIP->setPlaceholderText(QCoreApplication::translate("InterfaceScan", "ex. 192.168.184.170", nullptr));
        labelPort->setText(QCoreApplication::translate("InterfaceScan", "Port :", nullptr));
        groupCommandes->setTitle(QCoreApplication::translate("InterfaceScan", "Commandes", nullptr));
        labelAngle->setText(QCoreApplication::translate("InterfaceScan", "Angle (\302\260) :", nullptr));
        btnAngle->setText(QCoreApplication::translate("InterfaceScan", "Orienter (ANGLE)", nullptr));
        btnDist->setText(QCoreApplication::translate("InterfaceScan", "Mesurer (DIST)", nullptr));
        labelDebut->setText(QCoreApplication::translate("InterfaceScan", "D\303\251but / fin / pas :", nullptr));
        btnScan->setText(QCoreApplication::translate("InterfaceScan", "Balayer (SCAN)", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableMesures->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("InterfaceScan", "Angle (\302\260)", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableMesures->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("InterfaceScan", "Distance (cm)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InterfaceScan: public Ui_InterfaceScan {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERFACESCAN_H
