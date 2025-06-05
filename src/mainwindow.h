#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QDir>
#include <QStringList>
#include "Procesar.h"
#include <opencv2/opencv.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_prevButton_clicked();
    void on_nextButton_clicked();
    void on_patientCombo_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    Procesar *imageProc = nullptr;
    Procesar *maskProc  = nullptr;
    QStringList imageFiles;
    QStringList maskFiles;
    int numSlices = 0;
    int currentSlice = 0;

    void setupPatients(const QString &imagesDir, const QString &masksDir);
    void updateDisplay();
    QImage matToQImage(const cv::Mat &mat);
};

#endif // MAINWINDOW_H
