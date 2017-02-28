#include <QApplication>
#include "qm2cm.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    qm2cm foo;
    foo.show();
    return app.exec();
}
