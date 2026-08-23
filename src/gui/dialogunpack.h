#ifndef DIALOGUNPACK_H
#define DIALOGUNPACK_H

#include <QDialog>

#include "emulatorunpacker.h"

namespace Ui {
class DialogUnpack;
}

class QThread;
class UnpackWorker;

class DialogUnpack : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUnpack(const EmulatorUnpacker::Options &baseOptions, QWidget *parent = nullptr);
    ~DialogUnpack() override;

    EmulatorUnpacker::Options selectedOptions() const;
    bool hasUnpackResult() const { return m_hasUnpackResult; }
    const EmulatorUnpacker::Result &unpackResult() const { return m_unpackResult; }

public slots:
    void reject() override;

private slots:
    void browseResultDirectory();
    void startUnpack();
    void appendLog(const QString &line);
    void onWorkerFinished();

private:
    void setRunningUi();
    void setFinishedUi();
    void cancelAndWait();
    static bool isCancelledResult(const EmulatorUnpacker::Result &result);

    Ui::DialogUnpack *ui;
    EmulatorUnpacker::Options m_baseOptions;
    EmulatorUnpacker::Result m_unpackResult;
    QThread *m_thread = nullptr;
    UnpackWorker *m_worker = nullptr;
    bool m_hasUnpackResult = false;
    bool m_closeAfterFinish = false;
};

#endif  // DIALOGUNPACK_H
