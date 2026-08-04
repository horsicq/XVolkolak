#include "unpackworker.h"

namespace {

// PDSTRUCT callback fired by the engine (on the worker thread) for each info line: forward the
// current info string to the worker, which re-emits it as a queued logLine signal.
void workerLogCallback(void *pUserData, XBinary::PDSTRUCT *pPdStruct)
{
    static_cast<UnpackWorker *>(pUserData)->streamLine(pPdStruct->sInfoString);
}

}  // namespace

UnpackWorker::UnpackWorker(const EmulatorUnpacker::Options &options, QObject *parent)
    : QObject(parent)
    , m_options(options)
{
    // Route the engine's live log through this worker's callback (streams as queued signals).
    m_pdStruct = XBinary::createPdStruct();
    m_pdStruct.pCallback = workerLogCallback;
    m_pdStruct.pCallbackUserData = this;
}

void UnpackWorker::process()
{
    m_result = EmulatorUnpacker::unpack(m_options, &m_pdStruct);
    emit finished();
}
