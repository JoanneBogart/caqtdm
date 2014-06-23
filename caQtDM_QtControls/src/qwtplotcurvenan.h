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

#ifndef QWTPLOTCURVENAN_H
#define QWTPLOTCURVENAN_H

#if defined(_MSC_VER)
#define QWT_DLL
#endif

#include <qtcontrols_global.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_intervalcurve.h>
#include <stdio.h>

// this class allows to skip NaN numbers when drawing curves

class QTCON_EXPORT  QwtPlotCurveNaN : public QwtPlotCurve
{

public:

    QwtPlotCurveNaN(const QString &title = QString::null );
    void setSamplesList(QList<QPointF>& Samples);
    void getLimits(double &ymin, double &ymax);

protected:

    virtual void drawSeries(QPainter *painter, const QwtScaleMap &xMap,const QwtScaleMap &yMap, const QRectF &canvRect, int from, int to) const;

private:

    QList<QPointF> samples;
};


class QTCON_EXPORT  QwtPlotIntervalCurveNaN : public QwtPlotIntervalCurve
{

public:

    QwtPlotIntervalCurveNaN(const QString &title = QString::null );
    void setSamplesList(QList<QwtIntervalSample>& Samples);
    void getLimits(double &ymin, double &ymax);

protected:

    virtual void drawSeries(QPainter *painter, const QwtScaleMap &xMap,const QwtScaleMap &yMap, const QRectF &canvRect, int from, int to) const;

private:

    QList<QwtIntervalSample> samples;
};
#endif // QWTPLOTCURVENAN_H



