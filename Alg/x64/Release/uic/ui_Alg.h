/********************************************************************************
** Form generated from reading UI file 'Alg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALG_H
#define UI_ALG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AlgClass
{
public:

    void setupUi(QWidget *AlgClass)
    {
        if (AlgClass->objectName().isEmpty())
            AlgClass->setObjectName(QString::fromUtf8("AlgClass"));
        AlgClass->resize(600, 400);

        retranslateUi(AlgClass);

        QMetaObject::connectSlotsByName(AlgClass);
    } // setupUi

    void retranslateUi(QWidget *AlgClass)
    {
        AlgClass->setWindowTitle(QCoreApplication::translate("AlgClass", "Alg", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AlgClass: public Ui_AlgClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALG_H
