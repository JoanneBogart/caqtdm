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

#ifndef CATOGGLEBUTTON_H
#define CATOGGLEBUTTON_H

#include <QCheckBox>
#include <QString>
#include <QColor>
#include <QEvent>
#include <qtcontrols_global.h>

class QTCON_EXPORT caToggleButton : public QCheckBox
{
    Q_OBJECT

    Q_ENUMS(colMode)

    Q_PROPERTY(QString channel READ getPV WRITE setPV)

    Q_PROPERTY(QColor foreground READ getForeground WRITE setForeground)
    Q_PROPERTY(QColor background READ getBackground WRITE setBackground)

    Q_PROPERTY(colMode colorMode READ getColorMode WRITE setColorMode)

public:

    caToggleButton(QWidget *parent);

    enum colMode {Default, Static, Alarm};

    QColor getForeground() const {return thisForeColor;}
    void setForeground(QColor c);
    QColor getBackground() const {return thisBackColor;}
    void setBackground(QColor c);

    colMode getColorMode() const { return thisColorMode; }
    void setColorMode(colMode colormode) {thisColorMode = colormode;
                                          setBackground(thisBackColor);
                                          setForeground(thisForeColor);
                                           }
    void setAlarmColors(short status);
    void setUserAlarmColors(double val);
    void setColors(QColor bg, QColor fg);
    void setNormalColors();

    QString getPV() const;
    void setPV(QString const &newPV);

    void setState(bool state);
    bool bitState(long value);

    int getAccessW() const {return _AccessW;}
    void setAccessW(int access);

private slots:

    void buttonToggled();

signals:

    void toggleButtonSignal(bool checked);


protected:


private:

    QColor thisForeColor, oldForeColor;
    QColor thisBackColor, oldBackColor;
    QPalette thisPalette;
    colMode thisColorMode;
    QColor defaultBackColor;
    QColor defaultForeColor;

    QString     thisPV;
    bool eventFilter(QObject *obj, QEvent *event);
    bool _AccessW;
};

#endif  /* CATOGGLEBUTTON */