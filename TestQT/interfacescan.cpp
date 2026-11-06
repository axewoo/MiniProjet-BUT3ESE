#include "interfacescan.h"
#include "ui_interfacescan.h"

InterfaceScan::InterfaceScan(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InterfaceScan)
{
    ui->setupUi(this);
}

InterfaceScan::~InterfaceScan()
{
    delete ui;
}
