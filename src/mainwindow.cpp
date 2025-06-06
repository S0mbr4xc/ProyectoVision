#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>   // cv::VideoWriter
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <cmath>

static cv::Mat anisotropicDiffusion(const cv::Mat &inImage, int niter = 15, float kappa = 30.0f, float lambda = 0.2f)
{
    CV_Assert(inImage.type() == CV_8UC1);
    cv::Mat prev;
    inImage.convertTo(prev, CV_32F);
    int rows = prev.rows, cols = prev.cols;
    cv::Mat diff = cv::Mat::zeros(rows, cols, CV_32F);

    cv::Mat north = cv::Mat::zeros(rows, cols, CV_32F),
            south = north.clone(),
            east  = north.clone(),
            west  = north.clone();

    for (int iter = 0; iter < niter; ++iter) {
        // Gradientes
        for (int i = 1; i < rows; ++i) {
            float* prevRow   = prev.ptr<float>(i);
            float* prevRowUp = prev.ptr<float>(i - 1);
            float* northRow  = north.ptr<float>(i);
            for (int j = 0; j < cols; ++j) {
                northRow[j] = prevRowUp[j] - prevRow[j];
            }
        }
        for (int i = 0; i < rows - 1; ++i) {
            float* prevRow   = prev.ptr<float>(i);
            float* prevRowDn = prev.ptr<float>(i + 1);
            float* southRow  = south.ptr<float>(i);
            for (int j = 0; j < cols; ++j) {
                southRow[j] = prevRowDn[j] - prevRow[j];
            }
        }
        for (int i = 0; i < rows; ++i) {
            float* prevRow = prev.ptr<float>(i);
            float* westRow = west.ptr<float>(i);
            for (int j = 1; j < cols; ++j) {
                westRow[j] = prevRow[j - 1] - prevRow[j];
            }
        }
        for (int i = 0; i < rows; ++i) {
            float* prevRow = prev.ptr<float>(i);
            float* eastRow = east.ptr<float>(i);
            for (int j = 0; j < cols - 1; ++j) {
                eastRow[j] = prevRow[j + 1] - prevRow[j];
            }
        }

        // Conductividad
        cv::Mat cN(rows, cols, CV_32F), cS = cN.clone(), cW = cN.clone(), cE = cN.clone();
        for (int i = 0; i < rows; ++i) {
            float* nRow  = north.ptr<float>(i);
            float* sRow  = south.ptr<float>(i);
            float* wRow  = west.ptr<float>(i);
            float* eRow  = east.ptr<float>(i);
            float* cNRow = cN.ptr<float>(i);
            float* cSRow = cS.ptr<float>(i);
            float* cWRow = cW.ptr<float>(i);
            float* cERow = cE.ptr<float>(i);
            for (int j = 0; j < cols; ++j) {
                float gn = nRow[j], gs = sRow[j], gw = wRow[j], ge = eRow[j];
                cNRow[j] = std::exp(-(gn * gn) / (kappa * kappa));
                cSRow[j] = std::exp(-(gs * gs) / (kappa * kappa));
                cWRow[j] = std::exp(-(gw * gw) / (kappa * kappa));
                cERow[j] = std::exp(-(ge * ge) / (kappa * kappa));
            }
        }

        // Divergencia
        diff.setTo(0);
        for (int i = 0; i < rows; ++i) {
            float* nRow   = north.ptr<float>(i);
            float* sRow   = south.ptr<float>(i);
            float* wRow   = west.ptr<float>(i);
            float* eRow   = east.ptr<float>(i);
            float* cNRow  = cN.ptr<float>(i);
            float* cSRow  = cS.ptr<float>(i);
            float* cWRow  = cW.ptr<float>(i);
            float* cERow  = cE.ptr<float>(i);
            float* diffRow = diff.ptr<float>(i);
            for (int j = 0; j < cols; ++j) {
                diffRow[j] = cNRow[j] * nRow[j]
                           + cSRow[j] * sRow[j]
                           + cWRow[j] * wRow[j]
                           + cERow[j] * eRow[j];
            }
        }

        // Actualizar
        for (int i = 0; i < rows; ++i) {
            float* prevRow = prev.ptr<float>(i);
            float* diffRow = diff.ptr<float>(i);
            for (int j = 0; j < cols; ++j) {
                prevRow[j] = prevRow[j] + lambda * diffRow[j];
            }
        }
    }

    cv::Mat outImage;
    prev.convertTo(outImage, CV_8U);
    return outImage;
}

// ---------------------- Constructor y destructor -----------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      imageProc(nullptr),
      maskProc(nullptr),
      numSlices(0),
      currentSlice(0)
{
    ui->setupUi(this);

    const QString imgDir  = "/home/s0mbr4xc/CLionProjects/ProyectoVision/Brain Tumor Training Set/training_images";
    const QString maskDir = "/home/s0mbr4xc/CLionProjects/ProyectoVision/Brain Tumor Training Set/training_labels";

    if (!QDir(imgDir).exists() || !QDir(maskDir).exists()) {
        QMessageBox::critical(this, "Error",
                              "No se encontraron las carpetas:\n" +
                              imgDir + "\n" + maskDir);
        exit(EXIT_FAILURE);
    }

    setupPatients(imgDir, maskDir);

    connect(ui->patientCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(on_patientCombo_currentIndexChanged(int)));

    connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(on_prevButton_clicked()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(on_nextButton_clicked()));
    connect(ui->exportVideoButton, SIGNAL(clicked()), this, SLOT(exportarVideo()));
    connect(ui->exportStatsButton, SIGNAL(clicked()), this, SLOT(exportarEstadisticas()));

    if (ui->patientCombo->count() > 0) {
        on_patientCombo_currentIndexChanged(0);
    }
}

MainWindow::~MainWindow()
{
    delete imageProc;
    delete maskProc;
    delete ui;
}

// ------------------------ Carga del paciente ---------------------------

void MainWindow::setupPatients(const QString &imagesDir, const QString &masksDir)
{
    QDir dirImg(imagesDir);
    QDir dirMask(masksDir);
    QStringList imgList  = dirImg.entryList({ "*.nii" }, QDir::Files, QDir::Name);
    QStringList maskList = dirMask.entryList({ "*.nii" }, QDir::Files, QDir::Name);

    for (const QString &name : imgList) {
        if (maskList.contains(name)) {
            imageFiles.append(dirImg.filePath(name));
            maskFiles.append(dirMask.filePath(name));
            ui->patientCombo->addItem(QFileInfo(name).baseName());
        }
    }

    if (imageFiles.isEmpty()) {
        QMessageBox::critical(this, "Error",
                              "No se encontraron archivos .nii coincidentes\n"
                              "en:\n" + imagesDir + "\n" + masksDir);
        exit(EXIT_FAILURE);
    }
}

void MainWindow::on_patientCombo_currentIndexChanged(int index)
{
    if (index < 0 || index >= imageFiles.size() || index >= maskFiles.size())
        return;

    delete imageProc;
    delete maskProc;
    imageProc = new Procesar(imageFiles[index].toStdString());
    maskProc  = new Procesar(maskFiles[index].toStdString());
    numSlices = imageProc->obtenerNumSlices();
    currentSlice = 0;
    // Actualiza etiqueta de slice
    ui->sliceLabel->setText(QString("Slice: %1 / %2").arg(currentSlice + 1).arg(numSlices));
    updateDisplay();
}

// ---------------------- Navegación por slices --------------------------

void MainWindow::on_prevButton_clicked()
{
    if (numSlices == 0) return;
    currentSlice = (currentSlice > 0) ? currentSlice - 1 : numSlices - 1;
    ui->sliceLabel->setText(QString("Slice: %1 / %2").arg(currentSlice + 1).arg(numSlices));
    updateDisplay();
}

void MainWindow::on_nextButton_clicked()
{
    if (numSlices == 0) return;
    currentSlice = (currentSlice + 1) % numSlices;
    ui->sliceLabel->setText(QString("Slice: %1 / %2").arg(currentSlice + 1).arg(numSlices));
    updateDisplay();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A) {
        on_prevButton_clicked();
    } else if (event->key() == Qt::Key_D) {
        on_nextButton_clicked();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

// ------------------- Generación de un frame combinado ------------------

cv::Mat MainWindow::createCombinedFrame(int sliceIdx)
{
    // 1) Leer slice en escala de grises
    cv::Mat imgGray  = imageProc->obtenerSlice(sliceIdx);
    cv::Mat maskGray = maskProc->obtenerSlice(sliceIdx);

    // 2) Suavizado o difusión anisotrópica
    cv::Mat imgPre;
    if (ui->checkAniso->isChecked()) {
        imgPre = anisotropicDiffusion(imgGray, 15, 30.0f, 0.2f);
    }
    else if (ui->checkSmooth->isChecked()) {
        cv::GaussianBlur(imgGray, imgPre, cv::Size(5, 5), 0);
    }
    else {
        imgPre = imgGray.clone();
    }

    // 3) Ecualización de histograma
    cv::Mat imgEq;
    if (ui->checkEqualize->isChecked()) {
        cv::equalizeHist(imgPre, imgEq);
    } else {
        imgEq = imgPre;
    }

    // 4) CLAHE
    cv::Mat imgCLAHE;
    if (ui->checkCLAHE->isChecked()) {
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(2.0);
        clahe->apply(imgEq, imgCLAHE);
    } else {
        imgCLAHE = imgEq;
    }

    // 5) Binarización de la máscara
    cv::Mat maskBin;
    if (ui->checkThreshold->isChecked()) {
        cv::threshold(maskGray, maskBin, 1, 255, cv::THRESH_BINARY);
    } else {
        maskBin = maskGray;
    }

    // 6) Operaciones morfológicas para limpiar la máscara
    cv::Mat maskClean;
    if (ui->checkMorpho->isChecked()) {
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::morphologyEx(maskBin, maskClean, cv::MORPH_OPEN,  kernel);
        cv::morphologyEx(maskClean, maskClean, cv::MORPH_CLOSE, kernel);
    } else {
        maskClean = maskBin;
    }

    // 7) Detección de bordes (Canny)
    cv::Mat edges;
    if (ui->checkCanny->isChecked()) {
        cv::Canny(imgCLAHE, edges, 100, 200);
    }

    // 8) Convertir imgCLAHE a BGR para superponer (BGR: 3 canales)
    cv::Mat imgColor;
    cv::cvtColor(imgCLAHE, imgColor, cv::COLOR_GRAY2BGR);

    // 9) OPERACIONES BITWISE según checkbox correspondiente
    cv::Mat invMask, rojo(imgColor.size(), imgColor.type(), cv::Scalar(0, 0, 255));
    cv::bitwise_not(maskClean, invMask); // invMask = NOT(maskClean)

    cv::Mat imgCombined;
    if (ui->checkNOT->isChecked()) {
        cv::bitwise_not(imgColor, imgCombined);
    }
    else if (ui->checkAND->isChecked()) {
        cv::Mat maskCleanBGR;
        cv::cvtColor(maskClean, maskCleanBGR, cv::COLOR_GRAY2BGR);
        cv::bitwise_and(imgColor, maskCleanBGR, imgCombined);
    }
    else if (ui->checkOR->isChecked()) {
        cv::Mat bg, maskCleanBGR, rojoPart;
        cv::bitwise_and(imgColor, imgColor, bg, invMask);
        cv::cvtColor(maskClean, maskCleanBGR, cv::COLOR_GRAY2BGR);
        cv::bitwise_and(rojo, maskCleanBGR, rojoPart);
        cv::bitwise_or(bg, rojoPart, imgCombined);
    }
    else if (ui->checkXOR->isChecked()) {
        cv::Mat maskCleanBGR, rojoPart;
        cv::cvtColor(maskClean, maskCleanBGR, cv::COLOR_GRAY2BGR);
        cv::bitwise_and(rojo, maskCleanBGR, rojoPart);
        cv::bitwise_xor(imgColor, rojoPart, imgCombined);
    }
    else {
        // Blending semitransparente
        imgCombined = imgColor.clone();
        const float alpha = 0.3f;
        for (int y = 0; y < maskClean.rows; ++y) {
            uchar *maskRow = maskClean.ptr<uchar>(y);
            cv::Vec3b *row  = imgCombined.ptr<cv::Vec3b>(y);
            for (int x = 0; x < maskClean.cols; ++x) {
                if (maskRow[x] > 0) {
                    uchar B = row[x][0], G = row[x][1], R = row[x][2];
                    row[x][0] = static_cast<uchar>((1.0f - alpha) * B + alpha * 0);
                    row[x][1] = static_cast<uchar>((1.0f - alpha) * G + alpha * 0);
                    row[x][2] = static_cast<uchar>((1.0f - alpha) * R + alpha * 255);
                }
            }
        }
    }

    // 10) Superponer bordes en verde (si Canny está activo)
    if (ui->checkCanny->isChecked()) {
        for (int y = 0; y < edges.rows; ++y) {
            uchar *edgeRow = edges.ptr<uchar>(y);
            cv::Vec3b *row = imgCombined.ptr<cv::Vec3b>(y);
            for (int x = 0; x < edges.cols; ++x) {
                if (edgeRow[x] > 0) {
                    row[x] = cv::Vec3b(0, 255, 0);
                }
            }
        }
    }

    // 11) Devolver el frame final en BGR
    return imgCombined;
}

// ---------------------- Mostrar slice actual en la UI ------------------

void MainWindow::updateDisplay()
{
    if (!imageProc) return;

    // Para labelImage (solo hasta CLAHE)
    cv::Mat imgGray     = imageProc->obtenerSlice(currentSlice);
    cv::Mat imgPre;
    if (ui->checkAniso->isChecked()) {
        imgPre = anisotropicDiffusion(imgGray, 15, 30.0f, 0.2f);
    }
    else if (ui->checkSmooth->isChecked()) {
        cv::GaussianBlur(imgGray, imgPre, cv::Size(5, 5), 0);
    }
    else {
        imgPre = imgGray.clone();
    }

    cv::Mat imgEq;
    if (ui->checkEqualize->isChecked()) {
        cv::equalizeHist(imgPre, imgEq);
    } else {
        imgEq = imgPre;
    }

    cv::Mat imgCLAHE;
    if (ui->checkCLAHE->isChecked()) {
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
        clahe->setClipLimit(2.0);
        clahe->apply(imgEq, imgCLAHE);
    } else {
        imgCLAHE = imgEq;
    }

    // Mostrar imagen (CLAHE) en labelImage
    QImage qImgCLAHE = matToQImage(imgCLAHE);
    ui->labelImage->setPixmap(QPixmap::fromImage(qImgCLAHE).scaled(
        ui->labelImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Mostrar máscara procesada en labelMask
    cv::Mat maskGray = maskProc->obtenerSlice(currentSlice);
    cv::Mat maskBin, maskClean;
    cv::threshold(maskGray, maskBin, 1, 255, cv::THRESH_BINARY);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(maskBin, maskClean, cv::MORPH_OPEN,  kernel);
    cv::morphologyEx(maskClean, maskClean, cv::MORPH_CLOSE, kernel);

    QImage qImgMask = matToQImage(maskClean);
    ui->labelMask->setPixmap(QPixmap::fromImage(qImgMask).scaled(
        ui->labelMask->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Mostrar imagen combinada en labelCombined
    cv::Mat combinedBGR = createCombinedFrame(currentSlice);
    QImage qImgCombined = matToQImage(combinedBGR);
    ui->labelCombined->setPixmap(QPixmap::fromImage(qImgCombined).scaled(
        ui->labelCombined->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// ---------------------- Exportar vídeo de todos los slices ------------------

void MainWindow::exportarVideo()
{
    if (!imageProc) return;

    QString outPath = QFileDialog::getSaveFileName(this,
                    "Guardar vídeo de slices", "",
                    "AVI Files (*.avi);;Todos los archivos (*)");
    if (outPath.isEmpty()) return;

    int fps = 5;
    cv::Mat sample = imageProc->obtenerSlice(0);
    int width  = sample.cols;
    int height = sample.rows;

    cv::VideoWriter writer(
        outPath.toStdString(),
        cv::VideoWriter::fourcc('M','J','P','G'),
        fps,
        cv::Size(width, height)
    );
    if (!writer.isOpened()) {
        QMessageBox::critical(this, "Error", "No se pudo crear el archivo de vídeo.");
        return;
    }

    for (int s = 0; s < numSlices; ++s) {
        cv::Mat frameBGR = createCombinedFrame(s);
        if (frameBGR.type() != CV_8UC3) {
            frameBGR.convertTo(frameBGR, CV_8UC3);
        }
        writer.write(frameBGR);
    }

    writer.release();
    QMessageBox::information(this, "Éxito", "Vídeo exportado correctamente:\n" + outPath);
}

// -------------------- Exportar estadísticas llamando a script Python ----

void MainWindow::exportarEstadisticas()
{
    // Asumimos que el programa “ggraficas.py” está en el mismo directorio
    QString scriptPath = QCoreApplication::applicationDirPath()
                         + "/ggraficas.py";
    if (!QFile::exists(scriptPath)) {
        QMessageBox::critical(this, "Error", "No se encontró ggraficas.py en el directorio actual.");
        return;
    }

    // Ejecutar Python 3.12 con el script
    QString program = "python3.12";
    QStringList args;
    args << scriptPath;

    QProcess *process = new QProcess(this);
    process->start(program, args);
    if (!process->waitForStarted()) {
        QMessageBox::critical(this, "Error", "No se pudo iniciar el proceso de Python.");
        delete process;
        return;
    }

    if (!process->waitForFinished()) {
        QMessageBox::critical(this, "Error", "El script Python no finalizó correctamente.");
        delete process;
        return;
    }

    QString output = process->readAllStandardOutput();
    QString error  = process->readAllStandardError();
    delete process;

    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Python Error", error);
    } else {
        QMessageBox::information(this, "Estadísticas Generadas", output);
    }
}

// -------------------- Conversión cv::Mat → QImage -----------------------

QImage MainWindow::matToQImage(const cv::Mat &mat)
{
    if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows,
                      static_cast<int>(mat.step),
                      QImage::Format_Grayscale8).copy();
    }
    else if (mat.type() == CV_8UC3) {
        return QImage(mat.data, mat.cols, mat.rows,
                      static_cast<int>(mat.step),
                      QImage::Format_BGR888).copy();
    }
    return QImage();
}
