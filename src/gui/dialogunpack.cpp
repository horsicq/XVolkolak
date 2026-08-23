#include "dialogunpack.h"
#include "ui_dialogunpack.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QThread>

#include "unpackworker.h"

DialogUnpack::DialogUnpack(const EmulatorUnpacker::Options &baseOptions, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogUnpack)
    , m_baseOptions(baseOptions)
{
    ui->setupUi(this);
    setModal(true);

    const QString resultDirectory = m_baseOptions.resultDirectory.trimmed().isEmpty()
                                        ? EmulatorUnpacker::defaultResultDirectory(m_baseOptions.inputPath)
                                        : m_baseOptions.resultDirectory.trimmed();
    ui->lineEditResultDirectory->setText(resultDirectory);

    ui->comboBoxPacker->addItems(EmulatorUnpacker::availablePackers());
    const int requestedPacker = ui->comboBoxPacker->findText(m_baseOptions.packerName);
    if(requestedPacker >= 0) {
        ui->comboBoxPacker->setCurrentIndex(requestedPacker);
    }
    ui->checkBoxForce->setChecked(m_baseOptions.forceOverwrite);
    ui->checkBoxFixImports->setChecked(m_baseOptions.fixImports);
    ui->checkBoxFixRelocs->setChecked(m_baseOptions.fixRelocations);
    ui->checkBoxSaveOverlay->setChecked(m_baseOptions.saveOverlay);

    ui->plainTextEditLog->setPlainText(tr("Ready."));
    ui->progressBar->setRange(0, 0);
    ui->progressBar->setVisible(false);

    connect(ui->buttonBox->button(QDialogButtonBox::Ok),
            &QPushButton::clicked,
             this,
             &DialogUnpack::startUnpack);
    connect(ui->pushButtonBrowseResultDirectory, &QPushButton::clicked, this, &DialogUnpack::browseResultDirectory);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DialogUnpack::reject);
}

DialogUnpack::~DialogUnpack()
{
    cancelAndWait();
    delete ui;
}

EmulatorUnpacker::Options DialogUnpack::selectedOptions() const
{
    EmulatorUnpacker::Options options = m_baseOptions;
    options.resultDirectory = ui->lineEditResultDirectory->text().trimmed();
    options.packerName = ui->comboBoxPacker->currentText();
    options.forceOverwrite = ui->checkBoxForce->isChecked();
    options.fixImports = ui->checkBoxFixImports->isChecked();
    options.fixRelocations = ui->checkBoxFixRelocs->isChecked();
    options.saveOverlay = ui->checkBoxSaveOverlay->isChecked();
    return options;
}

void DialogUnpack::browseResultDirectory()
{
    QString currentDirectory = ui->lineEditResultDirectory->text().trimmed();
    if(currentDirectory.isEmpty()) {
        currentDirectory = EmulatorUnpacker::defaultResultDirectory(m_baseOptions.inputPath);
    }

    const QString directory = QFileDialog::getExistingDirectory(this,
                                                                 tr("Select result directory"),
                                                                 currentDirectory,
                                                                 QFileDialog::ShowDirsOnly);
    if(!directory.isEmpty()) {
        ui->lineEditResultDirectory->setText(QDir(directory).absolutePath());
    }
}

void DialogUnpack::reject()
{
    if(m_thread) {
        if(!m_closeAfterFinish) {
            m_closeAfterFinish = true;
            ui->plainTextEditLog->appendPlainText(tr("Cancelling..."));
            ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);
            if(m_worker) {
                m_worker->requestCancel();
            }
        }
        return;
    }

    QDialog::reject();
}

void DialogUnpack::startUnpack()
{
    if(m_thread || (m_hasUnpackResult && m_unpackResult.success)) {
        return;
    }

    EmulatorUnpacker::Options options = selectedOptions();
    if(options.resultDirectory.isEmpty()) {
        QMessageBox::warning(this, tr("Unpack"), tr("Result directory is not set."));
        return;
    }

    ui->plainTextEditLog->clear();
    m_closeAfterFinish = false;
    m_hasUnpackResult = false;
    m_unpackResult = EmulatorUnpacker::Result();

    m_thread = new QThread(this);
    m_worker = new UnpackWorker(options);
    m_worker->moveToThread(m_thread);

    connect(m_thread, &QThread::started, m_worker, &UnpackWorker::process);
    connect(m_worker, &UnpackWorker::logLine, this, &DialogUnpack::appendLog);
    connect(m_worker, &UnpackWorker::finished, this, &DialogUnpack::onWorkerFinished);

    setRunningUi();
    m_thread->start();
}

void DialogUnpack::appendLog(const QString &line)
{
    ui->plainTextEditLog->appendPlainText(line);
}

void DialogUnpack::onWorkerFinished()
{
    if(!m_thread || !m_worker) {
        return;
    }

    m_unpackResult = m_worker->result();
    m_hasUnpackResult = true;

    m_thread->quit();
    m_thread->wait();
    delete m_worker;
    delete m_thread;
    m_worker = nullptr;
    m_thread = nullptr;

    setFinishedUi();

    if(m_closeAfterFinish) {
        QDialog::reject();
        return;
    }

    if(!m_unpackResult.success && !isCancelledResult(m_unpackResult)) {
        QString message = m_unpackResult.messages.join(QLatin1Char('\n'));
        if(message.isEmpty()) {
            message = m_unpackResult.reason;
        }
        if(message.isEmpty()) {
            message = tr("The unpack operation failed.");
        }
        QMessageBox::warning(this, tr("Unpack failed"), message);
    }
}

void DialogUnpack::setRunningUi()
{
    ui->lineEditResultDirectory->setEnabled(false);
    ui->pushButtonBrowseResultDirectory->setEnabled(false);
    ui->comboBoxPacker->setEnabled(false);
    ui->checkBoxForce->setEnabled(false);
    ui->checkBoxFixImports->setEnabled(false);
    ui->checkBoxFixRelocs->setEnabled(false);
    ui->checkBoxSaveOverlay->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
    ui->progressBar->setVisible(true);
}

void DialogUnpack::setFinishedUi()
{
    const bool canRetry = !m_unpackResult.success;
    ui->lineEditResultDirectory->setEnabled(canRetry);
    ui->pushButtonBrowseResultDirectory->setEnabled(canRetry);
    ui->comboBoxPacker->setEnabled(canRetry);
    ui->checkBoxForce->setEnabled(canRetry);
    ui->checkBoxFixImports->setEnabled(canRetry);
    ui->checkBoxFixRelocs->setEnabled(canRetry);
    ui->checkBoxSaveOverlay->setEnabled(canRetry);
    ui->progressBar->setVisible(false);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(canRetry);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(true);
}

void DialogUnpack::cancelAndWait()
{
    if(!m_thread) {
        return;
    }

    if(m_worker) {
        m_worker->requestCancel();
    }
    m_thread->quit();
    m_thread->wait();
    delete m_worker;
    delete m_thread;
    m_worker = nullptr;
    m_thread = nullptr;
}

bool DialogUnpack::isCancelledResult(const EmulatorUnpacker::Result &result)
{
    return !result.success && result.reason.contains(QStringLiteral("cancelled"), Qt::CaseInsensitive);
}
