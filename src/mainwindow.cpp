#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    const QString imgDir  = "/home/s0mbr4xc/CLionProjects/ProyectoVision/Brain Tumor Training Set/training_images";
    const QString maskDir = "/home/s0mbr4xc/CLionProjects/ProyectoVision/Brain Tumor Training Set/training_labels";

    if (imgDir.isEmpty() || maskDir.isEmpty()) {
        QMessageBox::critical(this, "Error", "Debe seleccionar ambas carpetas.");
        exit(EXIT_FAILURE);
    }

    setupPatients(imgDir, maskDir);
    connect(ui->patientCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(on_patientCombo_currentIndexChanged(int)));
    if (ui->patientCombo->count() > 0) {
        on_patientCombo_currentIndexChanged(0);
    }
}

MainWindow::~MainWindow() {
    delete imageProc;
    delete maskProc;
    delete ui;
}

void MainWindow::setupPatients(const QString &imagesDir, const QString &masksDir) {
    QDir dirImg(imagesDir);
    QDir dirMask(masksDir);
    QStringList imgList = dirImg.entryList({"*.nii"}, QDir::Files, QDir::Name);
    QStringList maskList = dirMask.entryList({"*.nii"}, QDir::Files, QDir::Name);
    // Intersect
    for (const QString &name : imgList) {
        if (maskList.contains(name)) {
            imageFiles.append(dirImg.filePath(name));
            maskFiles.append(dirMask.filePath(name));
            ui->patientCombo->addItem(QFileInfo(name).baseName());
        }
    }
}

void MainWindow::on_patientCombo_currentIndexChanged(int index) {
    if (index < 0 || index >= imageFiles.size() || index >= maskFiles.size()) return;
    delete imageProc;
    delete maskProc;
    imageProc = new Procesar(imageFiles[index].toStdString());
    maskProc  = new Procesar(maskFiles[index].toStdString());
    numSlices = imageProc->obtenerNumSlices();
    currentSlice = 0;
    updateDisplay();
}

void MainWindow::updateDisplay() {
    cv::Mat img  = imageProc->obtenerSlice(currentSlice);
    cv::Mat mask = maskProc->obtenerSlice(currentSlice);
    ui->labelImage   ->setPixmap(QPixmap::fromImage(matToQImage(img)));
    ui->labelMask    ->setPixmap(QPixmap::fromImage(matToQImage(mask)));
    cv::Mat combo;
    cv::cvtColor(img, combo, cv::COLOR_GRAY2BGR);
    for (int y = 0; y < mask.rows; ++y)
        for (int x = 0; x < mask.cols; ++x)
            if (mask.at<uchar>(y, x) > 0)
                combo.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 255);
    ui->labelCombined->setPixmap(QPixmap::fromImage(matToQImage(combo)));
}

void MainWindow::on_prevButton_clicked() {
    currentSlice = (currentSlice > 0) ? currentSlice - 1 : numSlices - 1;
    updateDisplay();
}

void MainWindow::on_nextButton_clicked() {
    currentSlice = (currentSlice + 1) % numSlices;
    updateDisplay();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_A) {
        on_prevButton_clicked();
    } else if (event->key() == Qt::Key_D) {
        on_nextButton_clicked();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

QImage MainWindow::matToQImage(const cv::Mat &mat) {
    if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, int(mat.step), QImage::Format_Grayscale8).copy();
    } else if (mat.type() == CV_8UC3) {
        return QImage(mat.data, mat.cols, mat.rows, int(mat.step), QImage::Format_BGR888).copy();
    }
    return QImage();
}
