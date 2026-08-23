/* Copyright (c) 2026 hors<horsicq@gmail.com>
 *
 * MIT License
 */
#include "dialogoptions.h"
#include "ui_dialogoptions.h"

DialogOptions::DialogOptions(QWidget *parent, XOptions *options, XOptions::GROUPID groupId)
    : XShortcutsDialog(parent, false)
    , ui(new Ui::DialogOptions)
    , m_scanEngineOptionsWidget(new XScanEngineOptionsWidget(this))
{
    ui->setupUi(this);

    m_scanEngineOptionsWidget->setProperty("GROUPID", XOptions::GROUPID_SCAN);
    ui->widgetOptions->setOptions(options, X_APPLICATIONDISPLAYNAME);
    ui->widgetOptions->addPage(m_scanEngineOptionsWidget, tr("Scan"));
    m_scanEngineOptionsWidget->setOptions(options);
    ui->widgetOptions->setCurrentPage(groupId);
}

DialogOptions::~DialogOptions()
{
    delete ui;
}

void DialogOptions::setGlobal(XShortcuts *shortcuts, XOptions *options)
{
    ui->widgetOptions->setGlobal(shortcuts, options);
    XShortcutsDialog::setGlobal(shortcuts, options);
}

void DialogOptions::adjustView()
{
    ui->widgetOptions->adjustView();
}

void DialogOptions::registerShortcuts(bool state)
{
    Q_UNUSED(state)
}
