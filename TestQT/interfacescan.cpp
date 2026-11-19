#include "interfacescan.h"
#include "ui_interfacescan.h"
#include "protocole.h"            // udp_decoder_scan() : decoupage de SCAN, en C

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>
#include <QTime>
#include <QHeaderView>
#include <QRegularExpression>

// Delais d'attente : SCAN dure ~2,5 s cote carte (0 a 180 par pas de 10)
#define TIMEOUT_CMD_MS   1000     // ANGLE et DIST : 1 s ...
#define ESSAIS_CMD       3        // ... et 3 envois au maximum
#define TIMEOUT_SCAN_MS  5000     // SCAN : 5 s ...
#define ESSAIS_SCAN      2        // ... et 2 envois au maximum
#define MAX_MESURES      64

/*------------------------------------------------------------------------------
 * Principe (QUdpSocket, sans thread)
 *
 * Avec Winsock, recvfrom() bloquait : il fallait un thread pour que la
 * fenetre ne se fige pas. QUdpSocket fonctionne autrement, par SIGNAUX :
 *   1) envoyer()      : on cree un QUdpSocket, on envoie la commande avec
 *                       writeDatagram() et on lance un QTimer (delai d'attente).
 *                       La fonction se termine tout de suite : la fenetre
 *                       reste reactive.
 *   2) lireReponse()  : appelee par Qt (signal readyRead) quand un datagramme
 *                       arrive. On arrete le minuteur et on affiche la reponse.
 *   3) delaiDepasse() : appelee par Qt (signal timeout du QTimer) si rien
 *                       n'est arrive a temps. UDP peut perdre un datagramme :
 *                       on renvoie la commande, ou on abandonne apres le
 *                       dernier essai.
 * Un socket par requete : une reponse tres en retard d'une ancienne requete
 * arrive sur un socket deja ferme et ne peut pas etre prise pour la reponse
 * a la commande suivante.
 *----------------------------------------------------------------------------*/

InterfaceScan::InterfaceScan(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InterfaceScan)
{
    ui->setupUi(this);
    ui->tableMesures->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->btnAngle, &QPushButton::clicked, this, &InterfaceScan::cliqueAngle);
    connect(ui->btnDist,  &QPushButton::clicked, this, &InterfaceScan::cliqueDist);
    connect(ui->btnScan,  &QPushButton::clicked, this, &InterfaceScan::cliqueScan);

    // Minuteur du delai d'attente : un seul declenchement par envoi
    minuteur = new QTimer(this);
    minuteur->setSingleShot(true);
    connect(minuteur, &QTimer::timeout, this, &InterfaceScan::delaiDepasse);
}

InterfaceScan::~InterfaceScan()
{
    // Fermeture propre : le socket en cours (s'il y en a un) est ferme.
    // (il serait aussi detruit avec la fenetre, dont il est l'enfant)
    terminerRequete();
    delete ui;
}

void InterfaceScan::cliqueAngle()
{
    envoyer(QString("ANGLE %1").arg(ui->spinAngle->value()), TIMEOUT_CMD_MS, ESSAIS_CMD);
}

void InterfaceScan::cliqueDist()
{
    envoyer("DIST", TIMEOUT_CMD_MS, ESSAIS_CMD);
}

void InterfaceScan::cliqueScan()
{
    envoyer(QString("SCAN %1 %2 %3").arg(ui->spinDebut->value())
                                    .arg(ui->spinFin->value())
                                    .arg(ui->spinPas->value()),
            TIMEOUT_SCAN_MS, ESSAIS_SCAN);
}

// 1) Prepare et lance une requete UDP
void InterfaceScan::envoyer(const QString &cmd, int timeout_ms, int essais)
{
    // Adresse saisie dans l'interface (rien en dur). On exige 4 nombres
    // separes par des points (setAddress() seul accepterait "1.2.3"),
    // puis setAddress() verifie que chaque nombre vaut au plus 255.
    QString texte_ip = ui->lineEditIP->text().trimmed();
    static const QRegularExpression format_ip("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
    if (!format_ip.match(texte_ip).hasMatch() || !adresse_carte.setAddress(texte_ip)) {
        log("Erreur : adresse IP invalide (ex. 192.168.184.170)");
        return;
    }
    port_carte = (quint16)ui->spinPort->value();

    // Le protocole demande une commande ASCII terminee par \n
    commande = cmd.toLatin1() + "\n";
    essais_restants = essais;
    minuteur->setInterval(timeout_ms);

    // Nouveau socket pour cette requete. bind() sur AnyIPv4, port 0 : le
    // systeme choisit notre port source, c'est la que la carte repondra
    // (elle repond a l'adresse et au port de l'expediteur).
    socket = new QUdpSocket(this);
    if (!socket->bind(QHostAddress::AnyIPv4, 0)) {
        log("Erreur : impossible d'ouvrir le socket UDP (" + socket->errorString() + ")");
        terminerRequete();
        return;
    }
    // readyRead est emis par Qt chaque fois qu'un datagramme arrive
    connect(socket, &QUdpSocket::readyRead, this, &InterfaceScan::lireReponse);
    connect(socket, &QUdpSocket::errorOccurred, this, &InterfaceScan::erreurSocket);

    setOccupe(true);
    log("> " + cmd);
    envoyerDatagramme();
}

// Envoie la commande (premier envoi ou nouvel essai) et relance le minuteur
void InterfaceScan::envoyerDatagramme()
{
    essais_restants--;
    // writeDatagram() = sendto() de Winsock : un datagramme vers adresse:port
    if (socket->writeDatagram(commande, adresse_carte, port_carte) != commande.size()) {
        log("Erreur : envoi impossible (" + socket->errorString() + ")");
        terminerRequete();
        return;
    }
    minuteur->start();
}

// 2) Un ou plusieurs datagrammes sont arrives sur le socket
void InterfaceScan::lireReponse()
{
    while (socket && socket->hasPendingDatagrams()) {
        // receiveDatagram() = recvfrom() : donnees + adresse et port de l'expediteur
        QNetworkDatagram datagramme = socket->receiveDatagram();

        // On ignore un datagramme qui ne vient pas de la carte (adresse et port vises)
        if (!datagramme.senderAddress().isEqual(adresse_carte, QHostAddress::ConvertV4MappedToIPv4)
            || datagramme.senderPort() != port_carte) {
            continue;
        }

        // Bonne reponse : on arrete d'attendre, on ferme le socket, on affiche
        QString reponse = QString::fromLatin1(datagramme.data());
        terminerRequete();
        traiterReponse(reponse);
        return;
    }
}

// 3) Pas de reponse dans le delai : la commande ou la reponse a ete perdue
void InterfaceScan::delaiDepasse()
{
    if (essais_restants > 0) {
        log("  pas de reponse, nouvel essai...");
        envoyerDatagramme();
    } else {
        terminerRequete();
        log("Erreur : pas de reponse de la carte (delai depasse)");
    }
}

// Erreur signalee par le socket. Sous Windows, si personne n'ecoute sur le
// port vise, un message ICMP "port inaccessible" revient et Qt le signale
// ici (ConnectionRefusedError). On l'affiche, et on laisse le minuteur
// decider d'un nouvel essai.
void InterfaceScan::erreurSocket()
{
    if (socket) log("  socket : " + socket->errorString());
}

// Fin de requete (reponse recue, abandon ou fermeture de la fenetre) :
// minuteur arrete, socket ferme et libere, boutons de nouveau actifs
void InterfaceScan::terminerRequete()
{
    minuteur->stop();
    if (socket) {
        socket->close();         // = closesocket()
        socket->deleteLater();   // detruit par Qt une fois le signal en cours fini
        socket = nullptr;
    }
    setOccupe(false);
}

void InterfaceScan::traiterReponse(const QString &rep)
{
    if (commande.startsWith("SCAN") && !rep.startsWith("ERR")) {
        afficherScan(rep);
    } else {
        log("< " + rep.trimmed());  // OK, DIST 23.4, ERR ...
    }
}

void InterfaceScan::afficherScan(const QString &rep)
{
    Mesure tab[MAX_MESURES];
    QByteArray r = rep.toLatin1();
    int n = udp_decoder_scan(r.constData(), tab, MAX_MESURES);   // fonction C

    if (n < 0) {
        log("Erreur : reponse SCAN mal formee");
        return;
    }
    ui->tableMesures->setRowCount(n);
    for (int i = 0; i < n; i++) {
        QString dist = (tab[i].dixiemes < 0)
                     ? "TIMEOUT"
                     : QString("%1.%2").arg(tab[i].dixiemes / 10).arg(tab[i].dixiemes % 10);
        ui->tableMesures->setItem(i, 0, new QTableWidgetItem(QString::number(tab[i].angle)));
        ui->tableMesures->setItem(i, 1, new QTableWidgetItem(dist));
    }
    log(QString("< %1 mesures recues").arg(n));
}

// Pendant une requete : boutons grises (une seule requete a la fois)
void InterfaceScan::setOccupe(bool occupe)
{
    ui->btnAngle->setEnabled(!occupe);
    ui->btnDist->setEnabled(!occupe);
    ui->btnScan->setEnabled(!occupe);
    setCursor(occupe ? Qt::BusyCursor : Qt::ArrowCursor);
}

void InterfaceScan::log(const QString &texte)
{
    ui->journal->appendPlainText(QTime::currentTime().toString("HH:mm:ss  ") + texte);
}
