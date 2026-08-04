#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QTextStream>

#include "../global.h"
#include "emulatorunpacker.h"

namespace {

void configureApplicationMetadata()
{
    QCoreApplication::setOrganizationName(X_ORGANIZATIONNAME);
    QCoreApplication::setOrganizationDomain(X_ORGANIZATIONDOMAIN);
    QCoreApplication::setApplicationName(X_APPLICATIONNAME);
    QCoreApplication::setApplicationVersion(X_APPLICATIONVERSION);
}

// PDSTRUCT callback for --verbose: print each engine info line (the current info string) to the
// stream passed as user data. A named function, not a lambda, per the project's C++ style.
void consoleLogCallback(void *pUserData, XBinary::PDSTRUCT *pPdStruct)
{
    QTextStream *pOut = static_cast<QTextStream *>(pUserData);
    *pOut << pPdStruct->sInfoString << '\n';
    pOut->flush();
}

}  // namespace

int main(int argc, char *argv[])
{
    configureApplicationMetadata();

    QCoreApplication application(argc, argv);

    QTextStream out(stdout);
    QTextStream err(stderr);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("%1 - emulation-based PE unpacker").arg(X_APPLICATIONDISPLAYNAME));
    parser.addHelpOption();
    parser.addVersionOption();

    const QCommandLineOption resultDirectoryOption(QStringList() << QStringLiteral("r") << QStringLiteral("result-directory"),
                                                   QStringLiteral("Store unpacked output below <directory>."),
                                                   QStringLiteral("directory"));
    const QCommandLineOption forceOption(QStringList() << QStringLiteral("f") << QStringLiteral("force"),
                                         QStringLiteral("Overwrite an existing derived output file."));
    const QCommandLineOption packerOption(QStringList() << QStringLiteral("p") << QStringLiteral("packer"),
                                          QStringLiteral("Packer family to use (default: generic auto). See --list-packers."),
                                          QStringLiteral("name"));
    const QCommandLineOption maxStepsOption(QStringList() << QStringLiteral("m") << QStringLiteral("max-steps"),
                                            QStringLiteral("Instruction budget before giving up (0 = unpacker default)."),
                                            QStringLiteral("count"));
    const QCommandLineOption verboseOption(QStringList() << QStringLiteral("verbose"),
                                           QStringLiteral("Stream engine log lines while unpacking."));
    const QCommandLineOption apiLogOption(QStringLiteral("apilog"),
                                          QStringLiteral("Print the emulated API-call log after unpacking."));
    const QCommandLineOption listOption(QStringLiteral("list-packers"),
                                        QStringLiteral("List the supported packer families and exit."));
    const QCommandLineOption noImportsOption(QStringLiteral("no-fix-imports"),
                                             QStringLiteral("Do not reconstruct the import table (raw dump, diagnostics only)."));
    const QCommandLineOption noRelocsOption(QStringLiteral("no-fix-relocs"),
                                            QStringLiteral("Do not reconstruct base relocations (faster; output loads at its preferred base only)."));
    const QCommandLineOption overlayOption(QStringLiteral("save-overlay"),
                                           QStringLiteral("Append the original packed file's overlay to the output."));

    parser.addOption(resultDirectoryOption);
    parser.addOption(forceOption);
    parser.addOption(packerOption);
    parser.addOption(maxStepsOption);
    parser.addOption(verboseOption);
    parser.addOption(apiLogOption);
    parser.addOption(listOption);
    parser.addOption(noImportsOption);
    parser.addOption(noRelocsOption);
    parser.addOption(overlayOption);
    parser.addPositionalArgument(QStringLiteral("input"), QStringLiteral("Input executable or packed binary."));
    parser.process(application);

    if(parser.isSet(listOption)) {
        const QStringList packers = EmulatorUnpacker::availablePackers();
        for(const QString &name : packers) {
            out << name << '\n';
        }
        return 0;
    }

    const QStringList positionalArguments = parser.positionalArguments();
    if(positionalArguments.isEmpty()) {
        parser.showHelp(1);
    }

    EmulatorUnpacker::Options options;
    options.inputPath = positionalArguments.at(0);
    options.resultDirectory = parser.value(resultDirectoryOption);
    options.forceOverwrite = parser.isSet(forceOption);
    options.packerName = parser.isSet(packerOption) ? parser.value(packerOption) : EmulatorUnpacker::genericPackerName();
    if(parser.isSet(maxStepsOption)) {
        options.maxSteps = parser.value(maxStepsOption).toLongLong();
    }
    options.fixImports = !parser.isSet(noImportsOption);
    options.fixRelocations = !parser.isSet(noRelocsOption);
    options.saveOverlay = parser.isSet(overlayOption);

    const bool verbose = parser.isSet(verboseOption);
    XBinary::PDSTRUCT pdStruct = XBinary::createPdStruct();
    if(verbose) {
        pdStruct.pCallback = consoleLogCallback;
        pdStruct.pCallbackUserData = &out;
    }

    const EmulatorUnpacker::Result result = EmulatorUnpacker::unpack(options, &pdStruct);

    if(!verbose) {
        QTextStream &stream = result.success ? out : err;
        for(const QString &message : result.messages) {
            stream << message << '\n';
        }
    }

    if(parser.isSet(apiLogOption)) {
        out << "--- API log (" << result.apiLog.size() << ") ---\n";
        for(const QString &line : result.apiLog) {
            out << line << '\n';
        }
    }

    return result.success ? 0 : 2;
}
