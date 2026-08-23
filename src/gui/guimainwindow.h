#ifndef GUIMAINWINDOW_H
#define GUIMAINWINDOW_H

#include <QMainWindow>

#include "emulatorunpacker.h"
#include "xoptions.h"
#include "xshortcuts.h"

namespace Ui {
class GuiMainWindow;
}

class GuiMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GuiMainWindow(QWidget *parent = nullptr);
    ~GuiMainWindow() override;

private slots:
    void scanInput(const QString &fileName);
    void browseInputFile();
    void showUnpackDialog();
    void showOptionsDialog();
    void showAboutDialog();
    void updateActions();

private:
    void adjustView();
    Ui::GuiMainWindow *ui;
    EmulatorUnpacker::Options m_unpackOptions;
    XOptions m_xOptions;        // view/settings context the scan widget needs
    XShortcuts m_xShortcuts;    // keyboard-shortcut context for the scan widget
};

#endif  // GUIMAINWINDOW_H
