#ifndef INTERFACESCAN_H
#define INTERFACESCAN_H

#include <QDialog>
#include <QHostAddress>

class QUdpSocket;
class QTimer;

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
    void lireReponse();       // QUdpSocket::readyRead : un datagramme est arrive
    void delaiDepasse();      // QTimer::timeout : pas de reponse a temps
    void erreurSocket();      // QUdpSocket::errorOccurred

private:
    void envoyer(const QString &cmd, int timeout_ms, int essais);
    void envoyerDatagramme();
    void terminerRequete();
    void traiterReponse(const QString &rep);
    void afficherScan(const QString &rep);
    void setOccupe(bool occupe);
    void log(const QString &texte);

    Ui::InterfaceScan *ui;

    // Requete UDP en cours (une seule a la fois)
    QUdpSocket  *socket = nullptr;  // socket de la requete en cours (nullptr si aucune)
    QTimer      *minuteur;          // delai d'attente de la reponse
    QHostAddress adresse_carte;     // IP WAN du routeur (ou IP de la carte en LAN)
    quint16      port_carte = 0;    // port externe du routeur (ou 6969 en LAN)
    QByteArray   commande;          // commande envoyee, avec son \n
    int          essais_restants = 0;
};
#endif // INTERFACESCAN_H
