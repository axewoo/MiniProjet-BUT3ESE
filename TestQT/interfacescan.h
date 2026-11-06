#ifndef INTERFACESCAN_H
#define INTERFACESCAN_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class InterfaceScan;
}
QT_END_NAMESPACE

class InterfaceScan : public QDialog
{
    Q_OBJECT

public:
    explicit InterfaceScan(QWidget *parent = nullptr);
    ~InterfaceScan() override;

private:
    Ui::InterfaceScan *ui;
};
#endif // INTERFACESCAN_H
