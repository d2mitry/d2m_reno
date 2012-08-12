#include "rec.h"
#include "ui_rec.h"
#include <QMessageBox>
#include <QDir>
#include <QFile>

rec::rec(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rec)
{
    ui->setupUi(this);
    this->initButtonConnections();
}

rec::~rec()
{
    delete ui;
}

//Public methods
void rec::SetrecYes()
{
    recYes = true;
}
bool rec::GetrecYes()
{
    return recYes;
}
//Private methods
void rec::initButtonConnections()
{
    connect(ui->openDirButton, SIGNAL(clicked()), this, SLOT(openDir()));
    connect(ui->ApplyButton, SIGNAL(clicked()), this, SLOT(ApplyClick()));
}

void rec::openDir()
{
    QString dirName = QFileDialog::getExistingDirectory(this);
            if (!dirName.isEmpty())
            ui->lineDir->setText(dirName);//    loadFile(fileName);
}

void rec::ApplyClick()
{
    QString dirName = ui->lineDir->text();
    QString lineID = ui->lineID->text();
    QString lineName = ui->lineName->text();
    if (!dirName.isEmpty() && !lineName.isEmpty() && !lineID.isEmpty())
        {
               QString fileName = lineID+"_"+lineName;
               QDir patients(dirName);
               if(patients.mkdir(fileName))
               {
                   patients.cd(fileName);
                   QString tmpfile = patients.filePath(fileName+".dat");
                   emit this->applyRectmpfile(tmpfile);
               }
               else
               {
                   qWarning("Can not create Patient's folder");
               }

        }
    else
        {
               QMessageBox::about(this, tr("Error"),
                 tr("Fill all fields, please!"));
        }



}


