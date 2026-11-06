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

QT_BEGIN_NAMESPACE

class Ui_InterfaceScan
{
public:

    void setupUi(QDialog *InterfaceScan)
    {
        if (InterfaceScan->objectName().isEmpty())
            InterfaceScan->setObjectName("InterfaceScan");
        InterfaceScan->resize(800, 600);

        retranslateUi(InterfaceScan);

        QMetaObject::connectSlotsByName(InterfaceScan);
    } // setupUi

    void retranslateUi(QDialog *InterfaceScan)
    {
        InterfaceScan->setWindowTitle(QCoreApplication::translate("InterfaceScan", "InterfaceScan", nullptr));
    } // retranslateUi

};

namespace Ui {
    class InterfaceScan: public Ui_InterfaceScan {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERFACESCAN_H
