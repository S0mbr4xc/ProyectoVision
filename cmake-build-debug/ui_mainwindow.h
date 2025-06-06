/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QComboBox *patientCombo;
    QHBoxLayout *horizontalLayoutFilters;
    QGroupBox *filterGroup;
    QVBoxLayout *verticalLayoutFilters;
    QCheckBox *checkAniso;
    QCheckBox *checkSmooth;
    QCheckBox *checkEqualize;
    QCheckBox *checkCLAHE;
    QCheckBox *checkThreshold;
    QCheckBox *checkMorpho;
    QCheckBox *checkCanny;
    QHBoxLayout *horizontalLayoutImages;
    QLabel *labelImage;
    QLabel *labelMask;
    QLabel *labelCombined;
    QToolBar *toolBar;
    QPushButton *prevButton;
    QPushButton *nextButton;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1400, 700);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        patientCombo = new QComboBox(centralwidget);
        patientCombo->setObjectName(QString::fromUtf8("patientCombo"));

        verticalLayout->addWidget(patientCombo);

        horizontalLayoutFilters = new QHBoxLayout();
        horizontalLayoutFilters->setObjectName(QString::fromUtf8("horizontalLayoutFilters"));
        filterGroup = new QGroupBox(centralwidget);
        filterGroup->setObjectName(QString::fromUtf8("filterGroup"));
        verticalLayoutFilters = new QVBoxLayout(filterGroup);
        verticalLayoutFilters->setObjectName(QString::fromUtf8("verticalLayoutFilters"));
        checkAniso = new QCheckBox(filterGroup);
        checkAniso->setObjectName(QString::fromUtf8("checkAniso"));

        verticalLayoutFilters->addWidget(checkAniso);

        checkSmooth = new QCheckBox(filterGroup);
        checkSmooth->setObjectName(QString::fromUtf8("checkSmooth"));

        verticalLayoutFilters->addWidget(checkSmooth);

        checkEqualize = new QCheckBox(filterGroup);
        checkEqualize->setObjectName(QString::fromUtf8("checkEqualize"));

        verticalLayoutFilters->addWidget(checkEqualize);

        checkCLAHE = new QCheckBox(filterGroup);
        checkCLAHE->setObjectName(QString::fromUtf8("checkCLAHE"));

        verticalLayoutFilters->addWidget(checkCLAHE);

        checkThreshold = new QCheckBox(filterGroup);
        checkThreshold->setObjectName(QString::fromUtf8("checkThreshold"));

        verticalLayoutFilters->addWidget(checkThreshold);

        checkMorpho = new QCheckBox(filterGroup);
        checkMorpho->setObjectName(QString::fromUtf8("checkMorpho"));

        verticalLayoutFilters->addWidget(checkMorpho);

        checkCanny = new QCheckBox(filterGroup);
        checkCanny->setObjectName(QString::fromUtf8("checkCanny"));

        verticalLayoutFilters->addWidget(checkCanny);


        horizontalLayoutFilters->addWidget(filterGroup);

        horizontalLayoutImages = new QHBoxLayout();
        horizontalLayoutImages->setObjectName(QString::fromUtf8("horizontalLayoutImages"));
        labelImage = new QLabel(centralwidget);
        labelImage->setObjectName(QString::fromUtf8("labelImage"));
        labelImage->setMinimumSize(QSize(400, 400));
        labelImage->setAlignment(Qt::AlignCenter);

        horizontalLayoutImages->addWidget(labelImage);

        labelMask = new QLabel(centralwidget);
        labelMask->setObjectName(QString::fromUtf8("labelMask"));
        labelMask->setMinimumSize(QSize(400, 400));
        labelMask->setAlignment(Qt::AlignCenter);

        horizontalLayoutImages->addWidget(labelMask);

        labelCombined = new QLabel(centralwidget);
        labelCombined->setObjectName(QString::fromUtf8("labelCombined"));
        labelCombined->setMinimumSize(QSize(400, 400));
        labelCombined->setAlignment(Qt::AlignCenter);

        horizontalLayoutImages->addWidget(labelCombined);


        horizontalLayoutFilters->addLayout(horizontalLayoutImages);


        verticalLayout->addLayout(horizontalLayoutFilters);

        MainWindow->setCentralWidget(centralwidget);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        prevButton = new QPushButton(toolBar);
        prevButton->setObjectName(QString::fromUtf8("prevButton"));
        toolBar->addWidget(prevButton);
        nextButton = new QPushButton(toolBar);
        nextButton->setObjectName(QString::fromUtf8("nextButton"));
        toolBar->addWidget(nextButton);
        MainWindow->addToolBar(toolBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        filterGroup->setTitle(QCoreApplication::translate("MainWindow", "Filtros", nullptr));
        checkAniso->setText(QCoreApplication::translate("MainWindow", "Difusi\303\263n Anisotr\303\263pica", nullptr));
        checkSmooth->setText(QCoreApplication::translate("MainWindow", "Suavizado (Gaussian)", nullptr));
        checkEqualize->setText(QCoreApplication::translate("MainWindow", "Ecualizaci\303\263n Histograma", nullptr));
        checkCLAHE->setText(QCoreApplication::translate("MainWindow", "CLAHE", nullptr));
        checkThreshold->setText(QCoreApplication::translate("MainWindow", "Binarizaci\303\263n", nullptr));
        checkMorpho->setText(QCoreApplication::translate("MainWindow", "Operaciones Morfol\303\263gicas", nullptr));
        checkCanny->setText(QCoreApplication::translate("MainWindow", "Detecci\303\263n de Bordes (Canny)", nullptr));
        prevButton->setText(QCoreApplication::translate("MainWindow", "< Prev", nullptr));
        nextButton->setText(QCoreApplication::translate("MainWindow", "Next >", nullptr));
        (void)MainWindow;
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
