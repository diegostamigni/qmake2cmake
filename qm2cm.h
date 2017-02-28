#ifndef qm2cm_H
#define qm2cm_H

#include <QMainWindow>
#include "ui_qm2cm.h"
#include "converter.h"
#include <QProgressBar>

class qm2cm : public QMainWindow, private Ui::Qm2Cm
{
Q_OBJECT
public:
    qm2cm();
    virtual ~qm2cm();

private:
    Converter conv;
    QProgressBar progress;

private slots:
    void folderSelected();
    void startFolderSelect();
    void startConversion();

public slots:
    void showProgress(qreal p, QString what);
    void convDone();
};

#endif // qm2cm_H
