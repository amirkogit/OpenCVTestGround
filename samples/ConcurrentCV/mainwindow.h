#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfoList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_loopBtn_pressed();

    void on_concurrentBtn_pressed();

private:
    QFileInfoList getImagesInFolder();

private:
    Ui::MainWindow *ui;
};

void addDateTime(QFileInfo &info);

#endif // MAINWINDOW_H
