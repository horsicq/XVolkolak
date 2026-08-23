#include "guimainwindow.h"
#include "ui_guimainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QStatusBar>

#include "../global.h"
#include "dialogabout.h"
#include "dialogoptions.h"
#include "dialogunpack.h"
#include "emulatorunpacker.h"
#include "xscanengineoptionswidget.h"

GuiMainWindow::GuiMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);
    setWindowTitle(XOptions::getTitle(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION));

    m_xOptions.setName(X_OPTIONSFILE);
    m_xOptions.addID(XOptions::ID_VIEW_STAYONTOP, false);
    m_xOptions.addID(XOptions::ID_VIEW_STYLE, QStringLiteral("Fusion"));
    m_xOptions.addID(XOptions::ID_VIEW_LANG, QStringLiteral("System"));
    m_xOptions.addID(XOptions::ID_VIEW_QSS, QStringLiteral(""));
    m_xOptions.addID(XOptions::ID_VIEW_FONT_CONTROLS, XOptions::getDefaultFont().toString());
    m_xOptions.addID(XOptions::ID_VIEW_FONT_TABLEVIEWS, XOptions::getMonoFont().toString());
    m_xOptions.addID(XOptions::ID_VIEW_FONT_TREEVIEWS, XOptions::getDefaultFont().toString());
    m_xOptions.addID(XOptions::ID_VIEW_FONT_TEXTEDITS, XOptions::getMonoFont().toString());
    m_xOptions.addID(XOptions::ID_FILE_SAVELASTDIRECTORY, true);
    m_xOptions.addID(XOptions::ID_FILE_SAVERECENTFILES, true);
    m_xOptions.addID(XOptions::ID_FEATURE_READBUFFERSIZE, 8 * 1024);
    m_xOptions.addID(XOptions::ID_FEATURE_FILEBUFFERSIZE, 2 * 1024 * 1024);
    m_xOptions.addID(XOptions::ID_SCAN_ENGINE_NFD_ENABLED, true);
    XScanEngineOptionsWidget::setDefaultValues(&m_xOptions);
    m_xOptions.load();

    m_xShortcuts.setName(X_SHORTCUTSFILE);
    m_xShortcuts.setNative(m_xOptions.isNative(), m_xOptions.getApplicationDataPath());
    m_xShortcuts.load();

    // The NFD scan-results tree uses the same persistent options and shortcuts context as
    // the application options dialog.
    ui->widgetDetect->setGlobal(&m_xShortcuts, &m_xOptions);

    connect(ui->pushButtonBrowseInput, &QPushButton::clicked, this, &GuiMainWindow::browseInputFile);
    connect(ui->pushButtonUnpack, &QPushButton::clicked, this, &GuiMainWindow::showUnpackDialog);
    connect(ui->pushButtonOptions, &QPushButton::clicked, this, &GuiMainWindow::showOptionsDialog);
    connect(ui->pushButtonAbout, &QPushButton::clicked, this, &GuiMainWindow::showAboutDialog);
    connect(ui->pushButtonExit, &QPushButton::clicked, this, &GuiMainWindow::close);
    connect(ui->lineEditInput, &QLineEdit::textChanged, this, &GuiMainWindow::updateActions);
    connect(ui->lineEditInput, &QLineEdit::textChanged, this, &GuiMainWindow::scanInput);

    statusBar()->showMessage(tr("Ready"));
    adjustView();
    updateActions();
}

GuiMainWindow::~GuiMainWindow()
{
    m_xOptions.save();
    m_xShortcuts.save();

    delete ui;
}

void GuiMainWindow::scanInput(const QString &fileName)
{
    // Show the detector's tree for the current input file (empty clears it).
    const QString trimmed = fileName.trimmed();
    if(trimmed.isEmpty() || !QFileInfo::exists(trimmed)) {
        ui->widgetDetect->setData(QString(), false);
        return;
    }

    m_xOptions.setLastFileName(trimmed);
    ui->widgetDetect->setData(trimmed, m_xOptions.isScanAfterOpen());
}

void GuiMainWindow::browseInputFile()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Open input binary"),
                                                          m_xOptions.getLastDirectory(),
                                                          tr("Executable files (*.exe *.com *.dll *.sys);;All files (*)"));
    if(fileName.isEmpty()) {
        return;
    }

    ui->lineEditInput->setText(QFileInfo(fileName).absoluteFilePath());
}

void GuiMainWindow::showUnpackDialog()
{
    m_unpackOptions.inputPath = ui->lineEditInput->text();
    DialogUnpack dialog(m_unpackOptions, this);
    dialog.exec();
    m_unpackOptions = dialog.selectedOptions();

    if(!dialog.hasUnpackResult()) {
        return;
    }

    const EmulatorUnpacker::Result &result = dialog.unpackResult();
    const bool cancelled = !result.success && result.reason.contains(QStringLiteral("cancelled"), Qt::CaseInsensitive);

    if(result.success) {
        statusBar()->showMessage(tr("Completed: OEP RVA 0x%1 (%2)").arg(result.oepRva, 0, 16).arg(result.method));
    } else if(cancelled) {
        statusBar()->showMessage(tr("Cancelled"));
    } else {
        statusBar()->showMessage(tr("Failed"));
    }
}

void GuiMainWindow::showAboutDialog()
{
    DialogAbout dialog(this);
    dialog.exec();
}

void GuiMainWindow::showOptionsDialog()
{
    DialogOptions dialog(this, &m_xOptions, XOptions::GROUPID_FILE);
    dialog.setGlobal(&m_xShortcuts, &m_xOptions);
    dialog.exec();

    adjustView();
}

void GuiMainWindow::adjustView()
{
    if(m_xOptions.isIDPresent(XOptions::ID_VIEW_STAYONTOP)) {
        m_xOptions.adjustStayOnTop(this);
    }

    m_xOptions.adjustWidget(this, XOptions::ID_VIEW_FONT_CONTROLS);
    ui->widgetDetect->adjustView();
}

void GuiMainWindow::updateActions()
{
    ui->pushButtonUnpack->setEnabled(!ui->lineEditInput->text().trimmed().isEmpty());
}
