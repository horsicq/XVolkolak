/* Copyright (c) 2026 hors<horsicq@gmail.com>
 *
 * MIT License
 */
#ifndef DIALOGOPTIONS_H
#define DIALOGOPTIONS_H

#include "../global.h"
#include "xscanengineoptionswidget.h"
#include "xshortcutsdialog.h"

namespace Ui {
class DialogOptions;
}

class DialogOptions : public XShortcutsDialog
{
    Q_OBJECT

public:
    explicit DialogOptions(QWidget *parent, XOptions *options, XOptions::GROUPID groupId);
    ~DialogOptions() override;

    void setGlobal(XShortcuts *shortcuts, XOptions *options);
    void adjustView() override;

protected:
    void registerShortcuts(bool state) override;

private:
    Ui::DialogOptions *ui;
    XScanEngineOptionsWidget *m_scanEngineOptionsWidget;
};

#endif  // DIALOGOPTIONS_H
