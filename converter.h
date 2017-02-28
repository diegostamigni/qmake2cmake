/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CONVERTER_H
#define CONVERTER_H

#include <QThread>
#include <QStringList>


class Converter : public QThread
{
    Q_OBJECT
protected:
    virtual void run();

public:
    void setBaseDir(QString basis);

private:
    QStringList Files,
                CONFIG,QT,DEFINES,SOURCES,HEADERS,FORMS,DISTFILES,INCLUDEPATH,
                LIBS,RESOURCES,SUBDIRS,TRANSLATIONS,
                TOMOC;

    QString     DESTDIR,TEMPLATE,TARGET;

    void initQMakeVars();
    bool findQOMacro(QString);
    bool parseProFile(QString proFile);
    void operateOnVar(QStringList*& var, int &op, QString val);
    void prepareCmakeSettings(QString proFile);
    void writeCMakeLists(QString proFile);
signals:
    void progress(qreal completion, QString action);
};

#endif // CONVERTER_H
