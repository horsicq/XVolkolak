#include "dialogabout.h"
#include "ui_dialogabout.h"

#include "../global.h"

DialogAbout::DialogAbout(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogAbout)
{
    ui->setupUi(this);

    setWindowTitle(tr("About %1").arg(X_APPLICATIONDISPLAYNAME));
    ui->labelTitle->setText(QStringLiteral(X_APPLICATIONDISPLAYNAME));
    ui->labelDescription->setText(tr("Emulator-unpacker with console and Qt GUI front ends."));
    ui->labelVersion->setText(tr("Version %1").arg(X_APPLICATIONVERSION));
    ui->labelCopyright->setText(tr("(C) 2026 XVolkolak contributors"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DialogAbout::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &DialogAbout::reject);
}

DialogAbout::~DialogAbout()
{
    delete ui;
}
