#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QPlainTextEdit>
#include <QMainWindow>
#include <QComboBox>
#include <QtCore/QDateTime>
#include <QtGui/QScrollBar>
#include <qwt_plot.h>
#include <QLayout>
#include <qwidget.h>
#include <QPushButton>
#include <serialdeviceenumerator.h>
#include <abstractserial.h>

namespace Ui {
    class MainWindow;
}

class InfoWidget;
class OptionsWidget;
class rec;
class Plot;

class MainWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void optionsApplied(bool applied, const QStringList &notAppliedList);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void changeEvent(QEvent *e);

private slots:
    //
    void procEnumerate(const QStringList &l);
    void procSerialMessages(const QString &msg, QDateTime dt);
    void procSerialDataReceive();
    void printTrace(const QByteArray &data);
    void procApplyOptions(const QStringList &list);
    // Proc buttons click
    void procControlButtonClick();
    void procOptionsButtonClick();
    void procInfoButtonClick();
    void procrec0ButtonClick();
    void procrec1ButtonClick();

    // Proc box item change
    void procBoxChange(const QString &item);

    void about();

private:

    Ui::MainWindow *ui;
    InfoWidget *infoWidget;
    OptionsWidget *optionsWidget;
    rec *recWidget0;
    rec *recWidget1;
    SerialDeviceEnumerator *enumerator;
    AbstractSerial *serial;

    void initMainWidgetCloseState();
    void initMainWidgetOpenState();
    bool initInfoWidget();
    bool initOptionsWidget();
    bool initRecWidget0();
    bool initRecWidget1();

    void initEnumerator();
    void deinitEnumerator();
    void initSerial();
    void deinitSerial();
    void initButtonConnections();
    void initBoxConnections();
    void deinitWidgets();

    void updateInfoData(const QString &name);
    void updateOptionsData();

    void createActions();
    void createMenus();
    void createToolBars();
    void createCentralWidget();

    QMenu *helpMenu;
    QPlainTextEdit *textEdit;
    QAction *aboutqtAct;
    QAction *aboutAct;
    QAction *controlButton;
    QAction *optionsButton;
    QAction *infoButton;
    QComboBox *portBox;
    QToolBar * tb;
    Plot *plot0;
    Plot *plot1;
    QGridLayout *layout;
    QPushButton *rec0;
    QPushButton *rec1;
    QByteArray dataArray;

};
#endif // MAINWINDOW_H
