#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QKeyEvent>
#include "Procesar.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_patientCombo_currentIndexChanged(int index);
    void on_prevButton_clicked();
    void on_nextButton_clicked();
    void exportarVideo();
    void exportarEstadisticas();

private:
    Ui::MainWindow *ui;
    Procesar *imageProc;
    Procesar *maskProc;
    QStringList imageFiles;
    QStringList maskFiles;
    int numSlices;
    int currentSlice;

    void setupPatients(const QString &imagesDir, const QString &masksDir);
    void updateDisplay();
    cv::Mat createCombinedFrame(int sliceIdx);
    QImage matToQImage(const cv::Mat &mat);
};

#endif // MAINWINDOW_H
