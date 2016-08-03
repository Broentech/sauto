//h+//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016 Broentech Solutions AS
// Contact: https://broentech.no/#!/contact
//
//
// GNU Lesser General Public License Usage
// This file may be used under the terms of the GNU Lesser
// General Public License version 3 as published by the Free Software
// Foundation and appearing in the file LICENSE.LGPL3 included in the
// packaging of this file. Please review the following information to
// ensure the GNU Lesser General Public License version 3 requirements
// will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
//
//
//h+//////////////////////////////////////////////////////////////////////////
//
//  \file      waveletWidget.cpp
//
//  \brief     Definition of a widget to define a wavelet
//
//  \author    Stian Broen
//
//  \date      28.12.2012
//
//
//
//
//  \par       Revision History
//
//
//             
//
//
//h-//////////////////////////////////////////////////////////////////////////

// Qt includes
#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDial>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QToolButton>
#include <QPushButton>
#include <QDir>
#include <QFileDialog>
#include <QButtonGroup>
#include <QTimeEdit>
#include <QComboBox>
#include <QtMath>
#include <QStandardPaths>
#include <QtDebug>

// local includes
#include "waveletWidget.h"

using namespace sauto;

static const double m_pi = 3.14159;
static const QChar m_piChar(960);

CWaveView::CWaveView(QWidget *parent)
   :QWidget(parent),
   m_type(WAVELET),
   m_period(0),
   m_period_s(0),
   m_period_m(0),
   m_period_h(0),
   m_phase(0),
   m_waveLimitSeconds(120),
   m_togglePeaks(false),
   m_toggleValleys(false),
   m_toggleRising(false),
   m_toggleSinking(false),
   m_frame(0)
{
   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
   setFocusPolicy(Qt::StrongFocus);
   setMinimumSize(500, 150);
   drawFrame();
   update();
}

CWaveView::~CWaveView()
{
   delete m_frame;
}

void CWaveView::setData(const SautoModel &data)
{
   m_type = data.getType();
   m_period_s = data.getSecs();
   m_period_m = data.getMins();
   m_period_h = data.getHours();
   m_phase    = (data.getPhase() * m_pi) / 180;
   m_togglePeaks   = data.hasPeak();
   m_toggleValleys = data.hasValley();
   m_toggleRising  = data.hasRising();
   m_toggleSinking = data.hasSinking();
   redraw();
}

void CWaveView::changeMode(EIntervalType mode)
{
   m_type = mode;
   redraw();
}

void CWaveView::redraw()
{
   recalcWindow();
   drawFrame();
   switch(m_type)
   {
   case(WAVELET):
      drawGrid();
      drawWave();
      break;

   case(STATIC):
      drawClockGrid();
      drawStaticIntervals();
      break;

   case(SINGLE):
      drawClockGrid();
      drawSingleShot();
      break;

   case(NOT_SPECIFIED):
      drawGrid();
      drawWave();
      break;

   default:
      break;
   }

   update();
   emit updateParentGui();
}

void CWaveView::drawFrame()
{
   delete m_frame;
   m_frame = 0;

   QRectF allRect(QPointF(this->rect().topLeft().x(), this->rect().topLeft().y()),
      QPointF(this->rect().bottomRight().x(), this->rect().bottomRight().y()));

   m_frame = new QPixmap(this->size());
   QPainter painter(m_frame);
   painter.setRenderHint(QPainter::Antialiasing);
   QLinearGradient grad(allRect.topLeft(), allRect.bottomRight());
   grad.setColorAt(0, Qt::white);
   grad.setColorAt(1, Qt::lightGray);
   QBrush linGradBrush(grad);
   painter.fillRect(allRect, linGradBrush);
   painter.drawRect(allRect);

   switch (m_type)
   {
   case(WAVELET) :
      painter.drawText(QPoint(5, 15), QString("Period : %1h %2m %3s").arg(m_period_h).arg(m_period_m).arg(m_period_s));
      painter.drawText(QPoint(5, 35), QString("Phase  : %1 deg.").arg((m_phase * 180) / m_pi));
      break;

   case(STATIC) :
      painter.drawText(QPoint(5, 15), QString("Period : %1h %2m %3s").arg(m_period_h).arg(m_period_m).arg(m_period_s));
      break;

   case(SINGLE) :
      painter.drawText(QPoint(5, 15), QString("Time of Event : %1:%2:%3").arg(m_period_h).arg(m_period_m).arg(m_period_s));
      break;

   case(NOT_SPECIFIED) : // fall through
   default:
   // no need to draw a grid
   break;
   }

   painter.end();
}

void CWaveView::drawGrid()
{
   if (m_period <= 0)
   {
      return;
   }
   if (0 == m_frame)
   {
      drawFrame();
   }

   QRect m_rect = this->rect();
   qreal noPeriods = m_waveLimitSeconds / m_period;
   qreal stepSize = qRound(m_rect.width() / noPeriods);

   QColor lightGrey(100, 100, 100, 100);
   QPainter painter(m_frame);
   for (qreal i = 0; i < (2 * noPeriods); i += 0.25)
   {
      painter.setPen(Qt::red);
      QPointF roof(i*stepSize, 0);
      QPointF top(i*stepSize, m_rect.height() - 10);
      QPointF bot(i*stepSize, m_rect.height());
      painter.drawLine(top, bot);
      top.setX(top.x() + 1);
      top.setY(top.y() - 1);
      painter.setRenderHint(QPainter::Antialiasing);
      painter.drawText(top, QString("%1%2").arg(i * 2).arg(m_piChar));
      painter.setPen(lightGrey);
      painter.setRenderHint(QPainter::Antialiasing, false);
      painter.drawLine(roof, bot);
   }
   painter.end();
}

void CWaveView::drawClockGrid()
{
   QColor lightGrey(100,100,100,100);
   QPainter painter(m_frame);
   painter.setPen(lightGrey);
   qreal gapSize = static_cast<qreal>(m_frame->width()) / 24.0;
   for(qreal i=0; i<24; i++)
   {
      painter.drawLine(QPointF(i*gapSize, 0), QPointF(i*gapSize, m_frame->height()));
      painter.setRenderHint(QPainter::Antialiasing);
      painter.drawText(QPointF(i*gapSize + 2, m_frame->height() - 5), QString("%1").arg(i));
      painter.setRenderHint(QPainter::Antialiasing, false);
   }
}

void CWaveView::drawStaticIntervals()
{
   QPainter painter(m_frame);
   painter.setPen(Qt::blue);
   qreal noStepsF = 0;
   if (m_period <= 0)
   {
      return;
   }
   else
   {
      noStepsF = static_cast<qreal>(secsPer_Day) / static_cast<qreal>(m_period);
   }
   qreal stepSize = static_cast<qreal>(m_frame->width()) / noStepsF;
   if (stepSize < 2)
   {
      painter.fillRect(QRectF(QPointF(0, m_frame->height() / 2), QPointF(m_frame->width(), m_frame->height())), Qt::blue);
      return;
   }
   for (qreal i = 0; i < noStepsF; i++)
   {
      painter.drawLine(QPointF(i*stepSize, m_frame->height() / 2), QPointF(i*stepSize, m_frame->height()));
   }
}

void CWaveView::drawSingleShot()
{
   qreal xVal = 0.0;
   if (m_period > 0)
   {
      xVal = (m_period * m_frame->width()) / secsPer_Day;
   }
   QPainter painter(m_frame);
   painter.setPen(Qt::blue);
   painter.drawLine(QPointF(xVal, m_frame->height() / 2), QPointF(xVal, m_frame->height()));
   painter.drawLine(QPointF(xVal - 2, m_frame->height() / 2), QPointF(xVal + 2, m_frame->height() / 2));
   painter.end();
}

void CWaveView::drawWave()
{
   QSize frame = this->size();
   m_wave = makeSineInFrame(frame, m_waveLimitSeconds, m_period, m_phase);
   QPainter painter(m_frame);
   painter.setRenderHint(QPainter::Antialiasing);
   painter.setPen(Qt::blue);
   if (m_period == 0)
   {
      painter.drawLine(QPointF(0.0, static_cast<qreal>(this->height() / 2)),
         QPointF(static_cast<qreal>(this->width()), static_cast<qreal>(this->height() / 2)));
      painter.end();
      return;
   }

   painter.drawPolyline(m_wave);
   painter.end();
   if (m_togglePeaks || m_toggleValleys || m_toggleRising || m_toggleSinking)
   {
      drawDerivates();
   }
}

void CWaveView::drawDerivates()
{
   m_peaks.clear();
   m_valleys.clear();
   m_risings.clear();
   m_sinkings.clear();
   QPointF lastPt(0, 0);
   qreal lastDiff = 0.0;
   qreal lastSecondDiff = 0.0;
   for (qreal i = 0.0; i < m_wave.size(); i++)
   {
      QPointF currPt = m_wave.at(i);
      if (i > 0)
      {
         qreal currDiff = currPt.y() - lastPt.y();
         if (i > 1)
         {
            if (lastDiff <= 0 && currDiff >= 0 && m_togglePeaks)
            {
               // this is a peak
               m_peaks << QPointF(currPt.x() - 1, currPt.y());
            }
            else if (lastDiff >= 0 && currDiff <= 0 && m_toggleValleys)
            {
               // this is a valley
               m_valleys << QPointF(currPt.x() - 1, currPt.y());
            }
            qreal currSecondDiff = currDiff - lastDiff;
            if (i > 2)
            {
               if (lastSecondDiff >= 0 && currSecondDiff <= 0 && m_toggleRising)
               {
                  // this is a turn from sinking to rising
                  m_risings << QPointF(lastPt.x() - 1, lastPt.y());
               }
               else if (lastSecondDiff <= 0 && currSecondDiff >= 0 && m_toggleSinking)
               {
                  // this is a turn from rising to sinking
                  m_sinkings << QPointF(lastPt.x() - 1, lastPt.y());
               }
            }
            lastSecondDiff = currSecondDiff;
         }
         lastDiff = currDiff;
      }
      lastPt = currPt;
   }

   averagePointFValues(m_peaks);
   averagePointFValues(m_valleys);
   averagePointFValues(m_risings);
   if (m_risings.size() > 0)
   {
      qreal sinkY = m_risings.at(0).y();
      for (int i = 0; i < m_sinkings.size(); i++)
      {
         m_sinkings[i].setY(sinkY);
      }
   }
   else
   {
      averagePointFValues(m_sinkings);
   }

   drawSquareAroundPoints(m_peaks);
   drawSquareAroundPoints(m_valleys);
   drawSquareAroundPoints(m_risings);
   drawSquareAroundPoints(m_sinkings);
}

void CWaveView::paintEvent(QPaintEvent *)
{
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing);
   if (0 != m_frame)
   {
      painter.drawPixmap(this->rect(), *m_frame, m_frame->rect());
   }
   painter.end();
}

void CWaveView::resizeEvent(QResizeEvent *)
{
   redraw();
}

void CWaveView::averagePointFValues(QPolygonF &points)
{
   qreal averagedVal = 0;
   for (int i = 0; i < points.size(); i++)
   {
      averagedVal += points.at(i).y();
   }
   averagedVal /= points.size();
   for (int i = 0; i < points.size(); i++)
   {
      points[i].setY(averagedVal);
   }
}

void CWaveView::drawSquareAroundPoints(const QPolygonF &points)
{
   if (points.size() <= 0 || 0 == m_frame)
   {
      return;
   }

   QPen greenThick;
   greenThick.setColor(Qt::black);
   greenThick.setWidth(1);

   QColor transparentGrey(100, 100, 100, 100);

   QPainter painter(m_frame);
   painter.setPen(greenThick);
   for (int i = 0; i < points.size(); i++)
   {
      QPointF currPt = points.at(i);
      QPointF tl(currPt.x() - 6, currPt.y() - 6);
      QPointF br(currPt.x() + 6, currPt.y() + 6);
      QRectF  rect(tl, br);
      painter.fillRect(rect, transparentGrey);
      painter.drawRect(rect);
   }
   painter.end();
}

void CWaveView::receiveFrequency_seconds(int val)
{
   if (val >= 0 && val < 60)
   {
      m_period_s = val;
      redraw();
   }
}

void CWaveView::receiveFrequency_minutes(int val)
{
   if (val >= 0 && val < 60)
   {
      m_period_m = val;
      redraw();
   }
}

void CWaveView::receiveFrequency_hours(int val)
{
   if (val >= 0 && val < 24)
   {
      m_period_h = val;
      redraw();
   }
}

void CWaveView::recalcWindow()
{
   if (m_type == NOT_SPECIFIED)
   {
      m_type = WAVELET;
   }

   m_period = m_period_s + (m_period_m * 60) + (m_period_h * 60 * 60);
   if (m_type != WAVELET)
   {
      // no need to make window calculations for other types than wavelets
      return;
   }
   if (m_period == 0)
   {
      m_waveLimitSeconds = 120;
      return;
   }
   while (5 * m_period > m_waveLimitSeconds)
   {
      m_waveLimitSeconds *= 2;
   }
   while (5 * m_period < m_waveLimitSeconds)
   {
      m_waveLimitSeconds /= 2;
   }
}

void CWaveView::receivePhase(int val)
{
   m_phase = (val * m_pi) / 180;
   redraw();
}

void CWaveView::peakStateChanged(bool val)
{
   m_togglePeaks = val;
   redraw();
}

void CWaveView::valleyStateChanged(bool val)
{
   m_toggleValleys = val;
   redraw();
}

void CWaveView::risingStateChanged(bool val)
{
   m_toggleRising = val;
   redraw();
}

void CWaveView::sinkingStateChanged(bool val)
{
   m_toggleSinking = val;
   redraw();
}

CWaveletWidget::CWaveletWidget(QWidget *parent, bool allowCustomDuration)
   :QDialog(parent),
   m_customDurationAllowed(allowCustomDuration),
   m_durationEdit(0),
   m_enableDuration(0),
   m_typeGroup(0),
   m_type(WAVELET),
   m_OK(0),
   m_cancel(0),
   m_wave(0),
   m_freq_seconds(0),
   m_freq_minutes(0),
   m_freq_hours(0),
   m_phase(0),
   m_onPeakTaskBox(0),
   m_onValleyTaskBox(0),
   m_onRisingTaskBox(0),
   m_onSinkingTaskBox(0),
   m_peakBox(0),
   m_valleyBox(0),
   m_risingBox(0),
   m_sinkingBox(0),
   m_checkType_wave(0),
   m_checkType_static(0),
   m_checkType_single(0)
{
   initGui();
   updateData();
   updateWidgets();
   updateWidgetContent();
}

CWaveletWidget::~CWaveletWidget()
{

}

void CWaveletWidget::initGui()
{
   m_wave = new CWaveView(this);

   m_freq_seconds = new QDial(this);
   m_freq_seconds->setRange(0, 59);
   m_freq_seconds->setToolTip(tr("Period Seconds"));

   m_freq_minutes = new QDial(this);
   m_freq_minutes->setRange(0, 59);
   m_freq_minutes->setToolTip(tr("Period Minutes"));

   m_freq_hours = new QDial(this);
   m_freq_hours->setRange(0, 24);
   m_freq_hours->setToolTip(tr("Period Hours"));

   m_phase = new QDial(this);
   m_phase->setRange(0, 360);
   m_phase->setToolTip(tr("Phase"));

   m_freq_seconds->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
   m_freq_minutes->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
   m_freq_hours->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
   m_phase->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

   m_freq_seconds->setMaximumSize(40, 40);
   m_freq_minutes->setMaximumSize(40, 40);
   m_freq_hours->setMaximumSize(40, 40);
   m_phase->setMaximumSize(40, 40);

   QGridLayout *dialLay = new QGridLayout;
   dialLay->addWidget(new QLabel("S"), 0, 0);
   dialLay->addWidget(new QLabel("M"), 1, 0);
   dialLay->addWidget(new QLabel("H"), 2, 0);
   dialLay->addWidget(new QLabel("P"), 3, 0);
   dialLay->addWidget(m_freq_seconds, 0, 1);
   dialLay->addWidget(m_freq_minutes, 1, 1);
   dialLay->addWidget(m_freq_hours, 2, 1);
   dialLay->addWidget(m_phase, 3, 1);

   QLabel *peakLabel = new QLabel(tr("Peak"), this);
   QLabel *vallLabel = new QLabel(tr("Valley"), this);
   QLabel *riseLabel = new QLabel(tr("Rising"), this);
   QLabel *sinkLabel = new QLabel(tr("Sinking"), this);

   peakLabel->setToolTip(tr("Task to perform on maximum signal value"));
   vallLabel->setToolTip(tr("Task to perform on minimum signal value"));
   riseLabel->setToolTip(tr("Task to perform when signal goes from sinking to rising value increment"));
   sinkLabel->setToolTip(tr("Task to perform when signal goes from rising to sinking value increment"));

   m_peakBox = new QCheckBox(this);
   m_valleyBox = new QCheckBox(this);
   m_risingBox = new QCheckBox(this);
   m_sinkingBox = new QCheckBox(this);

   m_onPeakTaskBox = new QComboBox(this);
   m_onValleyTaskBox = new QComboBox(this);
   m_onRisingTaskBox = new QComboBox(this);
   m_onSinkingTaskBox = new QComboBox(this);

   QGridLayout *buttonLay = new QGridLayout;
   buttonLay->addWidget(peakLabel, 0, 0);
   buttonLay->addWidget(vallLabel, 1, 0);
   buttonLay->addWidget(riseLabel, 2, 0);
   buttonLay->addWidget(sinkLabel, 3, 0);
   buttonLay->addWidget(m_peakBox, 0, 1);
   buttonLay->addWidget(m_valleyBox, 1, 1);
   buttonLay->addWidget(m_risingBox, 2, 1);
   buttonLay->addWidget(m_sinkingBox, 3, 1);
   buttonLay->addWidget(m_onPeakTaskBox, 0, 2);
   buttonLay->addWidget(m_onValleyTaskBox, 1, 2);
   buttonLay->addWidget(m_onRisingTaskBox, 2, 2);
   buttonLay->addWidget(m_onSinkingTaskBox, 3, 2);

   connectWave();

   m_OK = new QPushButton(tr("OK"), this);
   connect(m_OK, SIGNAL(clicked()), this, SLOT(OKClicked()));
   m_cancel = new QPushButton(tr("Cancel"), this);
   connect(m_cancel, SIGNAL(clicked()), this, SLOT(cancelClicked()));

   m_checkType_wave = new QPushButton(tr("Wavelet"), this);
   m_checkType_static = new QPushButton(tr("Static"), this);
   m_checkType_single = new QPushButton(tr("Single"), this);
   m_checkType_wave->setCheckable(true);
   m_checkType_static->setCheckable(true);
   m_checkType_single->setCheckable(true);
   m_checkType_wave->setChecked(true);
   m_checkType_static->setChecked(false);
   m_checkType_single->setChecked(false);

   m_typeGroup = new QButtonGroup(this);
   m_typeGroup->setExclusive(true);
   m_typeGroup->addButton(m_checkType_wave);
   m_typeGroup->addButton(m_checkType_static);
   m_typeGroup->addButton(m_checkType_single);
   connect(m_typeGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(typeButtonClicked(QAbstractButton *)));

   QHBoxLayout *typeLay = new QHBoxLayout;
   typeLay->addWidget(m_checkType_wave);
   typeLay->addWidget(m_checkType_static);
   typeLay->addWidget(m_checkType_single);

   QHBoxLayout *topLay = new QHBoxLayout;
   topLay->addLayout(dialLay);
   topLay->addWidget(m_wave);
   topLay->addLayout(buttonLay);

   QHBoxLayout *bottomLay = new QHBoxLayout;
   bottomLay->addWidget(m_OK);
   bottomLay->addWidget(m_cancel);

   if (m_customDurationAllowed)
   {
      QLabel *durLabel = new QLabel("D", this);
      m_durationEdit = new QTimeEdit(this);
      m_durationEdit->setToolTip(tr("Wavelet Duration"));
      m_durationEdit->setEnabled(false);
      connect(m_durationEdit, SIGNAL(timeChanged(const QTime &)), this, SLOT(customDurationChanged(const QTime &)));
      m_enableDuration = new QCheckBox(this);
      m_enableDuration->setChecked(false);
      m_enableDuration->setToolTip(tr("Enable Custom Duration"));
      connect(m_enableDuration, SIGNAL(stateChanged(int)), this, SLOT(enableCustomDuration(int)));
      bottomLay->addStretch();
      bottomLay->addWidget(durLabel);
      bottomLay->addWidget(m_enableDuration);
      bottomLay->addWidget(m_durationEdit);
      m_data.setHasCustomInterval(true);
   }

   bottomLay->addStretch();
   bottomLay->addLayout(typeLay);

   QVBoxLayout *lay = new QVBoxLayout;
   lay->addLayout(topLay);
   lay->addLayout(bottomLay);

   this->setLayout(lay);
}

void CWaveletWidget::typeButtonClicked(QAbstractButton *button)
{
   if (button == m_checkType_single)
   {
      // not really an interval at all, simply a single scheduled worktask
      m_type = SINGLE;
   }
   else if (button == m_checkType_static)
   {
      // meaning the space between the tasks to perform are always the same
      m_type = STATIC;
   }
   else if (button == m_checkType_wave)
   {
      // tasks are scheduled on a wavelets derivates
      m_type = WAVELET;
   }
   else
   {
      // this is an error, should in theory never happen
      qCritical() << QString("Undefined widget action");
      m_type = NOT_SPECIFIED;
      return;
   }

   updateData();
   updateWidgets();
   emit changeMode(m_type);
}

void CWaveletWidget::updateWidgets()
{
   const bool hasValidData = m_data.isValid();
   if (hasValidData)
   {
      m_OK->setEnabled(true);
   }
   else
   {
      m_OK->setEnabled(false);
   }

   if (!m_data.getOnPeak().isEmpty() && m_onPeakTaskBox->findText(m_data.getOnPeak()) > -1)
   {
      m_onPeakTaskBox->setCurrentIndex(m_onPeakTaskBox->findText(m_data.getOnPeak()));
   }

   if (!m_data.getOnRising().isEmpty() && m_onRisingTaskBox->findText(m_data.getOnRising()) > -1)
   {
      m_onRisingTaskBox->setCurrentIndex(m_onRisingTaskBox->findText(m_data.getOnRising()));
   }

   if (!m_data.getOnSinking().isEmpty() && m_onSinkingTaskBox->findText(m_data.getOnSinking()) > -1)
   {
      m_onSinkingTaskBox->setCurrentIndex(m_onSinkingTaskBox->findText(m_data.getOnSinking()));
   }

   if (!m_data.getOnValley().isEmpty() && m_onValleyTaskBox->findText(m_data.getOnValley()) > -1)
   {
      m_onValleyTaskBox->setCurrentIndex(m_onValleyTaskBox->findText(m_data.getOnValley()));
   }

   if (m_customDurationAllowed)
   {
      if (m_data.getHasCustomInterval())
      {
         m_durationEdit->setEnabled(true);
         if (m_data.getDuration() <= 0)
         {
            m_OK->setEnabled(false);
         }
         else
         {
            m_OK->setEnabled(true);
         }
      }
      else
      {
         m_durationEdit->setEnabled(false);
      }
   }

   switch (m_type)
   {
   case(SINGLE) :
      m_phase->setEnabled(false);
      m_onValleyTaskBox->setEnabled(false);
      m_onRisingTaskBox->setEnabled(false);
      m_onSinkingTaskBox->setEnabled(false);
      m_valleyBox->setEnabled(false);
      m_risingBox->setEnabled(false);
      m_sinkingBox->setEnabled(false);
      break;

   case(STATIC) :
      m_phase->setEnabled(false);
      m_onValleyTaskBox->setEnabled(false);
      m_onRisingTaskBox->setEnabled(false);
      m_onSinkingTaskBox->setEnabled(false);
      m_valleyBox->setEnabled(false);
      m_risingBox->setEnabled(false);
      m_sinkingBox->setEnabled(false);
      break;

   case(WAVELET) :
      m_phase->setEnabled(true);
      m_onValleyTaskBox->setEnabled(true);
      m_onRisingTaskBox->setEnabled(true);
      m_onSinkingTaskBox->setEnabled(true);
      m_valleyBox->setEnabled(true);
      m_risingBox->setEnabled(true);
      m_sinkingBox->setEnabled(true);
      break;

   case(NOT_SPECIFIED) :
      m_phase->setEnabled(true);
      m_onValleyTaskBox->setEnabled(true);
      m_onRisingTaskBox->setEnabled(true);
      m_onSinkingTaskBox->setEnabled(true);
      m_valleyBox->setEnabled(true);
      m_risingBox->setEnabled(true);
      m_sinkingBox->setEnabled(true);
      break;

   default:
      qCritical() << QString("Undefined interval type");
      break;
   }
}

void CWaveletWidget::connectWave()
{
   connect(m_sinkingBox, SIGNAL(toggled(bool)),
      m_wave, SLOT(sinkingStateChanged(bool)));

   connect(m_risingBox, SIGNAL(toggled(bool)),
      m_wave, SLOT(risingStateChanged(bool)));

   connect(m_valleyBox, SIGNAL(toggled(bool)),
      m_wave, SLOT(valleyStateChanged(bool)));

   connect(m_peakBox, SIGNAL(toggled(bool)),
      m_wave, SLOT(peakStateChanged(bool)));

   connect(m_freq_seconds, SIGNAL(valueChanged(int)),
      m_wave, SLOT(receiveFrequency_seconds(int)));

   connect(m_freq_minutes, SIGNAL(valueChanged(int)),
      m_wave, SLOT(receiveFrequency_minutes(int)));

   connect(m_freq_hours, SIGNAL(valueChanged(int)),
      m_wave, SLOT(receiveFrequency_hours(int)));

   connect(m_phase, SIGNAL(valueChanged(int)),
      m_wave, SLOT(receivePhase(int)));

   connect(m_wave, SIGNAL(updateParentGui()),
      this, SLOT(updateWidgets()));

   connect(this, SIGNAL(changeMode(EIntervalType)),
      m_wave, SLOT(changeMode(EIntervalType)));
}

void CWaveletWidget::disconnectWave()
{
   disconnect(m_sinkingBox, SIGNAL(toggled(bool)),
      m_wave, SLOT(sinkingStateChanged(bool)));

   disconnect(m_risingBox, SIGNAL(toggled(bool)),
      m_wave, SLOT(risingStateChanged(bool)));

   disconnect(m_valleyBox, SIGNAL(toggled(bool)),
      m_wave, SLOT(valleyStateChanged(bool)));

   disconnect(m_peakBox, SIGNAL(toggled(bool)),
      m_wave, SLOT(peakStateChanged(bool)));

   disconnect(m_freq_seconds, SIGNAL(valueChanged(int)),
      m_wave, SLOT(receiveFrequency_seconds(int)));

   disconnect(m_freq_minutes, SIGNAL(valueChanged(int)),
      m_wave, SLOT(receiveFrequency_minutes(int)));

   disconnect(m_freq_hours, SIGNAL(valueChanged(int)),
      m_wave, SLOT(receiveFrequency_hours(int)));

   disconnect(m_phase, SIGNAL(valueChanged(int)),
      m_wave, SLOT(receivePhase(int)));

   disconnect(m_wave, SIGNAL(updateParentGui()),
      this, SLOT(updateWidgets()));

   disconnect(this, SIGNAL(changeMode(EIntervalType)),
      m_wave, SLOT(changeMode(EIntervalType)));
}

bool CWaveletWidget::evaluateTaskFile(const QString &fileName)
{
   if (fileName == "" || fileName.isNull() || fileName.isEmpty())
   {
      return false;
   }
   QFileInfo fInfo(fileName);
   if (!fInfo.exists())
   {
      return false;
   }

   // TODO : read xml file and check that content is valid task content
   return false;
}

void CWaveletWidget::setData(const SautoModel &data)
{
   disconnectWave();

   switch (data.getType())
   {
   case(WAVELET) :
      m_checkType_wave->setChecked(true);
      break;

   case(STATIC) :
      m_checkType_static->setChecked(true);
      break;

   case(SINGLE) :
      m_checkType_single->setChecked(true);
      break;

   case(NOT_SPECIFIED) :
      m_checkType_wave->setChecked(true);
      break;

   default:
   connectWave();
   return;
   }
   m_type = data.getType();
   m_data = data;
   if (m_type == SINGLE)
   {
      QTime time = QTime(0, 0, 0).addMSecs(m_data.getStartTimeMSec());
      m_data.setSeconds(time.second());
      m_data.setMinutes(time.minute());
      m_data.setHours(time.hour());
   }
   m_freq_seconds->setValue(m_data.getSecs());
   m_freq_minutes->setValue(m_data.getMins());
   m_freq_hours->setValue(m_data.getHours());
   m_phase->setValue(m_data.getPhase());
   m_peakBox->setChecked(m_data.hasPeak());
   m_valleyBox->setChecked(m_data.hasValley());
   m_risingBox->setChecked(m_data.hasRising());
   m_sinkingBox->setChecked(m_data.hasSinking());

   if (m_customDurationAllowed && 0 != m_durationEdit)
   {
      if (m_data.getHasCustomInterval())
      {
         QTime time = QTime(0, 0, 0).addMSecs(m_data.getDuration());
         m_durationEdit->setTime(time);
      }
      if (m_data.getStartTimeMSec() < 0)
      {
         m_enableDuration->setChecked(true);
         m_durationEdit->setEnabled(true);
      }
      else
      {
         m_enableDuration->setChecked(false);
         m_durationEdit->setEnabled(false);
      }
   }

   m_wave->setData(m_data);
   updateWidgets();
   connectWave();
}

SautoModel CWaveletWidget::getData()
{
   updateData();
   return m_data;
}

void CWaveletWidget::updateData()
{
   if (m_checkType_wave->isChecked())
   {
      m_type = WAVELET;
   }
   else if (m_checkType_static->isChecked())
   {
      m_type = STATIC;
   }
   else if (m_checkType_single->isChecked())
   {
      m_type = SINGLE;
   }
   else
   {
      m_type = NOT_SPECIFIED;
   }

   m_data.setType(m_type);
   m_data.setSeconds(m_freq_seconds->value());
   m_data.setMinutes(m_freq_minutes->value());
   m_data.setHours(m_freq_hours->value());
   m_data.setPhase(m_phase->value());

   if (m_customDurationAllowed && 0 != m_durationEdit)
   {
      if (m_enableDuration->isChecked())
      {
         QTime time = m_durationEdit->time();
         qint64 msec = get_MSEC_sinceMidnight(time);
         m_data.setDuration(msec);
         m_data.setHasCustomInterval(true);
      }
      else
      {
         m_data.setHasCustomInterval(false);
      }
   }

   if (m_type == SINGLE)
   {
      quint64 start = m_data.getPeriodTotMSec();
      m_data.setStartTimeMSecs(start);
   }

   if (m_peakBox->isEnabled() && m_peakBox->isChecked())
   {
      m_data.setOnPeak(m_onPeakTaskBox->currentText());
   }
   else
   {
      m_data.setOnPeak("");
   }

   if (m_valleyBox->isEnabled() && m_valleyBox->isChecked())
   {
      m_data.setOnValley(m_onValleyTaskBox->currentText());
   }
   else
   {
      m_data.setOnValley("");
   }

   if (m_risingBox->isEnabled() && m_risingBox->isChecked())
   {
      m_data.setOnRising(m_onRisingTaskBox->currentText());
   }
   else
   {
      m_data.setOnRising("");
   }

   if (m_sinkingBox->isEnabled() && m_sinkingBox->isChecked())
   {
      m_data.setOnSinking(m_onSinkingTaskBox->currentText());
   }
   else
   {
      m_data.setOnSinking("");
   }
}

void CWaveletWidget::OKClicked()
{
   updateData();
   emit dataSignal(m_data);
   emit closeMe();
}

void CWaveletWidget::cancelClicked()
{
   emit cancelled();
   emit closeMe();
}

void CWaveletWidget::customDurationChanged(const QTime &time)
{
   qint64 msec = get_MSEC_sinceMidnight(time);
   m_data.setDuration(msec);
   updateData();
   updateWidgets();
}

void CWaveletWidget::enableCustomDuration(int state)
{
   bool on;
   (state == 0) ? on = false : on = true;
   emit isSingleInterval(on);
   if (on)
   {
      m_durationEdit->setEnabled(true);
      m_data.setHasCustomInterval(true);
      QTime time = m_durationEdit->time();
      qint64 msec = get_MSEC_sinceMidnight(time);
      m_data.setDuration(msec);
      m_data.setStartTimeMSecs(-1); // by setting to -1, interval starts at current time, even though it has a duration
   }
   else
   {
      m_durationEdit->setEnabled(false);
      m_data.setHasCustomInterval(false);
      m_data.setDuration(msecsPer_Day);
      m_data.setStartTimeMSecs(0);
   }
   updateData();
   updateWidgets();
}

void CWaveletWidget::updateWidgetContent()
{
   if (0 == m_onPeakTaskBox ||
      0 == m_onValleyTaskBox ||
      0 == m_onRisingTaskBox ||
      0 == m_onSinkingTaskBox)
   {
      return;
   }
   m_onPeakTaskBox->clear();
   m_onValleyTaskBox->clear();
   m_onRisingTaskBox->clear();
   m_onSinkingTaskBox->clear();

   QString _appdir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tasks";

   fillComboBox(m_onPeakTaskBox, _appdir, "xml");
   fillComboBox(m_onValleyTaskBox, _appdir, "xml");
   fillComboBox(m_onRisingTaskBox, _appdir, "xml");
   fillComboBox(m_onSinkingTaskBox, _appdir, "xml");
   fillComboBox(m_onPeakTaskBox, _appdir, "json", "", false, false);
   fillComboBox(m_onValleyTaskBox, _appdir, "json", "", false, false);
   fillComboBox(m_onRisingTaskBox, _appdir, "json", "", false, false);
   fillComboBox(m_onSinkingTaskBox, _appdir, "json", "", false, false);
}

QPolygonF sauto::makeSineInFrame(
   const QSize &frame,
   const qreal secWindow,
   const qreal secPeriod,
   const qreal phase,
   const qreal ampl_shrinkage
   )
{
   QPolygonF poly;
   if (secPeriod == 0)
   {
      return poly;
   }
   const qreal m_h = static_cast<qreal>(frame.height());
   const qreal m_w = static_cast<qreal>(frame.width());
   const qreal stepSize = (secWindow * 360) / (secPeriod*m_w);
   for (qreal pixel = 1; pixel <= m_w; pixel++)
   {
      const qreal degrees = pixel*stepSize;
      const qreal radians = ((degrees * m_pi) / 180.0) + phase;
      const qreal sinval = -qSin(radians);
      const qreal x = pixel;
      const qreal y = (sinval * ((m_h - ampl_shrinkage) / 2)) + (m_h) / 2;
      poly << QPointF(x, y);
   }
   return poly;
}

void sauto::fillComboBox(
   QComboBox *box, 
   const QString &path, 
   const QString &suffix, 
   const QString &emptyFiller, 
   bool folders, 
   bool doClear
   )
{
   if (0 == box)
   {
      return;
   }
   if (doClear)
   {
      box->clear();
   }

   QFileInfoList list = QDir(path).entryInfoList();
   for (int i = 0; i<list.size(); i++)
   {
      QFileInfo fInfo = list.at(i);
      if (folders && fInfo.isDir())
      {
         const QString bName = fInfo.baseName();
         if (bName != "")
         {
            box->addItem(bName);
         }
         continue;
      }
      if (suffix != "" && fInfo.suffix() != suffix)
      {
         continue;
      }
      box->addItem(fInfo.baseName());
   }

   if (emptyFiller != "" && box->count() == 0)
   {
      box->addItem(emptyFiller);
   }
}