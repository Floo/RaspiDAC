
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

#include <string>
#include <iostream>

#include <QApplication>
#include <QDebug>

#ifdef __rpi__
    #include "rpicontrol/rpigpio.h"
    #include "wiringPi.h"
#endif

#include <libupnpp/upnpplib.hxx>
#include <libupnpp/log.hxx>

#include "application.h"

//using namespace UPnPClient;
using namespace UPnPP;

static const char *thisprog;

static int    op_flags;
#define OPT_h     0x4
#define OPT_c     0x20

static const char usage [] =  "\n";

//#ifdef __rpi__
//RPiGPIO *rpiGPIO;

//void cbTaster1()
//{
//    qDebug() << "Taster1 gedrückt";
//    emit rpiGPIO->taster1();;
//}

//void cbTaster2()
//{
//    qDebug() << "Taster2 gedrückt";
//    emit rpiGPIO->taster2();
//}

//void cbTaster3()
//{
//    qDebug() << "Taster3 gedrückt";
//    emit rpiGPIO->taster3();
//}

//void cbTaster4()
//{
//    qDebug() << "Taster4 gedrückt";
//    emit rpiGPIO->taster4();
//}
//#endif

static void Usage(void)
{
    FILE *fp = (op_flags & OPT_h) ? stdout : stderr;
    fprintf(fp, "%s: Usage: %s", thisprog, usage);
    exit((op_flags & OPT_h)==0);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    string a_config;

    QStringList params;
    for(int i = 1; i < argc; i++){
        QString param(argv[i]);
        params.push_back(param);
    }

    thisprog = argv[0];
    argc--; argv++;

    while (argc > 0 && **argv == '-') {
        (*argv)++;
        if (!(**argv))
            Usage();
        while (**argv)
            switch (*(*argv)++) {
            case 'c':   op_flags |= OPT_c; if (argc < 2)  Usage();
                a_config = *(++argv);
                argc--; goto b1;
            default: Usage();
            }
    b1: argc--; argv++;
    }

    if (argc > 0)
        Usage();

    if (Logger::getTheLog("stderr") == 0) {
        cerr << "Can't initialize log" << endl;
        return 1;
    }
    const char *cp;
    if ((cp = getenv("UPPLAY_LOGLEVEL"))) {
        Logger::getTheLog("")->setLogLevel(Logger::LogLevel(atoi(cp)));
    }

    LibUPnP *mylib = LibUPnP::getLibUPnP();
    if (!mylib) {
        cerr << "Can't get LibUPnP" << endl;
        return 1;
    }
    if (!mylib->ok()) {
        cerr << "Lib init failed: " <<
            mylib->errAsString("main", mylib->getInitError()) << endl;
        return 1;
    }
    if ((cp = getenv("UPPLAY_UPNPLOGFILENAME"))) {
        char *cp1 = getenv("UPPLAY_UPNPLOGLEVEL");
        int loglevel = LibUPnP::LogLevelNone;
        if (cp1) {
            loglevel = atoi(cp1);
        }
        loglevel = loglevel < 0 ? 0: loglevel;
        loglevel = loglevel > int(LibUPnP::LogLevelDebug) ?
            int(LibUPnP::LogLevelDebug) : loglevel;

        if (loglevel != LibUPnP::LogLevelNone) {
            mylib->setLogFileName(cp, LibUPnP::LogLevel(loglevel));
        }
    }

//#ifdef __rpi__
//    wiringPiISR(GPIO05, INT_EDGE_FALLING, &cbTaster1);
//    wiringPiISR(GPIO06, INT_EDGE_FALLING, &cbTaster2);
//    wiringPiISR(GPIO13, INT_EDGE_FALLING, &cbTaster3);
//    wiringPiISR(GPIO12, INT_EDGE_FALLING, &cbTaster4);
//#endif

    Application application(&a,params.size(), 0);
    if(!application.is_initialized())
        return 1;

    return a.exec();
}
