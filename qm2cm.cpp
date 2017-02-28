#include "qm2cm.h"

#include <QFileDialog>
#include <QDebug>


qm2cm::qm2cm()
  : QMainWindow(), conv(), progress(this)
{
    setupUi(this);

    connect(le_base, SIGNAL(editingFinished()), this, SLOT(folderSelected()));
    connect(pb_base, SIGNAL(clicked(bool)), this, SLOT(startFolderSelect()));
    connect(pb_start, SIGNAL(clicked(bool)), this, SLOT(startConversion()));
}

qm2cm::~qm2cm()
{}


void qm2cm::startFolderSelect()
{
    QString f = QFileDialog::getExistingDirectory(this,
                tr("Please select the base Folder of your QMake project"));
    if(!f.isEmpty())
    {
        le_base->setText(f);
        folderSelected();
    }
}



void qm2cm::folderSelected()
{
    if(!le_base->text().isEmpty())
    {
        QDir basedir(le_base->text());
        QStringList profiles = basedir.entryList(QStringList() << "*.pro",
                                                 QDir::Files | QDir::Readable);
        if(profiles.count() > 0)
        {
            qDebug()<<profiles;
            te_disp->clear();
            QFile profile(basedir.absolutePath() + "/" + profiles.first());
            profile.open(QIODevice::ReadOnly);
            while(!profile.atEnd())
            {
                QString line = profile.readLine();
                while(line.endsWith("\\\n") && !profile.atEnd())
                {
                    line.remove(line.length()-2,2);
                    line += profile.readLine();
                }
                te_disp->appendPlainText(line.simplified());
            }
            profile.close();
        }
    }
}


void qm2cm::startConversion()
{
    statusbar->showMessage(tr("checking Folders"));
    conv.setBaseDir(le_base->text());
    statusbar->addPermanentWidget(&progress);
    connect(&conv, SIGNAL(progress(qreal,QString)),
            this, SLOT(showProgress(qreal,QString)));
    connect(&conv, SIGNAL(finished()),
            this, SLOT(convDone()));
    conv.start();
}


void qm2cm::showProgress(qreal p, QString what)
{
    progress.setValue(p*100);
    progress.setFormat(QString("%1 - %%p%").arg(what));
}


void qm2cm::convDone()
{
    statusbar->removeWidget(&progress);
    statusbar->showMessage(tr("conversion finished..."));
    QFile fi(le_base->text()+"/CMakeLists.txt");
    if(fi.open(QIODevice::ReadOnly))
    {
        te_disp->clear();
        te_disp->appendPlainText(fi.readAll());
        fi.close();
    }
}

#include "qm2cm.moc"
