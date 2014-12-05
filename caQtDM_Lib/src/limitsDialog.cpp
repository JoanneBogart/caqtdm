/*
 *  This file is part of the caQtDM Framework, developed at the Paul Scherrer Institut,
 *  Villigen, Switzerland
 *
 *  The caQtDM Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The caQtDM Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the caQtDM Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2010 - 2014
 *
 *  Author:
 *    Anton Mezger
 *  Contact details:
 *    anton.mezger@psi.ch
 */

#include <QtGui>
#include "limitsDialog.h"


int limitsDialog::extractPrecisionFromFormat(const QString &fmt)
{
    int prec = -1;
    bool ok = false;

    QString format = fmt.trimmed();
    if(format.right(1).contains("d)")) return 0;   // d format
    if(!format.left(1).contains("%")) return -1;    // not a valid format

    if(format.right(1).contains("f")) {            // f format
        int len = format.size();
        if(len-3 > 0) {
            QString rightPart = format.mid(len-3, len);
            if(rightPart.left(1).contains(".")) {
                QString precision = rightPart.mid(1,1);
                prec = precision.toInt(&ok);
                if(ok) return prec;
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    }
    return -1;
}


QString limitsDialog::getFormatFromPrecision(int prec)
{
    QString format = "\%.%1f";
    format = format.arg(prec);
    return format;
}


limitsDialog::limitsDialog(QWidget *w, MutexKnobData *data, const QString &title, QWidget *parent) : QWidget(parent)
{
    int thisWidth = 650;
    int thisHeight = 150;

    thisWidget = w;
    thisParent = parent;

    monData = data;

    QGridLayout *Layout = new QGridLayout;
    setWindowModality (Qt::WindowModal);

#ifdef Q_OS_IOS
    QPalette palette;
    palette.setBrush(QPalette::Background, QColor(255,255,224,255));
    setPalette(palette);
    setAutoFillBackground(true);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter, QSize(thisWidth,thisHeight), qApp->desktop()->availableGeometry()));
#else
    Qt::WindowFlags flags = Qt::Dialog;
    setWindowFlags(flags);
    move(parent->x() + parent->width() / 2 - thisWidth / 2 , parent->y() + parent->height() /2 -thisHeight/2);
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout;
    QGroupBox *groupBox = new QGroupBox("Limits and precision change");

    // treat limits
    QLabel *limitsLabel = new QLabel("Limits ");

    // add combobox and set correct item
    limitsComboBox = new QComboBox();
    limitsComboBox->addItem("channel");
    limitsComboBox->addItem("user");

    minimumLineEdit = new QLineEdit("");
    maximumLineEdit = new QLineEdit("");

    // add all to layout
    int row = 1;
    Layout->addWidget(limitsLabel, row, 0);
    Layout->addWidget(limitsComboBox, row, 1);
    Layout->addWidget(minimumLineEdit, row, 2);
    Layout->addWidget(maximumLineEdit, row, 3);

    // precision
    QLabel *precisionLabel = new QLabel("Precision ");

    // add combobox and set correct item
    precisionComboBox = new QComboBox();
    precisionComboBox->addItem("channel");
    precisionComboBox->addItem("user");

    precisionLineEdit = new QSpinBox();
    precisionLineEdit->setMinimum(0);
    precisionLineEdit->setMaximum(17);
    precisionLineEdit->setSingleStep(1);

    // add all to layout
    row++;
    Layout->addWidget(precisionLabel,    row, 0);
    Layout->addWidget(precisionComboBox, row, 1);
    Layout->addWidget(precisionLineEdit, row, 2);

    // box with buttons
    buttonBox = new QDialogButtonBox( Qt::Horizontal );
    QPushButton *button = new QPushButton( "Return" );
    connect( button, SIGNAL(clicked()), this, SLOT(cancelClicked()) );
    buttonBox->addButton(button, QDialogButtonBox::RejectRole );

    button = new QPushButton( "Apply" );
    connect( button, SIGNAL(clicked()), this, SLOT(applyClicked()) );
    buttonBox->addButton(button, QDialogButtonBox::ApplyRole );


    QString className = w->metaObject()->className();
    if(className.contains("caNumeric") || className.contains("caApplyNumeric") || className.contains("caSpinbox")) {
         Layout->addWidget(buttonBox, 5, 0);
    } else {
         Layout->addWidget(buttonBox, 3, 0);
    }

    groupBox->setLayout(Layout);
    mainLayout->addWidget(groupBox);

    setLayout(mainLayout);

    if(caSlider* widget = qobject_cast<caSlider *>(w)) {
        thisPV = widget->getPV();
    } else if(caLineEdit* widget  = qobject_cast<caLineEdit *>(w)) {
        thisPV = widget->getPV();
    } else if(caTextEntry* widget  = qobject_cast<caTextEntry *>(w)) {
        thisPV = widget->getPV();
    } else if(caThermo* widget  = qobject_cast<caThermo *>(w)) {
        thisPV = widget->getPV();
    } else if(caNumeric* widget = qobject_cast<caNumeric *>(w)) {
        thisPV = widget->getPV();
    } else if(caApplyNumeric* widget = qobject_cast<caApplyNumeric *>(w)) {
        thisPV = widget->getPV();
    } else if(caSpinbox* widget = qobject_cast<caSpinbox *>(w)) {
        thisPV = widget->getPV();
    } else if(caLinearGauge* widget = qobject_cast<caLinearGauge *>(w)) {
        thisPV = widget->getPV();
    } else if(caCircularGauge* widget = qobject_cast<caCircularGauge *>(w)) {
        thisPV = widget->getPV();
    }

    if(className.contains("Gauge")) {
        QLabel *Title = new QLabel(thisPV + " / " + w->objectName() + " (warning/alarm limits can not yet be changed)");
        Layout->addWidget(Title,0,0);
    } else {
        QLabel *Title = new QLabel(thisPV + " / " + w->objectName());
        Layout->addWidget(Title,0,0);
    }

    // get channel limits if needed later
    knobData *kPtr = monData->getMutexKnobDataPV(w, thisPV);
    if(kPtr != (knobData*) 0) {
        channelLowerLimit = kPtr->edata.lower_disp_limit;
        channelUpperLimit = kPtr->edata.upper_disp_limit;
        channelPrecision = kPtr->edata.precision;
        if(kPtr->edata.lower_disp_limit != kPtr->edata.upper_disp_limit) doNothing = false; else doNothing = true;
    } else {
        doNothing = true;
    }

    // fill fields
    if(caSlider* widget = qobject_cast<caSlider *>(w)) {
        caSlider::SourceMode mode = widget->getLimitsMode();
        if(mode == caSlider::Channel) limitsComboBox->setCurrentIndex(0); else limitsComboBox->setCurrentIndex(1);
        initMin = widget->getMinValue();
        initMax = widget->getMaxValue();
        minimumLineEdit->setText(QString::number(initMin, 'g'));
        maximumLineEdit->setText(QString::number(initMax, 'g'));
        precisionComboBox->setDisabled(true);
        precisionLineEdit->setDisabled(true);

    } else if(EAbstractGauge* widget = qobject_cast<EAbstractGauge *>(w)) {
            EAbstractGauge::displayLims mode = widget->getDisplayLimits();
            if(mode == EAbstractGauge::Channel_Limits) limitsComboBox->setCurrentIndex(0); else limitsComboBox->setCurrentIndex(1);
            initMin = widget->minValue();
            initMax = widget->maxValue();
            minimumLineEdit->setText(QString::number(initMin, 'g'));
            maximumLineEdit->setText(QString::number(initMax, 'g'));
            precisionComboBox->setCurrentIndex(1);
            precisionComboBox->setDisabled(true);
            initPrecision = extractPrecisionFromFormat(widget->valueFormat());
            if(initPrecision >=0) precisionLineEdit->setValue(initPrecision);

    } else  if(className.contains("caNumeric") || className.contains("caApplyNumeric")  || className.contains("caSpinbox")) {
        int decDigits=2, intDigits=4;
        bool fixedFormat = false;

        if(caNumeric* widget = qobject_cast<caNumeric *>(w)) {
            if(widget->getLimitsMode()== caNumeric::Channel) limitsComboBox->setCurrentIndex(0); else limitsComboBox->setCurrentIndex(1);
            if(widget->getPrecisionMode() == caNumeric::Channel) {
                initPrecision =  channelPrecision;
                precisionComboBox->setCurrentIndex(0);
            } else {
                initPrecision = widget->decDigits();
                precisionComboBox->setCurrentIndex(1);
            }

            decDigits = widget->decDigits();
            intDigits = widget->intDigits();
            initMin = widget->getMinValue();
            initMax = widget->getMaxValue();
            fixedFormat = widget->getFixedFormat();
        } else if(caApplyNumeric* widget = qobject_cast<caApplyNumeric *>(w)) {
            if(widget->getLimitsMode() == caApplyNumeric::Channel) limitsComboBox->setCurrentIndex(0); else limitsComboBox->setCurrentIndex(1);
            if(widget->getPrecisionMode() == caApplyNumeric::Channel) initPrecision =  channelPrecision; else initPrecision = widget->decDigits();
            decDigits = widget->decDigits();
            intDigits = widget->intDigits();
            initMin = widget->getMinValue();
            initMax = widget->getMaxValue();
            fixedFormat = widget->getFixedFormat();
        } else if(caSpinbox* widget = qobject_cast<caSpinbox *>(w)) {
            if(widget->getLimitsMode() == caSpinbox::Channel) limitsComboBox->setCurrentIndex(0); else limitsComboBox->setCurrentIndex(1);
            if(widget->getPrecisionMode() == caSpinbox::Channel) initPrecision =  channelPrecision; else initPrecision = widget->decDigits();
            decDigits = widget->decDigits();
            intDigits = widget->intDigits();
            initMin = widget->getMinValue();
            initMax = widget->getMaxValue();
            fixedFormat = widget->getFixedFormat();
        }

        minimumLineEdit->setText(QString::number(initMin, 'g'));
        maximumLineEdit->setText(QString::number(initMax, 'g'));
        precisionLineEdit->setValue(initPrecision);

        // special addition for fixed format
        row++;
        QLabel *integerLabel = new QLabel("integer digits");
        integerLineEdit = new QSpinBox();
        integerLineEdit->setMinimum(0);
        integerLineEdit->setMaximum(7);
        integerLineEdit->setSingleStep(1);
        Layout->addWidget(integerLabel, row, 2);
        Layout->addWidget(integerLineEdit, row, 3);
        integerLineEdit->setValue(intDigits);

        QLabel *decimalLabel = new QLabel("decimal digits");
        decimalLineEdit = new QSpinBox();
        decimalLineEdit->setMinimum(0);
        decimalLineEdit->setMaximum(5);
        decimalLineEdit->setSingleStep(1);
        Layout->addWidget(decimalLabel, row+1, 2);
        Layout->addWidget(decimalLineEdit, row+1, 3);
        decimalLineEdit->setValue(decDigits);

        QLabel *formatLabel = new QLabel("Fixed format ");
        formatComboBox = new QComboBox();
        formatComboBox->addItem("no");
        formatComboBox->addItem("yes");
        Layout->addWidget(formatLabel, row, 0);
        Layout->addWidget(formatComboBox, row, 1);
        if(fixedFormat) {
            formatComboBox->setCurrentIndex(1);
            integerLineEdit->setDisabled(false);
            decimalLineEdit->setDisabled(false);
            precisionLineEdit->setDisabled(true);
        } else {
            formatComboBox->setCurrentIndex(0);
            integerLineEdit->setDisabled(true);
            decimalLineEdit->setDisabled(true);
            precisionLineEdit->setDisabled(false);
        }

        connect(formatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));

    } else if(caThermo* widget = qobject_cast<caThermo *>(w)) {
        caThermo::SourceMode mode = widget->getLimitsMode();
        if(mode == caThermo::Channel) limitsComboBox->setCurrentIndex(0); else limitsComboBox->setCurrentIndex(1);
        initMin = widget->minValue();
        initMax = widget->maxValue();
        minimumLineEdit->setText(QString::number(initMin, 'g'));
        maximumLineEdit->setText(QString::number(initMax, 'g'));
        precisionComboBox->setDisabled(true);
        precisionLineEdit->setDisabled(true);

    } else if(caLineEdit* widget  = qobject_cast<caLineEdit *>(w)) {
        caLineEdit::SourceMode mode = widget->getLimitsMode();
        if(mode == caLineEdit::Channel) {
            limitsComboBox->setCurrentIndex(0);
            initMin = channelLowerLimit;
            initMax = channelUpperLimit;
        } else {
            limitsComboBox->setCurrentIndex(1);
            initMin = widget->getMinValue();
            initMax = widget->getMaxValue();
        }
        minimumLineEdit->setText(QString::number(initMin, 'g'));
        maximumLineEdit->setText(QString::number(initMax, 'g'));

        mode = widget->getPrecisionMode();
        if(mode == caLineEdit::Channel) {
            initPrecision =  channelPrecision;
            precisionComboBox->setCurrentIndex(0);
        } else {
            initPrecision = widget->getPrecision();
            precisionComboBox->setCurrentIndex(1);
        }
        precisionLineEdit->setValue(initPrecision);
    }

    setWindowTitle(title);

    showNormal();
}

void limitsDialog::indexChanged(int) {

    if(formatComboBox->currentIndex() == 1) {
        formatComboBox->setCurrentIndex(1);
        integerLineEdit->setDisabled(false);
        decimalLineEdit->setDisabled(false);
        precisionLineEdit->setDisabled(true);
    } else {
        formatComboBox->setCurrentIndex(0);
        integerLineEdit->setDisabled(true);
        decimalLineEdit->setDisabled(true);
        precisionLineEdit->setDisabled(false);
    }
}

void limitsDialog::cancelClicked()
{
    close();
}

void limitsDialog::applyClicked()
{
    bool ok1, ok2, fixedFormat = false;
    QString minimum, maximum;
    double min, max;
    int prec, intDigits=4, decDigits=2;

    int limitsMode = limitsComboBox->currentIndex();
    int precisionMode = precisionComboBox->currentIndex();

    QString className = thisWidget->metaObject()->className();
    if(className.contains("caNumeric") || className.contains("caApplyNumeric") || className.contains("caSpinbox")) {
        if(formatComboBox->currentIndex() == 0) fixedFormat = false; else fixedFormat = true;
        intDigits = integerLineEdit->value();
        decDigits = decimalLineEdit->value();
    }

    minimum = minimumLineEdit->text().trimmed();
    min = minimum.toDouble(&ok1);
    maximum = maximumLineEdit->text().trimmed();
    max = maximum.toDouble(&ok2);
    if(!ok1) min = initMin;
    if(!ok2) max = initMax;
    if(min == max) {
        min = initMin;
        max = initMax;
    }
    prec= precisionLineEdit->value();

    // set already something to the dialog
    if(limitsMode == Channel && !doNothing) {
        minimumLineEdit->setText(QString::number(channelLowerLimit, 'g'));
        maximumLineEdit->setText(QString::number(channelUpperLimit, 'g'));
    } else if(limitsMode == User && ok1 && ok2) {
        minimumLineEdit->setText(QString::number(min, 'g'));
        maximumLineEdit->setText(QString::number(max, 'g'));
    } else if(limitsMode == User) {
        minimumLineEdit->setText(QString::number(initMin, 'g'));
        maximumLineEdit->setText(QString::number(initMax, 'g'));
    }

    if(precisionMode == Channel ) {
        precisionLineEdit->setValue(channelPrecision);
    } else if(precisionMode == User) {
        precisionLineEdit->setValue(prec);
    } else if(precisionMode == User) {
        precisionLineEdit->setValue((int) initPrecision);
    }

    // ************* we have a slider
    if(caSlider* widget = qobject_cast<caSlider *>(thisWidget)) {

        // do this to prevent qwtslider to set the value when changing bounds
        // however we could miss a value coming from the monitor (is this really true?)
        widget->blockSignals(true);
        if(limitsMode == Channel) {
            widget->setLimitsMode(caSlider::Channel);
            if(!doNothing) {
                widget->setMaxValue(channelUpperLimit);
                widget->setMinValue(channelLowerLimit);
            }
        } else if(limitsMode == User){
            widget->setLimitsMode(caSlider::User);
            widget->setMaxValue(max);
            widget->setMinValue(min);
        }
        widget->blockSignals(false);
        // set eventual missed value
        knobData *kPtr = monData->getMutexKnobDataPV(widget, thisPV);
        if(kPtr != (knobData*) 0) widget->setSliderValue(kPtr->edata.rvalue);

        // ************* we have a thermometer
    } else if(caThermo* widget = qobject_cast<caThermo *>(thisWidget)) {

        if(limitsMode == Channel) {
            widget->setLimitsMode(caThermo::Channel);
            if(!doNothing) {
                widget->setMaxValue(channelUpperLimit);
                widget->setMinValue(channelLowerLimit);
            }
        } else if(limitsMode == User){
            widget->setLimitsMode(caThermo::User);
            widget->setMaxValue(max);
            widget->setMinValue(min);
        }

        // ************* we have a calineedit or catextentry
    } else if(caLineEdit* widget = qobject_cast<caLineEdit *>(thisWidget)) {

        if(limitsMode == Channel) {
            widget->setLimitsMode(caLineEdit::Channel);
            if(!doNothing) {
                widget->setMaxValue(channelUpperLimit);
                widget->setMinValue(channelLowerLimit);
            }
        } else if(limitsMode == User){
            widget->setLimitsMode(caLineEdit::User);
            widget->setMaxValue(max);
            widget->setMinValue(min);
        }

        if(precisionMode == Channel) {
            widget->setPrecisionMode(caLineEdit::Channel);
            widget->setFormat(channelPrecision);
        } else if(precisionMode == User){
            widget->setPrecisionMode(caLineEdit::User);
            widget->setPrecision(prec);
            widget->setFormat(prec);
        }
        widget->updateAlarmColors();

        // ************* we have a caNumeric
    } else if(caNumeric* widget = qobject_cast<caNumeric *>(thisWidget)) {

        if(limitsMode == Channel) {
            widget->setLimitsMode(caNumeric::Channel);
            if(!doNothing) {
                widget->setMaxValue(channelUpperLimit);
                widget->setMinValue(channelLowerLimit);
            }
        } else if(limitsMode == User){
            widget->setLimitsMode(caNumeric::User);
            widget->setMaxValue(max);
            widget->setMinValue(min);
        }

        if(fixedFormat) {
           widget->setFixedFormat(true);
           widget->setDecDigits(decDigits);
           widget->setIntDigits(intDigits);
        } else {
           widget->setFixedFormat(false);
           if(precisionMode == Channel) {
               widget->setPrecisionMode(caNumeric::Channel);
               widget->setDecDigits((int) channelPrecision);
           } else if(precisionMode == User){
               widget->setPrecisionMode(caNumeric::User);
               widget->setDecDigits(prec);
           }
        }
        knobData *kPtr = monData->getMutexKnobDataPV(widget, thisPV);
        CaQtDM_Lib *compute = (CaQtDM_Lib *) widget;
        if(kPtr != (knobData*) 0) {
            kPtr->edata.initialize = true;
            compute->ComputeNumericMaxMinPrec(widget, *kPtr);
        }

        // ************* we have a caApplyNumeric, we just cut and paste the above code, not nice, we should redesign the widgets
    } else if(caApplyNumeric* widget = qobject_cast<caApplyNumeric *>(thisWidget)) {

        if(limitsMode == Channel) {
            widget->setLimitsMode(caApplyNumeric::Channel);
            if(!doNothing) {
                widget->setMaxValue(channelUpperLimit);
                widget->setMinValue(channelLowerLimit);
            }
        } else if(limitsMode == User){
            widget->setLimitsMode(caApplyNumeric::User);
            widget->setMaxValue(max);
            widget->setMinValue(min);
        }

        if(fixedFormat) {
           widget->setFixedFormat(true);
           widget->setDecDigits(decDigits);
           widget->setIntDigits(intDigits);
        } else {
           widget->setFixedFormat(false);
           if(precisionMode == Channel) {
               widget->setPrecisionMode(caApplyNumeric::Channel);
               widget->setDecDigits((int) channelPrecision);
           } else if(precisionMode == User){
               widget->setPrecisionMode(caApplyNumeric::User);
               widget->setDecDigits(prec);
           }
        }
        knobData *kPtr = monData->getMutexKnobDataPV(widget, thisPV);
        CaQtDM_Lib *compute = (CaQtDM_Lib *) widget;
        if(kPtr != (knobData*) 0) {
            kPtr->edata.initialize = true;
            compute->ComputeNumericMaxMinPrec(widget, *kPtr);
        }

        // ************* we have a caSpinbox, we just cut and paste the above code, not nice, we should redesign the widgets
    } else if(caSpinbox* widget = qobject_cast<caSpinbox *>(thisWidget)) {

        if(limitsMode == Channel) {
            widget->setLimitsMode(caSpinbox::Channel);
            if(!doNothing) {
                widget->setMaxValue(channelUpperLimit);
                widget->setMinValue(channelLowerLimit);
            }
        } else if(limitsMode == User){
            widget->setLimitsMode(caSpinbox::User);
            widget->setMaxValue(max);
            widget->setMinValue(min);
        }

        if(fixedFormat) {
           widget->setFixedFormat(true);
           widget->setDecDigits(decDigits);
           widget->setIntDigits(intDigits);
        } else {
           widget->setFixedFormat(false);
           if(precisionMode == Channel) {
               widget->setPrecisionMode(caSpinbox::Channel);
               widget->setDecDigits((int) channelPrecision);
           } else if(precisionMode == User){
               widget->setPrecisionMode(caSpinbox::User);
               widget->setDecDigits(prec);
           }
        }
        knobData *kPtr = monData->getMutexKnobDataPV(widget, thisPV);
        CaQtDM_Lib *compute = (CaQtDM_Lib *) widget;
        if(kPtr != (knobData*) 0) {
            kPtr->edata.initialize = true;
            compute->ComputeNumericMaxMinPrec(widget, *kPtr);
        }
    } else if(EAbstractGauge* widget = qobject_cast<EAbstractGauge *>(thisWidget)) {

        if(limitsMode == Channel) {
            widget->setDisplayLimits(EAbstractGauge::Channel_Limits);
        } else if(limitsMode == User){
            widget->setDisplayLimits(EAbstractGauge::User_Limits);
            widget->setMaxValue(max);
            widget->setMinValue(min);
            widget->setLowWarning(min);
            widget->setHighWarning(max);
            widget->setLowError(min);
            widget->setHighError(max);
        }
        knobData *kPtr = monData->getMutexKnobDataPV(widget, thisPV);
        CaQtDM_Lib *compute = (CaQtDM_Lib *) widget;
        if(kPtr != (knobData*) 0) {
            kPtr->edata.initialize = true;
            compute->UpdateGauge(widget, *kPtr);
        }
        widget->setValueFormat(getFormatFromPrecision(prec));

    }
}


void limitsDialog::exec()
{
    connect(buttonBox, SIGNAL(rejected()), &loop, SLOT(quit()) );
    connect(buttonBox, SIGNAL(accepted()), &loop, SLOT(quit()) );
    loop.exec();
    close();
    deleteLater();
}

void limitsDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    loop.quit();
}

void limitsDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QPen pen(Qt::black, 3, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);
    painter.setPen(pen);
    painter.drawRoundedRect(5, 5, width()-7, height()-7, 3, 3);

    QWidget::paintEvent(e);
}


