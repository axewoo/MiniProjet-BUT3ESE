#include "interfacescan.h"
#include "ui_interfacescan.h"
#include "client_udp.h"

#include <QTime>
#include <QHeaderView>

// Delais d'attente : SCAN dure ~2,5 s cote carte (0 a 180 par pas de 10)
#define TIMEOUT_CMD_MS   1000
#define ESSAIS_CMD       3
#define TIMEOUT_SCAN_MS  5000
#define ESSAIS_SCAN      2
#define MAX_MESURES      64

InterfaceScan::InterfaceScan(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InterfaceScan)
{
    ui->setupUi(this);
    ui->tableMesures->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->btnAngle, &QPushButton::clicked, this, &InterfaceScan::cliqueAngle);
    connect(ui->btnDist,  &QPushButton::clicked, this, &InterfaceScan::cliqueDist);
    connect(ui->btnScan,  &QPushButton::clicked, this, &InterfaceScan::cliqueScan);

    if (udp_init() < 0) log("Erreur : initialisation du reseau impossible");
}

InterfaceScan::~InterfaceScan()
{
    // fermeture propre : on attend la fin de la requete en cours,
    // qui ferme son socket (au plus TIMEOUT x ESSAIS)
    if (thread_reseau) {
        thread_reseau->wait();
        delete thread_reseau;
    }
    udp_quitter();
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

// Lance la requete dans un thread : udp_requete() bloque pendant l'attente,
// la fenetre doit rester reactive. Le resultat revient dans reponseRecue(),
// executee dans le thread de l'interface.
void InterfaceScan::envoyer(const QString &cmd, int timeout_ms, int essais)
{
    QString texte_ip = ui->lineEditIP->text().trimmed();
    if (texte_ip.isEmpty()) {
        log("Erreur : saisir l'adresse IP du routeur");
        return;
    }
    QByteArray ip  = texte_ip.toLatin1();
    QByteArray c   = cmd.toLatin1();
    unsigned short port = (unsigned short)ui->spinPort->value();

    setOccupe(true);
    log("> " + cmd);

    thread_reseau = QThread::create([this, ip, c, port, cmd, timeout_ms, essais]() {
        char rep[1500];             // un datagramme tient dans 1500 octets
        int n = udp_requete(ip.constData(), port, c.constData(),
                            rep, sizeof(rep), timeout_ms, essais);
        QString texte = (n >= 0) ? QString::fromLatin1(rep) : QString();
        // retour dans le thread de l'interface (on ne touche pas aux widgets ici)
        QMetaObject::invokeMethod(this, [this, cmd, n, texte]() {
            reponseRecue(cmd, n, texte);
        }, Qt::QueuedConnection);
    });
    thread_reseau->start();
}

void InterfaceScan::reponseRecue(const QString &cmd, int code, const QString &rep)
{
    thread_reseau->wait();          // le thread a fini (socket ferme)
    delete thread_reseau;
    thread_reseau = nullptr;
    setOccupe(false);

    if (code < 0) {
        log(QString("Erreur : ") + udp_erreur_texte(code));
        return;
    }
    if (cmd.startsWith("SCAN") && !rep.startsWith("ERR")) {
        afficherScan(rep);
    } else {
        log("< " + rep.trimmed());  // OK, DIST 23.4, ERR ...
    }
}

void InterfaceScan::afficherScan(const QString &rep)
{
    Mesure tab[MAX_MESURES];
    QByteArray r = rep.toLatin1();
    int n = udp_decoder_scan(r.constData(), tab, MAX_MESURES);

    if (n < 0) {
        log(QString("Erreur : ") + udp_erreur_texte(n));
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
