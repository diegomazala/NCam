/********************************************************************************
** Form generated from reading UI file 'LensWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.4.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LENSWIDGET_H
#define UI_LENSWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LensWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_8;
    QGroupBox *deviationGroupBox;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *zoomLabel;
    QDoubleSpinBox *zoomSpinBox;
    QHBoxLayout *horizontalLayout_2;
    QLabel *focusLabel;
    QDoubleSpinBox *focusSpinBox;
    QGroupBox *encodeFeedingGroupBox;
    QHBoxLayout *horizontalLayout_7;
    QHBoxLayout *horizontalLayout_4;
    QLabel *zoomEncoderLabel;
    QLineEdit *zoomEncoderLineEdit;
    QHBoxLayout *horizontalLayout_5;
    QLabel *focusEncoderLabel;
    QLineEdit *focusEncoderLineEdit;
    QHBoxLayout *horizontalLayout_6;
    QLabel *fovEncoderLabel;
    QLineEdit *fovEncoderLineEdit;
    QTableWidget *tableWidget;

    void setupUi(QWidget *LensWidget)
    {
        if (LensWidget->objectName().isEmpty())
            LensWidget->setObjectName(QStringLiteral("LensWidget"));
        LensWidget->resize(506, 380);
        LensWidget->setMinimumSize(QSize(506, 380));
#ifndef QT_NO_TOOLTIP
        LensWidget->setToolTip(QStringLiteral(""));
#endif // QT_NO_TOOLTIP
        LensWidget->setToolTipDuration(-1);
        verticalLayout = new QVBoxLayout(LensWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        deviationGroupBox = new QGroupBox(LensWidget);
        deviationGroupBox->setObjectName(QStringLiteral("deviationGroupBox"));
        horizontalLayout_3 = new QHBoxLayout(deviationGroupBox);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        zoomLabel = new QLabel(deviationGroupBox);
        zoomLabel->setObjectName(QStringLiteral("zoomLabel"));

        horizontalLayout->addWidget(zoomLabel);

        zoomSpinBox = new QDoubleSpinBox(deviationGroupBox);
        zoomSpinBox->setObjectName(QStringLiteral("zoomSpinBox"));
        zoomSpinBox->setDecimals(3);
        zoomSpinBox->setMaximum(1);
        zoomSpinBox->setSingleStep(0.01);
        zoomSpinBox->setValue(0.05);

        horizontalLayout->addWidget(zoomSpinBox);


        horizontalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        focusLabel = new QLabel(deviationGroupBox);
        focusLabel->setObjectName(QStringLiteral("focusLabel"));

        horizontalLayout_2->addWidget(focusLabel);

        focusSpinBox = new QDoubleSpinBox(deviationGroupBox);
        focusSpinBox->setObjectName(QStringLiteral("focusSpinBox"));
        focusSpinBox->setDecimals(3);
        focusSpinBox->setMaximum(1);
        focusSpinBox->setSingleStep(0.01);
        focusSpinBox->setValue(0.05);

        horizontalLayout_2->addWidget(focusSpinBox);


        horizontalLayout_3->addLayout(horizontalLayout_2);

        focusSpinBox->raise();
        focusSpinBox->raise();
        zoomSpinBox->raise();
        zoomLabel->raise();
        focusLabel->raise();

        horizontalLayout_8->addWidget(deviationGroupBox);

        encodeFeedingGroupBox = new QGroupBox(LensWidget);
        encodeFeedingGroupBox->setObjectName(QStringLiteral("encodeFeedingGroupBox"));
        encodeFeedingGroupBox->setCheckable(true);
        encodeFeedingGroupBox->setChecked(false);
        horizontalLayout_7 = new QHBoxLayout(encodeFeedingGroupBox);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        zoomEncoderLabel = new QLabel(encodeFeedingGroupBox);
        zoomEncoderLabel->setObjectName(QStringLiteral("zoomEncoderLabel"));

        horizontalLayout_4->addWidget(zoomEncoderLabel);

        zoomEncoderLineEdit = new QLineEdit(encodeFeedingGroupBox);
        zoomEncoderLineEdit->setObjectName(QStringLiteral("zoomEncoderLineEdit"));
        zoomEncoderLineEdit->setReadOnly(true);

        horizontalLayout_4->addWidget(zoomEncoderLineEdit);


        horizontalLayout_7->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        focusEncoderLabel = new QLabel(encodeFeedingGroupBox);
        focusEncoderLabel->setObjectName(QStringLiteral("focusEncoderLabel"));

        horizontalLayout_5->addWidget(focusEncoderLabel);

        focusEncoderLineEdit = new QLineEdit(encodeFeedingGroupBox);
        focusEncoderLineEdit->setObjectName(QStringLiteral("focusEncoderLineEdit"));
        focusEncoderLineEdit->setReadOnly(true);

        horizontalLayout_5->addWidget(focusEncoderLineEdit);


        horizontalLayout_7->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        fovEncoderLabel = new QLabel(encodeFeedingGroupBox);
        fovEncoderLabel->setObjectName(QStringLiteral("fovEncoderLabel"));

        horizontalLayout_6->addWidget(fovEncoderLabel);

        fovEncoderLineEdit = new QLineEdit(encodeFeedingGroupBox);
        fovEncoderLineEdit->setObjectName(QStringLiteral("fovEncoderLineEdit"));
        fovEncoderLineEdit->setReadOnly(true);

        horizontalLayout_6->addWidget(fovEncoderLineEdit);


        horizontalLayout_7->addLayout(horizontalLayout_6);


        horizontalLayout_8->addWidget(encodeFeedingGroupBox);


        verticalLayout->addLayout(horizontalLayout_8);

        tableWidget = new QTableWidget(LensWidget);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
#ifndef QT_NO_TOOLTIP
        tableWidget->setToolTip(QStringLiteral(""));
#endif // QT_NO_TOOLTIP
        tableWidget->setRowCount(0);

        verticalLayout->addWidget(tableWidget);


        retranslateUi(LensWidget);
        QObject::connect(tableWidget, SIGNAL(cellChanged(int,int)), LensWidget, SLOT(cellChanged(int,int)));
        QObject::connect(zoomSpinBox, SIGNAL(valueChanged(double)), LensWidget, SLOT(zoomDeviationChanged(double)));
        QObject::connect(focusSpinBox, SIGNAL(valueChanged(double)), LensWidget, SLOT(focusDeviationChanged(double)));
        QObject::connect(encodeFeedingGroupBox, SIGNAL(toggled(bool)), LensWidget, SLOT(onEncodeFeedingToggled(bool)));
        QObject::connect(tableWidget, SIGNAL(cellDoubleClicked(int,int)), LensWidget, SLOT(cellClicked(int,int)));

        QMetaObject::connectSlotsByName(LensWidget);
    } // setupUi

    void retranslateUi(QWidget *LensWidget)
    {
        LensWidget->setWindowTitle(QApplication::translate("LensWidget", "Lens Widget", 0));
        deviationGroupBox->setTitle(QApplication::translate("LensWidget", "Deviation", 0));
        zoomLabel->setText(QApplication::translate("LensWidget", "Zoom", 0));
        focusLabel->setText(QApplication::translate("LensWidget", "Focus", 0));
        focusSpinBox->setPrefix(QString());
        encodeFeedingGroupBox->setTitle(QApplication::translate("LensWidget", "Encode Feeding", 0));
        zoomEncoderLabel->setText(QApplication::translate("LensWidget", "Zoom", 0));
        focusEncoderLabel->setText(QApplication::translate("LensWidget", "Focus", 0));
        fovEncoderLabel->setText(QApplication::translate("LensWidget", "Fov", 0));
    } // retranslateUi

};

namespace Ui {
    class LensWidget: public Ui_LensWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LENSWIDGET_H
