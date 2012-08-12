#include <QtCore/QStringList>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "infowidget.h"
#include "optionswidget.h"
#include "rec.h"
#include <QMessageBox>
#include "plot.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    infoWidget(0), optionsWidget(0),
    enumerator(0),
    serial(0),
    recWidget0(0),
    recWidget1(0),
    dataArray(0)
{
    ui->setupUi(this);
    createActions();
    createMenus();
    createToolBars();
    createCentralWidget();



    this->initEnumerator();
    this->initSerial();
    this->initButtonConnections();
    this->initBoxConnections();
    this->initMainWidgetCloseState();

}

MainWindow::~MainWindow()
{
    this->deinitEnumerator();
    this->deinitSerial();
    this->deinitWidgets();
    delete ui;
}

void MainWindow::createActions()
{
     aboutAct = new QAction(tr("About"), this);
     aboutAct->setStatusTip(tr("Show the About box"));
     connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));


     aboutqtAct = new QAction(tr("AboutQT"), this);
     aboutqtAct->setStatusTip(tr("Show the Qt library's About box"));
     connect(aboutqtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

     controlButton = new QAction(tr("Open"), this);
     controlButton->setStatusTip(tr("Open port"));
     //connect(Open, SIGNAL(triggered()), this, SLOT(about()));

     optionsButton = new QAction(tr("Options"), this);
     optionsButton->setStatusTip(tr("Options of connection"));
     //connect(Open, SIGNAL(triggered()), this, SLOT(about()));

     infoButton = new QAction(tr("Info"), this);
     infoButton->setStatusTip(tr("Info"));
     //connect(Open, SIGNAL(triggered()), this, SLOT(about()));

    // portBox->setStatusTip(tr("Available ports"));

 }

void MainWindow::createMenus()
 {
     helpMenu = menuBar()->addMenu(tr("&Help"));
     helpMenu->addAction(aboutqtAct);
     helpMenu->addAction(aboutAct);
 }

void MainWindow::createCentralWidget()
 {
    textEdit = new QPlainTextEdit;
    textEdit->setSizePolicy(QSizePolicy::Expanding,
          QSizePolicy::Fixed);
    plot0 = new Plot(this);
    QString f0 = "tmpfile0.dat";
    plot0->SetTmpfile(f0);

    plot1 = new Plot(this);
    QString f1 = "tmpfile1.dat";
    plot1->SetTmpfile(f1);


    rec0 = new QPushButton(this);
    rec0 ->setText("Rec");
    rec1 = new QPushButton(this);
    rec1 ->setText("Rec");

    layout = new QGridLayout();
    layout->addWidget(plot0, 0, 0);
    layout->addWidget(plot1, 0, 1);
    layout->addWidget(rec0, 1, 0);
    layout->addWidget(rec1, 1, 1);
    layout->addWidget(textEdit, 2, 0, 2, 2);
    centralWidget()->setLayout(layout);
    setWindowTitle(tr("Reno_term"));

 }

void MainWindow::createToolBars()
 {
     tb = new QToolBar(this);
     addToolBar(tb);
     portBox = new QComboBox(tb);
     portBox->setObjectName("Ports");
     tb->addWidget(portBox);
     tb->addAction(controlButton);
     tb->addAction(optionsButton);
     tb->addAction(infoButton);
 }

void MainWindow::about()
 {
    QMessageBox::about(this, tr("About Application"),
             tr("<b>Reno_term</b> - software for communication with Reno.<p> Created by Alex Alexeev. 2011"));
 }

void MainWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/* Private slots section */

void MainWindow::procEnumerate(const QStringList &l)
{
    // Fill ports box.
    portBox->clear();
    portBox->addItems(l);
}

void MainWindow::procSerialMessages(const QString &msg, QDateTime dt)
{
    QString s = dt.time().toString() + " > " + msg;
    textEdit->appendPlainText(s);
}

void MainWindow::procSerialDataReceive()
{
    if (this->serial && this->serial->isOpen()) {
        QByteArray byte = this->serial->readAll();
        this->printTrace(byte);
        qDebug() << "Rx: " << byte;
        if(byte.at(0)!='\n'){
            dataArray.append(byte);
            qDebug() << "Data: " << dataArray;
        }
        else {

            if(dataArray.at(0)=='C'){
                if(dataArray.at(3) == '0') {
                    QByteArray u;    
                    int j =5;
                    while(dataArray.at(j)!='\r')
                    {
                        u[j-5]= dataArray.at(j);
                        j++;
                    }
                    plot0 ->appendPoint0(u.toDouble());
                    qDebug() << "Data5: " << u;
                }

                if(dataArray.at(3) == '1') {
                    QByteArray u;
                    int d =5;
                    while(dataArray.at(d)!='\r')
                    {
                        u[d-5]= dataArray.at(d);
                        d++;
                    }
                    plot0 ->appendPoint1(u.toDouble());
                    qDebug() << "Data5: " << u;
                }

                if(dataArray.at(3) == '2') {
                    QByteArray u;
                    int l =5;
                    while(dataArray.at(l)!='\r')
                    {
                        u[l-5]= dataArray.at(l);
                        l++;
                    }
                    plot1 ->appendPoint0(u.toDouble());
                    qDebug() << "Data5: " << u;
                }

                if(dataArray.at(3) == '3') {
                    QByteArray u;

                    int k =5;
                    while(dataArray.at(k)!='\r')
                    {
                        u[k-5]= dataArray.at(k);
                        k++;
                    }
                    plot1 ->appendPoint1(u.toDouble());
                    qDebug() << "Data5: " << u;
                }
            }

            dataArray = 0;
            qDebug() << "Data: " << dataArray;
        }

    }
}

void MainWindow::printTrace(const QByteArray &data)
{
    //textEdit->setTextColor((directionRx) ? Qt::darkBlue : Qt::darkGreen);
    textEdit->insertPlainText(QString(data));

    QScrollBar *bar = textEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void MainWindow::procApplyOptions(const QStringList &list)
{
    if (this->serial && this->serial->isOpen()) {
        QStringList notApplyList;
        bool result = true;
        if ((this->serial->baudRate() != list.at(0)) &&
            (!this->serial->setBaudRate(list.at(0)))) {
            notApplyList << list.at(0);
            result = false;
        }

        if ((this->serial->dataBits() != list.at(1)) &&
            (!this->serial->setDataBits(list.at(1)))) {
            notApplyList << list.at(1);
            result = false;
        }

        if ((this->serial->parity() != list.at(2)) &&
            (!this->serial->setParity(list.at(2)))) {
            notApplyList << list.at(2);
            result = false;
        }

        if ((this->serial->stopBits() != list.at(3)) &&
            (!this->serial->setStopBits(list.at(3)))) {
            notApplyList << list.at(3);
            result = false;
        }

        if ((this->serial->flowControl() != list.at(4)) &&
            (!this->serial->setFlowControl(list.at(4)))) {
            notApplyList << list.at(4);
            result = false;
        }

        if ((!result) && this->initInfoWidget())
            this->updateOptionsData();

        emit this->optionsApplied(result, notApplyList);
    }
}

void MainWindow::procControlButtonClick()
{
    if (this->serial) {
        bool result = this->serial->isOpen();
        if (result) {
            this->serial->close();
            result = false;
        }
        else {
            this->serial->setDeviceName(portBox->currentText());
            result = this->serial->open(QIODevice::ReadWrite);

            plot0->start();
            plot1->start();

        }

        (result) ? this->initMainWidgetOpenState() : this->initMainWidgetCloseState();
    }
}

void MainWindow::procOptionsButtonClick()
{
    if (this->initOptionsWidget()) {
        this->updateOptionsData();
        this->optionsWidget->show();
    }
}

void MainWindow::procInfoButtonClick()
{
    if (this->initInfoWidget()) {
        this->updateInfoData(portBox->currentText());
        this->infoWidget->show();
    }
}

void MainWindow::procrec0ButtonClick()
{
    if (this->initRecWidget0()) {
        this->recWidget0->show();
    }
}

void MainWindow::procrec1ButtonClick()
{
    if (this->initRecWidget1()) {
        this->recWidget1->show();
    }
}

void MainWindow::procBoxChange(const QString &item)
{
    if (this->initInfoWidget())
        this->updateInfoData(item);
}

/* Private methods section */

void MainWindow::initMainWidgetCloseState()
{
    portBox->setEnabled(true);
    optionsButton->setEnabled(false);
    controlButton->setText(QString(tr("Open")));
    if (this->optionsWidget && this->optionsWidget->isVisible())
        this->optionsWidget->hide();
}

void MainWindow::initMainWidgetOpenState()
{
    portBox->setEnabled(false);
    optionsButton->setEnabled(true);
    controlButton->setText(QString(tr("Close")));
}

bool MainWindow::initRecWidget0()
{
    if (!this->recWidget0) {
        this->recWidget0 = new rec();
         recWidget0->SetrecYes();

        connect(this->recWidget0, SIGNAL(applyRectmpfile(QString)),
                this->plot0, SLOT(SetTmpfile(QString)));

        if (!this->recWidget0)
            return false;
    }
    return true;
}

bool MainWindow::initRecWidget1()
{
    if (!this->recWidget1) {
        this->recWidget1 = new rec();
         recWidget1->SetrecYes();

        connect(this->recWidget1, SIGNAL(applyRectmpfile(QString)),
                this->plot1, SLOT(SetTmpfile(QString)));

        if (!this->recWidget1)
            return false;
    }
    return true;
}

bool MainWindow::initInfoWidget()
{
    if (!this->infoWidget) {
        this->infoWidget = new InfoWidget();
        if (!this->infoWidget)
            return false;
    }
    return true;
}

bool MainWindow::initOptionsWidget()
{
    if (!this->optionsWidget && this->serial) {
        this->optionsWidget = new OptionsWidget(this->serial->listBaudRate(),
                                                this->serial->listDataBits(),
                                                this->serial->listParity(),
                                                this->serial->listStopBits(),
                                                this->serial->listFlowControl());
        if (!this->optionsWidget)
            return false;

        connect(this, SIGNAL(optionsApplied(bool,QStringList)),
                this->optionsWidget, SLOT(procAppliedOptions(bool,QStringList)));
        connect(this->optionsWidget, SIGNAL(applyOptions(QStringList)),
                this, SLOT(procApplyOptions(QStringList)));
    }
    return true;
}

void MainWindow::initEnumerator()
{
    if (this->enumerator)
        return;
    this->enumerator = new SerialDeviceEnumerator(this);
    connect(this->enumerator, SIGNAL(hasChanged(QStringList)), this, SLOT(procEnumerate(QStringList)));
    this->enumerator->setEnabled(true);
}

void MainWindow::deinitEnumerator()
{
    if (this->enumerator && this->enumerator->isEnabled())
        this->enumerator->setEnabled(false);
}

void MainWindow::initSerial()
{
    if (this->serial)
        return;
    this->serial = new AbstractSerial(this);
    connect(this->serial, SIGNAL(signalStatus(QString,QDateTime)), this, SLOT(procSerialMessages(QString,QDateTime)));
    connect(this->serial, SIGNAL(readyRead()), this, SLOT(procSerialDataReceive()));

    // Enable emmiting signal signalStatus
    this->serial->enableEmitStatus(true);
}

void MainWindow::deinitSerial()
{
    if (this->serial && this->serial->isOpen())
        this->serial->close();
}

void MainWindow::initButtonConnections()
{
    connect(controlButton, SIGNAL(triggered()), this, SLOT(procControlButtonClick()));
    connect(infoButton, SIGNAL(triggered()), this, SLOT(procInfoButtonClick()));
    connect(optionsButton, SIGNAL(triggered()), this, SLOT(procOptionsButtonClick()));
    connect(rec0, SIGNAL(clicked()), this, SLOT(procrec0ButtonClick()));
    connect(rec1, SIGNAL(clicked()), this, SLOT(procrec1ButtonClick()));
}

void MainWindow::initBoxConnections()
{
    connect(portBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(procBoxChange(QString)));
}

void MainWindow::deinitWidgets()
{
    if (this->infoWidget)
        delete (this->infoWidget);
    if (this->optionsWidget)
        delete (this->optionsWidget);
    if (this->recWidget0)
        delete (this->recWidget0);
    if (this->recWidget1)
        delete (this->recWidget1);
}

//Update widgets
void MainWindow::updateInfoData(const QString &name)
{
    if (this->enumerator && this->infoWidget) {
        InfoWidget::InfoData data;

        this->enumerator->setDeviceName(name);
        data.name = name;
        data.bus = this->enumerator->bus();
        data.busy = this->enumerator->isBusy();
        data.description = this->enumerator->description();
        data.driver = this->enumerator->driver();
        data.exists = this->enumerator->isExists();
        data.friendlyName = this->enumerator->friendlyName();
        data.locationInfo = this->enumerator->locationInfo();
        data.manufacturer = this->enumerator->manufacturer();
        data.productID = this->enumerator->productID();
        data.revision = this->enumerator->revision();
        data.service = this->enumerator->service();
        data.shortName = this->enumerator->shortName();
        data.subSystem = this->enumerator->subSystem();
        data.systemPath = this->enumerator->systemPath();
        data.vendorID = this->enumerator->vendorID();

        this->infoWidget->updateInfo(data);
    }
}

void MainWindow::updateOptionsData()
{
    if (this->initOptionsWidget() && this->serial && this->serial->isOpen()) {
        OptionsWidget::OptionsData data;

        data.baud = this->serial->baudRate();
        data.data = this->serial->dataBits();
        data.parity = this->serial->parity();
        data.stop = this->serial->stopBits();
        data.flow = this->serial->flowControl();

        data.name = this->serial->deviceName();

        this->optionsWidget->updateOptions(data);
    }
}

