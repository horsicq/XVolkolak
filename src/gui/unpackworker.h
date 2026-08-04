#ifndef UNPACKWORKER_H
#define UNPACKWORKER_H

#include <QObject>

#include "xbinary.h"
#include "emulatorunpacker.h"

// Runs a single EmulatorUnpacker job off the GUI thread, streaming engine log lines
// back as queued signals so the window stays responsive while the emulator runs.
// Progress/cancellation ride on an XBinary::PDSTRUCT: the engine reports each info line
// through its callback (which re-emits logLine) and polls the stop flag for cancellation.
class UnpackWorker : public QObject
{
    Q_OBJECT

public:
    explicit UnpackWorker(const EmulatorUnpacker::Options &options, QObject *parent = nullptr);

    const EmulatorUnpacker::Result &result() const { return m_result; }

    // Request the running job to stop; the engine polls the PDSTRUCT stop flag (the same
    // cross-thread stop pattern used throughout the XBinary-based GUIs).
    void requestCancel() { XBinary::setPdStructStopped(&m_pdStruct); }

    // Invoked from the engine's PDSTRUCT callback (on this worker's thread) to stream one line.
    void streamLine(const QString &line) { emit logLine(line); }

public slots:
    void process();

signals:
    void logLine(const QString &line);
    void finished();

private:
    EmulatorUnpacker::Options m_options;
    EmulatorUnpacker::Result m_result;
    XBinary::PDSTRUCT m_pdStruct;
};

#endif  // UNPACKWORKER_H
