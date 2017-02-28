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

#include "converter.h"
#include <QDir>
#include <QFile>
#include <QRegExp>

#include <QDebug>

void Converter::run()
{
    qreal max = Files.count() * 3, now=0;
    foreach(QString file2do,Files)
    {
        emit progress(now/max,"parse "+file2do);now += 1;
        if(parseProFile(file2do))
        {
            emit progress(now/max,"parse "+file2do);now += 1;
            prepareCmakeSettings(file2do);
            emit progress(now/max,"parse "+file2do);now += 1;
            writeCMakeLists(file2do);
        } else {emit progress(now/max,"nix damit: "+file2do);now += 2;}
    }
}


void Converter::setBaseDir(QString basis)
{
    // What are we going to do?
    // We'll scan the basedir for the *.pro file, read it and add the content
    // to our Files list.  If it contains subdirs, we'll also add them here...
    QStringList dirs(basis),liste;
    QString string;
    QDir base;
    QRegExp templ("\\s*TEMPLATE\\s*[+]?=(.*)"),
            subre("\\s*SUBDIRS\\s*[+]?=(.*)",
                  Qt::CaseSensitive,QRegExp::RegExp2);
    int i;

    base.setFilter(QDir::Files | QDir::Readable);
    base.setNameFilters(QStringList()<<"*.pro");

    while(dirs.count())
    {
        qDebug()<<"DIRS loop:"<<dirs;

        base.setPath(dirs.first());
        liste = base.entryList();
        if(liste.count() > 0)
        {
            Files << QString("%1/%2").arg(dirs.first()).arg(liste.first());
            qDebug()<<"found: "<<liste.first();
            QFile dieses(Files.last());
            dieses.open(QIODevice::ReadOnly);
            string = dieses.readAll();
            dieses.close();
            string.remove("\\\n");
            liste = string.split('\n');
            i = liste.indexOf(templ);
            if(i > -1)
            {
                string = templ.cap(1);
                if(string.contains("subdirs",Qt::CaseInsensitive))
                {
                    i = liste.indexOf(subre);
                    if(i > -1)
                    {
                        string = subre.cap(1).simplified();
                        if(string.contains('"'))
                        {
                            liste = string.split('"',QString::SkipEmptyParts);
                            for(i=0;i<liste.count();i++)
                                liste[i] = liste[i].trimmed();
                            liste.removeAll("");
                        } else {
                            liste = string.split(QRegExp("\\s+"),QString::SkipEmptyParts);
                        }
                        foreach(string, liste)
                            dirs.append(QString("%1/%2").arg(dirs.first()).arg(string));
                    }
                }
            }
        }
        dirs.takeFirst();
    }
    qDebug()<<"QMake files to do:"<<Files;
}


void Converter::initQMakeVars()
{
    DESTDIR = "";
    CONFIG = QStringList() << "qt" << "warn_on" << "release" << "incremental" << "link_prl";
    QT = QStringList() << "core" << "gui";
    DEFINES.clear();
    SOURCES.clear();
    HEADERS.clear();
    FORMS.clear();
    DISTFILES.clear();
    INCLUDEPATH.clear();
    LIBS.clear();
    RESOURCES.clear();
    SUBDIRS.clear();
    TARGET.clear();
    TEMPLATE.clear();
    TRANSLATIONS.clear();
    TOMOC.clear();
}


bool Converter::parseProFile(QString proFile)
{
    QFile file(proFile);
    QStringList lines;
    QString work;
    QRegExp liner("^\\s*(\\w+)\\s*([-+]?=)\\s*(.*)$",Qt::CaseInsensitive,QRegExp::RegExp2);
    int op; QStringList *sl;

    if(file.open(QIODevice::ReadOnly))
    {
        work = file.readAll();
        file.close();
        qDebug()<< "parsing QMake file:" << proFile.section('/',-1);
        // make multilines simple
        work.remove("\\\n");
        // get the lines
        lines = work.split('\n',QString::SkipEmptyParts);

        initQMakeVars();
        // now parse the lines ...
        while(lines.count())
        {
            // capture all the contents...
            lines.first().indexOf(liner);
            // cap(1) is the variable, cap(2) is the operator
            if(liner.captureCount() > 0)
            {
                if(liner.cap(1) == "TEMPLATE")
                {
                    TEMPLATE = liner.cap(3);
                }
                else if(liner.cap(1) == "TARGET")
                {
                    TARGET = liner.cap(3);
                }
                else if(liner.cap(1) == "DESTDIR")
                {
                    DESTDIR = liner.cap(3);
                }
                else
                {
                    sl = 0;
                    if(liner.cap(2) == "-=") {
                        op = -1;
                    } else if(liner.cap(2) == "+=") {
                        op = +1;
                    } else op = 0;

                    if(liner.cap(1) == "CONFIG")
                        sl = &CONFIG;
                    else if(liner.cap(1) == "QT")
                        sl = &QT;
                    else if(liner.cap(1) == "DEFINES")
                        sl = &DEFINES;
                    else if(liner.cap(1) == "SOURCES")
                        sl = &SOURCES;
                    else if(liner.cap(1) == "HEADERS")
                        sl = &HEADERS;
                    else if(liner.cap(1) == "FORMS")
                        sl = &FORMS;
                    else if(liner.cap(1) == "DISTFILES")
                        sl = &DISTFILES;
                    else if(liner.cap(1) == "INCLUDEPATH")
                        sl = &INCLUDEPATH;
                    else if(liner.cap(1) == "LIBS")
                        sl = &LIBS;
                    else if(liner.cap(1) == "RESOURCES")
                        sl = &RESOURCES;
                    else if(liner.cap(1) == "SUBDIRS")
                        sl = &SUBDIRS;
                    else if(liner.cap(1) == "TRANSLATIONS")
                        sl = &TRANSLATIONS;

                    if(sl) operateOnVar(sl,op,liner.cap(3));
                }
            }
            else    // nothing captured with our regex - parsing the line needed!
            {
                //TODO: write something complicated here ...
            }
            lines.takeFirst();  //just that the loop won't be infinite...
        }
        if(TEMPLATE.isEmpty()) TEMPLATE = "app";
        qDebug()<<"CONFIG:"<<CONFIG;
        qDebug()<<"QT:"<<QT;
        qDebug()<<"DEFINES:"<<DEFINES;
        qDebug()<<"DISTFILES:"<<DISTFILES;
        qDebug()<<"LIBS:"<<LIBS;
        qDebug()<<"SUBDIRS:"<<SUBDIRS;
        qDebug()<<"INCLUDEPATH:"<<INCLUDEPATH;
        qDebug()<<"TEMPLATE:"<<TEMPLATE<<", TARGET:"<<TARGET<<", DESTDIR:"<<DESTDIR;
    } else return false;
    return true;
}


void Converter::operateOnVar(QStringList*& var, int &op, QString val)
{
    // First part: split value correctly
    // - it can contain quoted strings
    // - or it simply contains space-separated strings ...
    // -> make them all quoted for that purpose, split them, and remove quotes
    //    afterwards ...
    int i; QString v=val;
    i = 0;
    while(i<v.count())
    {
        while((i<v.count()) && (v[i].isSpace())) i++;
        if(v[i] != '"')
        {
            v.insert(i++,'"');
            while((i<v.count()) && (!v[i].isSpace())) i++;
            v.insert(i++,"\"+++");
        } else {
            ++i;
            while((i<v.count()) && (v[i] != '"')) i++;
            v.insert(++i,"+++");
        }
        i+=3;
    }
    //2nd part: split string ...
    QStringList lst = v.split(QRegExp("\"[+]{3}\\s*\"?"),QString::SkipEmptyParts);
    v = lst.takeFirst(); v.remove('"'); lst.insert(0,v);
    v = lst.takeLast(); v.remove('"'); lst << v;
    qDebug()<< lst;
    //3rd part: operate on the variable...
    switch(op)
    {
        case -1:    //"-="
            foreach(QString item, lst)
            {
                var->removeAll(item);
            }
            break;
        case 0:
            var->clear();
        case 1:
            *var << lst;
            break;
    }
}


void Converter::prepareCmakeSettings(QString proFile)
{
    /*  TODOs:
        Here we will have to check some stuff.  F.e. which files need "mocing"
        and other things we might need to include in our CMakeLists.

        The qmake-way for mocing was to detect the Q_OBJECT macro (as far as I
        understood).  Actually this macro shouldn't be found in sources, only
        in headers.  But we cannot make sure this wouldn't be the case.  So we
        will just grep all source and header files for the Q_OBJECT macro.  If
        it's in any of the lists, we add it to the "tomoc" list and remove it
        from the other.
    */
    int i = 0; QString dir;
    dir = proFile.section('/',0,-2);
    while(i < HEADERS.count())
    {
        if(findQOMacro(QString("%1/%2").arg(dir).arg(HEADERS[i])))
        {
            TOMOC << HEADERS.takeAt(i);
        } else ++i;
    }
    i=0;
    while(i < SOURCES.count())
    {
        if(findQOMacro(QString("%1/%2").arg(dir).arg(SOURCES[i])))
        {
            TOMOC << SOURCES.takeAt(i);
        } else ++i;
    }
}


bool Converter::findQOMacro(QString fn)
{
    QRegExp qom("^.*Q_OBJECT\\s*$"); QFile file(fn); bool found(false);

    if(file.open(QIODevice::ReadOnly))
    {
        while(!found && !file.atEnd())
        {
            if(qom.exactMatch(file.readLine())) found=true;
        }
        file.close();
        return found;
    } else return false;
}


void Converter::writeCMakeLists(QString proFile)
{
    /*  Let's start writing a CMakeLists.txt file ...
    */
    QString links,help,fn = proFile.section('/',0,-2) + "/CMakeLists.txt";
    QFile file(fn); QTextStream text(&file); int i;

#ifdef _DEBUG
    if(file.exists()) file.remove();
#endif
    if(file.open(QIODevice::WriteOnly))
    {
        if(!TARGET.isEmpty()) {
            text << QString("project( %1 )\n").arg(TARGET);
        } else    TARGET = proFile.section('/',0,-2).section('/',-1);
        text << "cmake_minimum_required( VERSION 3.1 )\n";
        text << "set( CMAKE_CXX_STANDARD 14 )\n";
        text << "set( CMAKE_INCLUDE_CURRENT_DIR ON )\n";
        text << "set( CMAKE_AUTOMOC ON )\n";
        // check the config-variable and take respective action
        QRegExp buildtype(".*(debug|release|debug_and_release).*",Qt::CaseInsensitive);
        i = CONFIG.lastIndexOf(buildtype);
        if(!buildtype.cap(1).isEmpty())
        {
            // buildtype is set - so let's set it for CMake
            text << "set( CMAKE_BUILD_TYPE ";
            // workaround the "debug_and_release" option: just build release
            // with debug info ...
            if(buildtype.cap(1)=="debug_and_release")
                text << "RelWithDebInfo";
            else if(buildtype.cap(1)=="debug")
                text << "Debug";
            else text << "Release";
            text << " )\n";
        }

        QRegExp warnings(".*(warn_on|warn_off).*",Qt::CaseInsensitive);
        i = CONFIG.lastIndexOf(warnings);
        if(!warnings.cap(1).isEmpty() || CONFIG.contains("largefile"))
        {
            text << "add_definitions ( ";
            if(warnings.cap(1) == "warn_on")
                text << "-Wall ";
            else
                text << "-w ";
            if(CONFIG.contains("largefile"))
                text << "-D_FILE_OFFSET_BITS=64 ";
            text << ")\n";
        }

        if(CONFIG.contains("qt"))
        {
            text << "find_package ( Qt5Widgets REQUIRED )\n";
            text << "find_package ( Qt5Core REQUIRED )\n";
            text << "find_package ( Qt5Gui REQUIRED )\n";
            links.replace("INCLUDE_DIR","LIBRARY");
        }

        if(!DESTDIR.isEmpty())
        {
            if(TEMPLATE.contains("app"))
                text << QString("set ( EXECUTABLE_OUTPUT_PATH %1 )\n").arg(DESTDIR);
            if(TEMPLATE.contains("lib"))
                text << QString("set ( LIBRARY_OUTPUT_PATH %1 )\n").arg(DESTDIR);
        }

        // check subdirs/app/lib and add directories/files accordingly
        if(TEMPLATE.contains("subdirs",Qt::CaseInsensitive))
        foreach(QString sd, SUBDIRS)
        {
            text << QString("add_subdirectory( %1 )\n").arg(sd);
        } else
        if(TEMPLATE.contains(QRegExp("(app)|(lib)")))
        {
            //write the source/header/ui/moc-stuff
            //prepare helper
            help = QString("${%1_SRCS}").arg(TARGET);
            // write headers
            text << QString("set ( %1_HDRS\n\t%2\n\t)\n\n")
                    .arg(TARGET).arg(HEADERS.join("\n\t"));
            // write sources
            text << QString("set ( %1_SRCS\n\t%2\n\t)\n\n")
                    .arg(TARGET).arg(SOURCES.join("\n\t"));
            // write forms
            if(!FORMS.isEmpty())
            {   text << QString("set ( %1_UIS\n\t%2\n\t)\n")
                    .arg(TARGET).arg(FORMS.join("\n\t"));
                text << QString("QT5_WRAP_UI(UIS ${%1_UIS})\n\n").arg(TARGET);
                help += " ${UIS}";
            }
            // write resources
            if(!RESOURCES.isEmpty())
            {   text << QString("set ( %1_RSCS\n\t%2\n\t)\n")
                    .arg(TARGET).arg(RESOURCES.join("\n\t"));
                text << QString("QT5_ADD_RESOURCES(RSCS ${%1_RSCS})\n\n")
                    .arg(TARGET);
                help += " ${RSCS}";
            }
            // write translations
            if(!TRANSLATIONS.isEmpty())
            {   text << QString("set ( %1_TRS\n\t%2\n\t)\n")
                    .arg(TARGET).arg(TRANSLATIONS.join("\n\t"));
                text << QString("QT5_ADD_TRANSLATION(TRS ${%1_TRS})\n\n")
                    .arg(TARGET);
                help += " ${TRS}";
            }
            // write 2Bmoc-ed
            if(!TOMOC.isEmpty())
            {   text << QString("set ( %1_MOCS\n\t%2\n\t)\n")
                    .arg(TARGET).arg(TOMOC.join("\n\t"));
                text << QString("QT5_WRAP_CPP(MOCS ${%1_MOCS})\n\n").arg(TARGET);
                help += " ${MOCS}";
            }
            if(TEMPLATE == "app")
            {
                text << QString("add_executable ( %1 ").arg(TARGET);
            } else { //"lib" (or even a plugin
                text << QString("add_library ( %1 ").arg(TARGET);
                if(CONFIG.contains("plugin"))
                    text << "MODULE ";
                else
                    text << "SHARED ";
            }
            text << QString("${%1_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )\n").arg(TARGET);
            text << QString("target_link_libraries ( %1 %2 Qt5::Core Qt5::Gui Qt5::Widgets )\n").arg(TARGET)
                    .arg(links);
        }
        text.flush();
        file.close();
    }
}

#include "converter.moc"