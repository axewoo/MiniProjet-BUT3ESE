#ifndef INTERFACESCAN_H
#define INTERFACESCAN_H

#include <QDialog>
#include <QThread>

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

private slots:
    void cliqueAngle();
    void cliqueDist();
    void cliqueScan();

private:
    void envoyer(const QString &cmd, int timeout_ms, int essais);
    void reponseRecue(const QString &cmd, int code, const QString &rep);
    void afficherScan(const QString &rep);
    void setOccupe(bool occupe);
    void log(const QString &texte);

    Ui::InterfaceScan *ui;
    QThread *thread_reseau = nullptr;   // requete UDP en cours (une seule a la fois)
};
#endif // INTERFACESCAN_H
