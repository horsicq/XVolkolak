#include <QApplication>
#include <QCoreApplication>
#include <QIcon>

#include "../global.h"
#include "guimainwindow.h"
#include "xoptions.h"

namespace {

void configureApplicationMetadata()
{
    QCoreApplication::setOrganizationName(X_ORGANIZATIONNAME);
    QCoreApplication::setOrganizationDomain(X_ORGANIZATIONDOMAIN);
    QCoreApplication::setApplicationName(X_APPLICATIONNAME);
    QCoreApplication::setApplicationVersion(X_APPLICATIONVERSION);
}

}  // namespace

int main(int argc, char *argv[])
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    configureApplicationMetadata();

    QApplication application(argc, argv);
    application.setWindowIcon(QIcon(QStringLiteral(":/images/logo.png")));

#ifdef Q_OS_LINUX
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    application.setDesktopFileName(QStringLiteral("xvolkolak"));
#endif
#endif

    XOptions options;
    options.setName(X_OPTIONSFILE);
    options.addID(XOptions::ID_VIEW_STYLE, QStringLiteral("Fusion"));
    options.addID(XOptions::ID_VIEW_LANG, QStringLiteral("System"));
    options.addID(XOptions::ID_VIEW_QSS, QStringLiteral(""));
    options.load();
    XOptions::adjustApplicationView(X_APPLICATIONNAME, &options);

    GuiMainWindow window;
    window.show();

    return application.exec();
}
