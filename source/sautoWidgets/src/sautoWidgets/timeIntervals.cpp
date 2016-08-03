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
//  \file      timeIntervals.cpp
//
//  \brief     Definition of widget to specify a time interval
//
//  \author    Stian Broen
//
//  \date      27.06.2012
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
#include <QTimeEdit>
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QRadioButton>
#include <QMenu>
#include <QWidgetAction>
#include <QButtonGroup>
#include <QtCore/qmath.h>

// local includes
#include "waveletWidget.h"
#include "timeIntervals.h"

using namespace sauto;

CTimeLineView::CTimeLineView(QWidget *parent)
   :QWidget(parent),
   m_intervalType(NOT_SPECIFIED)
{
   resizeIndex_trueStart.first = -1;
   m_lastPos = 0;
   secsPerPixel = 0;
   m_frame = 0;
   freqWidget = new CWaveletWidget(this);
   freqMenu = new QMenu(this);
   QWidgetAction *m_waveWidgetAction = new QWidgetAction(this);
   m_waveWidgetAction->setDefaultWidget(freqWidget);
   freqMenu->addAction(m_waveWidgetAction);
   freqMenu->show();
   freqMenu->hide();

   connect(freqWidget, SIGNAL(closeMe()), 
      this, SLOT(receivedCloseFreqSig()));

   connect(freqWidget, SIGNAL(dataSignal(const SautoModel &)), 
      this, SLOT(receivedFreq(const SautoModel &)));

   isResizingBar = false;
   mouseIsPressed = false;
   barMoveInitOK = false;
   isMovingBar = false;
   qRegisterMetaType<TimeInterval>("TimeInterval");
   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
   setFocusPolicy(Qt::StrongFocus);

   viewRect = QRectF(QPointF(0,0), QPointF(0,0));
   if(0 != parent)
   {
      setMinimumSize(parent->width() - 10, 100);
   }
   setMouseTracking(true);
   drawFrame();
   update();
}

CTimeLineView::~CTimeLineView()
{
   delete m_frame;
}

void CTimeLineView::receivedCloseFreqSig()
{
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while(it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if(interval.m_hasFocus)
      {
         interval.m_useFreq = false;
         it.setValue(interval);
         break;
      }
   }
   freqMenu->close();
   update();
}

void CTimeLineView::receivedFreq(const SautoModel &data)
{
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while(it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if((interval.m_hasFocus))
      {
         interval.m_waveData = data;
         interval.m_waveData.setHasCustomInterval(true);
         quint64 startTime_ms = 0;
         quint64 stopTime_ms  = 0;
         switch(interval.m_waveData.getType())
         {
         case(WAVELET):
            startTime_ms = (qRound((interval.m_position_start - viewRect.topLeft().x()) * secsPerPixel)) * 1000;
            stopTime_ms  = (qRound((interval.m_position_stop  - viewRect.topLeft().x()) * secsPerPixel)) * 1000;
            interval.m_waveData.setStartTimeMSecs(startTime_ms);
            interval.m_waveData.setDuration(stopTime_ms - startTime_ms);
            interval.m_useFreq = true;
            it.setValue(interval);
            break;

         case(STATIC):
            interval.m_useFreq = true;
            it.setValue(interval);
            break;

         case(SINGLE):
            interval.m_useFreq = false;
            interval.m_position_start = timeToPos(QTime(0,0,0).addMSecs(data.getStartTimeMSec()));
            it.setValue(interval);
            break;

         default:
            break;
         }
         break;
      }
   }
   freqMenu->close();
   update();
}

void CTimeLineView::receiveIntervalType(EIntervalType type)
{
   m_intervalType = type;
}

QPolygonF CTimeLineView::getPoly(const QPointF &pt, qreal halfDist)
{
   QPolygonF returnMe;
   returnMe << QPointF(pt.x() - halfDist, pt.y())
      << QPointF(pt.x() + halfDist, pt.y())
      << QPointF(pt.x(), pt.y() - 2 * halfDist)
      << QPointF(pt.x() - halfDist, pt.y());
   return returnMe;
}

void CTimeLineView::paintEvent(QPaintEvent *)
{
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing);
   if (0 != m_frame)
   {
      painter.drawPixmap(this->rect(), *m_frame, m_frame->rect());
   }

   // draw intervals first
   QHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if (interval.m_waveData.getType() != SINGLE)
      {
         // draw an interval
         drawInterval(interval, painter);
      }
   }

   // then draw singleshots
   it.toFront();
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if (interval.m_waveData.getType() == SINGLE)
      {
         // draw a singleshot
         drawSingleShot(interval, painter);
      }
   }

   painter.end();
}

void CTimeLineView::drawSingleShot(const TimeInterval &interval, QPainter &painter)
{
   QPointF top(interval.m_position_start, viewRect.topLeft().y());
   QPointF bot(interval.m_position_start, viewRect.bottomRight().y());

   QPolygonF dragPoly = getPoly(bot);
   QPainterPath path;
   path.addPolygon(dragPoly);

   QPen lastPen = painter.pen();
   QPen pen;
   pen.setColor(Qt::red);
   pen.setWidth(1);
   painter.setPen(pen);
   painter.drawLine(top, bot);
   painter.fillPath(path, Qt::red);

   if (interval.m_hasFocus)
   {
      QPolygonF bigPoly = getPoly(bot, 6.0);
      QPainterPath bigPath;
      bigPath.addPolygon(bigPoly);
      pen.setColor(Qt::black);
      painter.setPen(pen);
      painter.drawPath(bigPath);
   }

   painter.setPen(lastPen);
}

void CTimeLineView::drawInterval(const TimeInterval &interval, QPainter &painter)
{
   QRectF intervalRect = getBar(interval);
   if (interval.m_hasFocus)
   {
      QLinearGradient interGrad(intervalRect.topLeft(), intervalRect.topRight());
      if (interval.m_waveData.getType() == STATIC)
      {
         interGrad.setColorAt(0, Qt::green);
         interGrad.setColorAt(1, Qt::white);
      }
      else
      {
         interGrad.setColorAt(0, Qt::cyan);
         interGrad.setColorAt(1, Qt::white);
      }
      QBrush gradBrush(interGrad);
      painter.fillRect(intervalRect, gradBrush);
      QPen lastPen = painter.pen();
      QPen pen;
      pen.setWidth(2);
      painter.setPen(pen);
      painter.drawRect(intervalRect);
      painter.setPen(lastPen);
   }
   else
   {
      QLinearGradient interGrad(intervalRect.topLeft(), intervalRect.topRight());
      if (interval.m_waveData.getType() == STATIC)
      {
         interGrad.setColorAt(0, Qt::green);
         interGrad.setColorAt(1, Qt::white);
      }
      else
      {
         interGrad.setColorAt(0, Qt::cyan);
         interGrad.setColorAt(1, Qt::white);
      }
      QBrush greenBrush(interGrad);
      painter.fillRect(intervalRect, greenBrush);
      painter.drawRect(intervalRect);
   }
   if (interval.m_waveData.getType() == WAVELET)
   {
      qreal startSecs = (interval.m_position_start - viewRect.topLeft().x()) * secsPerPixel;
      qreal stopSecs = (interval.m_position_stop - viewRect.topLeft().x()) * secsPerPixel;

      qreal secDiff = (stopSecs - startSecs);
      QSize waveFrame(qRound(intervalRect.width()), qRound(intervalRect.height()*2.5));
      qreal period = static_cast<qreal>(interval.m_waveData.getPeriodTotMSec()) / 1000.0;
      qreal phase = interval.m_waveData.getPhase();
      qreal shrink = static_cast<qreal>(waveFrame.height()) / 1.4;
      QPolygonF poly = makeSineInFrame(waveFrame, secDiff, period, phase, shrink);

      poly.translate(QPointF(intervalRect.topLeft().x(), intervalRect.topLeft().y() / 1.75));
      QPen redPen;
      redPen.setColor(Qt::red);
      QPen lastPen = painter.pen();
      painter.setPen(redPen);
      painter.drawPolyline(poly);
      painter.setPen(lastPen);
   }
   else if (interval.m_waveData.getType() == STATIC)
   {
      // draw trigger indicators
      qreal startSecs = (interval.m_position_start - viewRect.topLeft().x()) * secsPerPixel;
      qreal stopSecs = (interval.m_position_stop - viewRect.topLeft().x()) * secsPerPixel;
      qreal secDiff = (stopSecs - startSecs);
      qreal noStepsReal = (secDiff * 1000) / interval.m_waveData.getPeriodTotMSec();
      int noSteps = qRound(noStepsReal);

      QPen redPen;
      redPen.setColor(Qt::red);
      QPen lastPen = painter.pen();
      painter.setPen(redPen);

      if (intervalRect.width() < noSteps)
      {
         // draw rectangle
         QRectF rect(QPointF(intervalRect.bottomLeft().x(), intervalRect.bottomLeft().y() - 10),
            QPointF(intervalRect.bottomRight().x(), intervalRect.bottomRight().y()));

         painter.fillRect(rect, Qt::red);
      }
      else
      {
         // draw lines

         qreal stepSize = (static_cast<qreal>(intervalRect.width())) / noStepsReal;
         for (int i = 0; i < noSteps; i++)
         {
            QPointF a = QPointF((static_cast<qreal>(intervalRect.bottomLeft().x())) + ((static_cast<qreal>(i)) * stepSize),
               static_cast<qreal>(intervalRect.bottomLeft().y()));

            QPointF b = QPointF((static_cast<qreal>(intervalRect.bottomLeft().x())) + ((static_cast<qreal>(i)) * stepSize),
               static_cast<qreal>(intervalRect.bottomLeft().y() - 10));
            painter.drawLine(a, b);
         }
      }
      painter.setPen(lastPen);
   }
}

void CTimeLineView::drawFrame()
{
   if (viewRect.size().isNull() ||
      viewRect.size().isEmpty() ||
      viewRect.topLeft() == viewRect.bottomRight())
   {
      viewRect = QRectF(QPointF(0, 0), QPointF(this->width() - 40, this->height() / 5 * 2 - 10));
      viewRect.translate(20, 50);
   }

   delete m_frame;
   m_frame = 0;

   m_frame = new QPixmap(this->size());

   QPainter painter(m_frame);
   QRectF allRect(QPointF(this->rect().topLeft().x(), this->rect().topLeft().y()),
      QPointF(this->rect().bottomRight().x() - 2, this->rect().bottomRight().y() - 1));

   QLinearGradient grad(allRect.topLeft(), allRect.bottomRight());
   grad.setColorAt(0, Qt::white);
   grad.setColorAt(1, Qt::lightGray);
   QBrush linGradBrush(grad);
   painter.fillRect(allRect, linGradBrush);
   painter.fillRect(viewRect, Qt::white);

   // draw tag lines
   qreal m_viewWidth = viewRect.width();
   for (qreal i = 0.0; i <= 24.0; i = i + 1.0)
   {
      QPointF a;
      QPointF b;
      if (i == 0.0)
      {
         a = QPointF(viewRect.topLeft().x(), viewRect.topLeft().y());
         b = QPointF(viewRect.topLeft().x(), viewRect.topLeft().y() - 5);
         painter.drawLine(a, b);
      }
      else
      {
         qreal m_x = static_cast<qreal>(viewRect.topLeft().x()) + (m_viewWidth / 24.0 * i);

         a = QPointF(m_x, viewRect.topLeft().y());
         b = QPointF(m_x, viewRect.topLeft().y() - 5);
         painter.drawLine(a, b);
      }

      // draw clock text
      if (i != 24)
      {
         QString text;
         if (i < 10.0)
         {
            text = QString("0%1").arg(static_cast<int>(i));
         }
         else
         {
            text = QString("%1").arg(static_cast<int>(i));
         }
         painter.drawText(QPointF(b.x(), b.y() - 2), text);
      }
   }

   secsPerPixel = static_cast<qreal>(secsPer_Day) / m_viewWidth;

   painter.drawRect(viewRect);
   painter.drawRect(allRect);

   painter.end();
}

void CTimeLineView::mousePressEvent(QMouseEvent *e)
{
   QPointF locPos = e->localPos();
   m_lastPos = locPos.x();
   QPoint winPos = QCursor::pos();
   bool focusSet = false;
   bool openMenu = false;

   // check singleshots focus first
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   it.toBack();
   while (it.hasPrevious())
   {
      it.previous();
      TimeInterval interval = it.value();
      QPointF bot(interval.m_position_start, viewRect.bottomRight().y());
      QPolygonF dragPoly = getPoly(bot);
      QRectF intervalRect = dragPoly.boundingRect();
      if (interval.m_waveData.getType() == SINGLE)
      {
         if (!dragPoly.containsPoint(locPos, Qt::OddEvenFill))
         {
            // not the single-target that was clicked
            interval.m_hasFocus = false;
         }
         else
         {
            if (!focusSet)
            {
               focusSet = true;
               interval.m_hasFocus = true;
               barMoveInitOK = true;
               m_intervalType = SINGLE;
            }
            else
            {
               interval.m_hasFocus = false;
            }
            if (e->button() == Qt::RightButton)
            {
               mouseReleaseEvent(e);
               int distanceY = intervalRect.bottomLeft().y() - e->pos().y();
               QRect freqGeo(QWidget::mapToGlobal(this->pos()).x(), winPos.y() + distanceY, freqWidget->width(), freqWidget->height());
               freqMenu->setGeometry(freqGeo);
               quint64 startS = ((interval.m_position_start - viewRect.topLeft().x()) * secsPerPixel) * 1000;
               interval.m_waveData.setStartTimeMSecs(startS);
               freqWidget->setData(interval.m_waveData);
               openMenu = true;
               this->setCursor(Qt::ArrowCursor);
            }
         }
         it.setValue(interval);
      }
   }

   // then check interval focus. if no singleshot focus was found
   it.toBack();
   while (it.hasPrevious())
   {
      it.previous();
      TimeInterval interval = it.value();
      if (interval.m_waveData.getType() != SINGLE)
      {
         QRectF intervalRect = getBar(interval);
         if (!openMenu && !focusSet && intervalRect.contains(locPos))
         {
            interval.m_hasFocus = true;
            m_intervalType = interval.m_waveData.getType();
            emit setMode(m_intervalType);
            if (e->button() == Qt::RightButton)
            {
               focusSet = true;
               mouseMoveEvent(e);
               int distanceY = intervalRect.bottomLeft().y() - e->pos().y();
               QRect freqGeo(QWidget::mapToGlobal(this->pos()).x() + 22, winPos.y() + distanceY, freqWidget->width(), freqWidget->height());
               freqMenu->setGeometry(freqGeo);
               freqWidget->setData(interval.m_waveData);
               this->setCursor(Qt::ArrowCursor);
               openMenu = true;
            }
            else
            {
               barMoveInitOK = true;
            }
         }
         else
         {
            interval.m_hasFocus = false;
         }
         it.setValue(interval);
      }
   }

   mouseIsPressed = true;
   mouseMoveEvent(e);
   update();
   if (openMenu)
   {
      freqMenu->exec();
      mouseReleaseEvent(e);
   }
}

void CTimeLineView::mouseMoveEvent(QMouseEvent *e)
{
   QPointF pos = e->localPos();
   qreal fPos = pos.x();
   if (!viewRect.contains(pos) && !barMoveInitOK)
   {
      this->setCursor(Qt::ArrowCursor);
      return;
   }

   if ((isResizingBar && !isMovingBar) && (isResizingBar != isMovingBar))
   {
      return resizeBar(pos);
   }

   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if (!interval.m_hasFocus)
      {
         continue;
      }

      if (interval.m_waveData.getType() == SINGLE)
      {
         // move singleshot arrow
         QPointF bot(interval.m_position_start, viewRect.bottomRight().y());
         QPolygonF dragPoly = getPoly(bot);
         if (dragPoly.containsPoint(pos, Qt::OddEvenFill) || barMoveInitOK)
         {
            if (mouseIsPressed)
            {
               this->setCursor(Qt::ClosedHandCursor);
               qreal xDiff = fPos - m_lastPos;
               if (xDiff != 0)
               {
                  if (interval.m_position_start + xDiff <= viewRect.topLeft().x())
                  {
                     m_lastPos = fPos;
                     update();
                     return;
                  }

                  interval.m_position_start += xDiff;
               }
               it.setValue(interval);
               calcTime();
               update();
            }
            else
            {
               this->setCursor(Qt::OpenHandCursor);
            }
            m_lastPos = fPos;
            return;
         }
      }
      else
      {
         // move interval rectangle
         QRectF intervalRect = getBar(interval);
         if (intervalRect.contains(pos) || barMoveInitOK)
         {
            if (mouseIsPressed)
            {
               if (!isMovingBar &&
                  QRectF(intervalRect.topLeft(),
                  QPointF(intervalRect.topLeft().x() + 5, intervalRect.bottomRight().y())).contains(pos))
               {
                  // resize start
                  isResizingBar = true;
                  resizeIndex_trueStart.first = it.key();
                  resizeIndex_trueStart.second = true;
                  this->setCursor(Qt::SizeHorCursor);
                  return resizeBar(pos);
               }
               else if (!isMovingBar &&
                  QRectF(QPointF(intervalRect.topRight().x() - 5, intervalRect.topRight().y()),
                  intervalRect.bottomRight()).contains(pos))
               {
                  // resize stop
                  isResizingBar = true;
                  resizeIndex_trueStart.first = it.key();
                  resizeIndex_trueStart.second = false;
                  this->setCursor(Qt::SizeHorCursor);
                  return resizeBar(pos);
               }
               else
               {
                  // reposition bar
                  this->setCursor(Qt::ClosedHandCursor);
                  isMovingBar = true;
               }

               qreal xDiff = fPos - m_lastPos;
               if (xDiff != 0)
               {
                  if (interval.m_position_start + xDiff <= viewRect.topLeft().x() ||
                     interval.m_position_stop + xDiff >= viewRect.bottomRight().x())
                  {
                     m_lastPos = fPos;
                     update();
                     return;
                  }

                  interval.m_position_start += xDiff;
                  interval.m_position_stop += xDiff;

                  if (interval.m_waveData.getType() == WAVELET)
                  {
                     quint64 startTime_ms = (qRound((interval.m_position_start - viewRect.topLeft().x()) * secsPerPixel)) * 1000;
                     quint64 stopTime_ms = (qRound((interval.m_position_stop - viewRect.topLeft().x()) * secsPerPixel)) * 1000;
                     interval.m_waveData.setStartTimeMSecs(startTime_ms);
                     interval.m_waveData.setDuration(stopTime_ms - startTime_ms);
                  }
               }
               it.setValue(interval);
               calcTime();
               update();
            }
            else
            {
               if (QRectF(intervalRect.topLeft(),
                  QPointF(intervalRect.topLeft().x() + 5, intervalRect.bottomRight().y())).contains(pos))
               {
                  // resize start
                  this->setCursor(Qt::SizeHorCursor);
               }
               else if (QRectF(QPointF(intervalRect.topRight().x() - 5, intervalRect.topRight().y()),
                  intervalRect.bottomRight()).contains(pos))
               {
                  // resize stop
                  this->setCursor(Qt::SizeHorCursor);
               }
               else
               {
                  // reposition bar
                  this->setCursor(Qt::OpenHandCursor);
               }
            }

            m_lastPos = fPos;
            return;
         }
      }
   }

   this->setCursor(Qt::ArrowCursor);
}

void CTimeLineView::mouseReleaseEvent(QMouseEvent *e)
{
   resizeIndex_trueStart.first = -1;
   isMovingBar = false;
   isResizingBar = false;
   mouseIsPressed = false;
   barMoveInitOK = false;
   mouseMoveEvent(e);
}

void CTimeLineView::resizeBar(const QPointF &pos)
{
   if (!isResizingBar || isMovingBar || resizeIndex_trueStart.first == -1)
   {
      return;
   }

   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      if (it.key() != resizeIndex_trueStart.first)
      {
         continue;
      }

      TimeInterval interval = it.value();
      qreal fPos = pos.x();
      qreal xDiff = fPos - m_lastPos;
      m_lastPos = fPos;
      if (xDiff != 0)
      {
         qreal testStart = interval.m_position_start + xDiff;
         qreal viewStart = viewRect.topLeft().x();

         qreal testStop = interval.m_position_stop + xDiff;
         qreal viewStop = viewRect.bottomRight().x();

         if (resizeIndex_trueStart.second && testStart <= viewStart)
         {
            return;
         }
         if (!resizeIndex_trueStart.second && testStop >= viewStop)
         {
            return;
         }

      }
      else
      {
         return;
      }

      if (resizeIndex_trueStart.second && (interval.m_position_start + xDiff < interval.m_position_stop - 20))
      {
         interval.m_position_start += xDiff;
      }
      else if (!resizeIndex_trueStart.second && interval.m_position_stop + xDiff > interval.m_position_start + 20)
      {
         interval.m_position_stop += xDiff;
      }
      else
      {
         return;
      }

      it.setValue(interval);
      break;
   }

   calcTime();
   update();
}

void CTimeLineView::calcTime()
{
   QHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if (!interval.m_hasFocus)
      {
         continue;
      }

      qreal startSecs = (interval.m_position_start - viewRect.topLeft().x()) * secsPerPixel;
      qreal stopSecs = (interval.m_position_stop - viewRect.topLeft().x()) * secsPerPixel;
      emit timeChangeSig(startSecs, stopSecs);
      return;
   }
}

void CTimeLineView::removeFocusTime()
{
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if (interval.m_hasFocus)
      {
         it.remove();
         break;
      }
   }

   it.toFront();
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      interval.m_hasFocus = true;
      it.setValue(interval);
      break;
   }

   update();
}

void CTimeLineView::changeStartTime(int newVal)
{
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if (interval.m_waveData.getType() != SINGLE && interval.m_hasFocus)
      {
         interval.m_position_start = viewRect.topLeft().x() + (newVal / secsPerPixel);
         it.setValue(interval);
         update();
         return;
      }
   }
}

void CTimeLineView::changeStopTime(int newVal)
{
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if (interval.m_hasFocus)
      {
         interval.m_position_stop = viewRect.topLeft().x() + (newVal / secsPerPixel);
         it.setValue(interval);
         update();
         return;
      }
   }
}

void CTimeLineView::changeSingleshot(int newVal)
{
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      if (interval.m_waveData.getType() == SINGLE && interval.m_hasFocus)
      {
         interval.m_position_start = viewRect.topLeft().x() + (newVal / secsPerPixel);
         it.setValue(interval);
         update();
         return;
      }
   }
}

QRectF CTimeLineView::getBar(const TimeInterval &interval)
{
   QPointF tl(interval.m_position_start, viewRect.topLeft().y());
   QPointF br(interval.m_position_stop, viewRect.bottomRight().y() + 1);
   return QRectF(tl, br);
}

void CTimeLineView::addSingleShot(const QTime &time)
{
   // first, turn off focus for all current arrows
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while(it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      interval.m_hasFocus = false;
      it.setValue(interval);
   }

   // make a new "interval", it's not really an interval since it only contains an interval start
   TimeInterval interval;
   qreal pos  = timeToPos(time);
   interval.m_position_start = pos;
   interval.m_hasFocus = true;
   interval.m_waveData.setType(SINGLE);
   int newkey = m_intervals.size();
   while(m_intervals.contains(newkey))
   {
      newkey++;
   }
   m_intervals.insert(newkey, interval);

   // update the GUI visual
   update();
}

void CTimeLineView::addInterval(quint64 start, quint64 duration, EIntervalType type)
{
   m_intervalType = type;

   // first, turn off focus for all current intervals
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      interval.m_hasFocus = false;
      it.setValue(interval);
   }

   QTime starttime = QTime(0, 0, 0).addSecs(start);
   QTime stopTime = starttime.addSecs(duration);
   qreal startPos = timeToPos(starttime);
   qreal stopPos = timeToPos(stopTime);

   TimeInterval interval;
   interval.m_waveData.setType(m_intervalType);
   interval.m_waveData.setHasCustomInterval(true);
   interval.m_hasFocus = true;
   interval.m_position_start = startPos;
   interval.m_position_stop = stopPos;

   int newkey = m_intervals.size();
   while (m_intervals.contains(newkey))
   {
      newkey++;
   }
   m_intervals.insert(newkey, interval);

   // update the GUI visual
   update();
}

qreal CTimeLineView::timeToPos(const QTime &time) const
{
   qreal steps = static_cast<qreal>(viewRect.width()) / static_cast<qreal>(secsPer_Day);
   qreal posStep = static_cast<qreal>(time.hour()   * static_cast<qreal>(secsPer_Hour)+
      time.minute() * static_cast<qreal>(secsPer_Min)+
      time.second());
   qreal pos = viewRect.topLeft().x() + (steps * posStep);
   return pos;
}

void CTimeLineView::addInterval(quint64 start, quint64 duration, quint64 period)
{
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      interval.m_hasFocus = false;
      it.setValue(interval);
   }

   QTime starttime = QTime(0, 0, 0).addSecs(start);
   QTime stopTime = starttime.addSecs(duration);
   qreal startPos = timeToPos(starttime);
   qreal stopPos = timeToPos(stopTime);

   TimeInterval interval;
   interval.m_waveData.setType(STATIC);
   interval.m_waveData.setPeriodTotMSecs(period * 1000);
   interval.m_waveData.setHasCustomInterval(true);
   interval.m_position_start = startPos;
   interval.m_position_stop = stopPos;
   interval.m_hasFocus = true;
   interval.m_useFreq = true;

   int newkey = m_intervals.size();
   while (m_intervals.contains(newkey))
   {
      newkey++;
   }
   m_intervals.insert(newkey, interval);
}

void CTimeLineView::addInterval(const SautoModel &data)
{
   QMutableHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      interval.m_hasFocus = false;
      it.setValue(interval);
   }

   QTime starttime = QTime(0, 0, 0).addMSecs(data.getStartTimeMSec());
   QTime stopTime = starttime.addMSecs(data.getDuration());
   qreal startPos = timeToPos(starttime);
   qreal stopPos = timeToPos(stopTime);

   TimeInterval interval;
   interval.m_waveData = data;
   interval.m_waveData.setHasCustomInterval(true);
   interval.m_position_start = startPos;
   interval.m_position_stop = stopPos;
   interval.m_hasFocus = true;
   interval.m_useFreq = true;

   int newkey = m_intervals.size();
   while (m_intervals.contains(newkey))
   {
      newkey++;
   }
   m_intervals.insert(newkey, interval);
}

INTERVAL_LIST CTimeLineView::makeTimeIntervals()
{
   INTERVAL_LIST intervals;
   QHashIterator<int, TimeInterval> it(m_intervals);
   while (it.hasNext())
   {
      it.next();
      TimeInterval interval = it.value();
      quint64 startS = ((interval.m_position_start - viewRect.topLeft().x()) * secsPerPixel) * 1000;
      quint64 stopS = ((interval.m_position_stop - viewRect.topLeft().x()) * secsPerPixel) * 1000;
      interval.m_waveData.setStartTimeMSecs(startS);
      interval.m_waveData.setDuration(stopS - startS);
      intervals << interval.m_waveData;
   }
   return intervals;
}

void CTimeLineView::setData(const INTERVAL_LIST &intervals)
{
   clearAll();
   for(int i=0; i<intervals.size(); i++)
   {
      addInterval(intervals.at(i));
   }
   update();
}

void CTimeLineView::clearAll()
{
   m_intervals.clear();
   update();
}

CTimeIntervalWidget::CTimeIntervalWidget(QWidget *parent)
   :QDialog(parent) ,
    m_intervalType(WAVELET),
    m_weekDay(NO_SPECIFIC_DAY),
    m_addButton(0),
    m_removeButton(0),
    m_okButton(0),
    m_cancelButton(0),
    m_makeIntervalRadio(0),
    m_makeSingleshotRadio(0),
    m_startEdit(0),
    m_stopEdit(0),
    m_timeLine(0)
{
   m_data.clear();
   m_start = QTime(0,0,0);
   m_stop  = QTime(1,0,0);
   this->setMaximumWidth(800);
   this->setMinimumWidth(800);
   this->setMaximumHeight(200);
   this->setMinimumHeight(200);
}

CTimeIntervalWidget::~CTimeIntervalWidget()
{

}

void CTimeIntervalWidget::init()
{
   initGUI();
}

void CTimeIntervalWidget::setData(const INTERVAL_LIST &intervals)
{
   m_data = intervals;
   m_timeLine->setData(intervals);
}

void CTimeIntervalWidget::cancel()
{
   QTime midnight(0,0,0);
   if(m_data.size() == 0)
   {
      m_start = QTime(0,0,0);
      m_stop  = QTime(1,0,0);
   }
   else
   {
      m_start = midnight.addMSecs(m_data.last().getPeriodTotMSec());
      m_stop  = midnight.addMSecs(m_data.last().getPeriodTotMSec() + m_data.last().getDuration());
   }

   m_startEdit->setTime(m_start);
   m_stopEdit->setTime(m_stop);

   setData(m_data);
}

void CTimeIntervalWidget::initGUI()
{
   m_startEdit = new QTimeEdit(this);
   m_stopEdit = new QTimeEdit(this);
   m_stopEdit->setTime(m_stop);

   QLabel *startLabel = new QLabel(this);
   startLabel->setText(tr("Start"));

   QLabel *stopLabel = new QLabel(this);
   stopLabel->setText(tr("Stop"));

   QVBoxLayout *startLay = new QVBoxLayout;
   startLay->addWidget(startLabel);
   startLay->addWidget(m_startEdit);

   QVBoxLayout *stopLay = new QVBoxLayout;
   stopLay->addWidget(stopLabel);
   stopLay->addWidget(m_stopEdit);

   m_addButton = new QPushButton(this);
   m_addButton->setText(tr("Add"));
   m_removeButton = new QPushButton(this);
   m_removeButton->setText(tr("Remove"));

   m_okButton = new QPushButton(this);
   m_okButton->setText(tr("OK"));

   connect(m_okButton, SIGNAL(clicked()), 
      this, SLOT(OKClicked()));

   m_cancelButton = new QPushButton(this);
   m_cancelButton->setText(tr("Cancel"));

   connect(m_cancelButton, SIGNAL(clicked()), 
      this, SLOT(cancelClicked()));

   m_removeButton->setEnabled(false);

   connect(m_addButton, SIGNAL(clicked()), 
      this, SLOT(addClicked()));

   connect(m_removeButton, SIGNAL(clicked()), 
      this, SLOT(removeClicked()));

   // radios
   QButtonGroup *radioGroup = new QButtonGroup(this);
   m_makeIntervalRadio = new QRadioButton(tr("Interval"), this);
   m_makeIntervalRadio->setChecked(true);
   m_makeSingleshotRadio = new QRadioButton(tr("SingleShot"), this);
   m_makeSingleshotRadio->setChecked(false);
   radioGroup->addButton(m_makeIntervalRadio, 0);
   radioGroup->addButton(m_makeSingleshotRadio, 1);

   connect(radioGroup, SIGNAL(buttonClicked(int)), 
      this, SLOT(radioButtonClicked(int)));

   QVBoxLayout *radioLay = new QVBoxLayout;
   radioLay->addWidget(m_makeIntervalRadio);
   radioLay->addWidget(m_makeSingleshotRadio);

   QGridLayout *addRemLay = new QGridLayout;
   addRemLay->addWidget(m_addButton, 0, 0);
   addRemLay->addWidget(m_removeButton, 0, 1);
   addRemLay->addWidget(m_okButton, 1, 0);
   addRemLay->addWidget(m_cancelButton, 1, 1);
   addRemLay->addLayout(radioLay, 0, 2, 3, 1);

   QHBoxLayout *startStopLay = new QHBoxLayout;
   startStopLay->addLayout(startLay);
   startStopLay->addStretch();
   startStopLay->addLayout(addRemLay);
   startStopLay->addStretch();
   startStopLay->addLayout(stopLay);

   m_timeLine = new CTimeLineView(this);

   connect(this, SIGNAL(setIntervalType(timeStuff::EIntervalType)),
      m_timeLine, SLOT(receiveIntervalType(timeStuff::EIntervalType)));

   connect(this, SIGNAL(addSingleShot(const QTime &)),
      m_timeLine, SLOT(addSingleShot(const QTime &)));

   connect(this, SIGNAL(addInterval(quint64, quint64, EIntervalType)),
      m_timeLine, SLOT(addInterval(quint64, quint64, EIntervalType)));

   connect(m_timeLine, SIGNAL(timeChangeSig(qreal, qreal)),
      this, SLOT(timeChange(qreal, qreal)));

   connect(this, SIGNAL(removeFocusTime()),
      m_timeLine, SLOT(removeFocusTime()));

   connect(this, SIGNAL(changeStartTime(int)),
      m_timeLine, SLOT(changeStartTime(int)));

   connect(this, SIGNAL(changeStopTime(int)),
      m_timeLine, SLOT(changeStopTime(int)));

   connect(this, SIGNAL(changeSingleshot(int)),
      m_timeLine, SLOT(changeSingleshot(int)));

   QVBoxLayout *totalLayout = new QVBoxLayout;
   totalLayout->addLayout(startStopLay);
   totalLayout->addWidget(m_timeLine);

   emit setIntervalType(m_intervalType);
   this->setLayout(totalLayout);
}

void CTimeIntervalWidget::radioButtonClicked(int id)
{
   disconnect(m_startEdit, SIGNAL(timeChanged(const QTime &)),
      this, SLOT(startEditChanged(const QTime &)));

   disconnect(m_stopEdit, SIGNAL(timeChanged(const QTime &)),
      this, SLOT(stopEditChanged(const QTime &)));

   if (id == 0)
   {
      m_intervalType = STATIC;
   }
   else
   {
      m_intervalType = SINGLE;
   }

   if (m_intervalType == STATIC)
   {
      m_stopEdit->setEnabled(true);
      m_startEdit->setTime(m_start);
   }
   else
   {
      m_stopEdit->setEnabled(false);
      m_startEdit->setTime(m_singleShotTime);
   }

   emit setIntervalType(m_intervalType);

   connect(m_startEdit, SIGNAL(timeChanged(const QTime &)),
      this, SLOT(startEditChanged(const QTime &)));

   connect(m_stopEdit, SIGNAL(timeChanged(const QTime &)),
      this, SLOT(stopEditChanged(const QTime &)));
}

void CTimeIntervalWidget::OKClicked()
{
   m_data = m_timeLine->makeTimeIntervals();
   if(m_weekDay == NO_SPECIFIC_DAY)
   {
      emit timeIntervals(m_data);
   }
   else
   {
      emit timeIntervals(m_data, m_weekDay);
   }
}

void CTimeIntervalWidget::cancelClicked()
{
   emit cancelled();
   emit closeMe();
}

void CTimeIntervalWidget::timeChange(qreal start, qreal stop)
{
   disconnect(m_startEdit, SIGNAL(timeChanged(const QTime &)),
      this, SLOT(startEditChanged(const QTime &)));

   disconnect(m_stopEdit, SIGNAL(timeChanged(const QTime &)),
      this, SLOT(stopEditChanged(const QTime &)));

   QTime midnight(0, 0, 0);
   int startFloor = qRound(start);

   if (m_makeIntervalRadio->isChecked())
   {
      int stopFloor = qRound(stop);
      m_stop = midnight.addSecs(stopFloor);
      m_start = midnight.addSecs(startFloor);
      m_startEdit->setTime(m_start);
      m_stopEdit->setTime(m_stop);
   }
   else
   {
      m_singleShotTime = midnight.addSecs(startFloor);
      m_startEdit->setTime(m_singleShotTime);
   }

   connect(m_startEdit, SIGNAL(timeChanged(const QTime &)),
      this, SLOT(startEditChanged(const QTime &)));

   connect(m_stopEdit, SIGNAL(timeChanged(const QTime &)),
      this, SLOT(stopEditChanged(const QTime &)));
}

void CTimeIntervalWidget::startEditChanged(const QTime &time)
{
   if (time >= m_stop && !m_makeSingleshotRadio->isChecked())
   {
      m_startEdit->setTime(m_start);
      return;
   }

   int newVal = 0;
   newVal += time.hour() * secsPer_Hour;
   newVal += time.minute() * secsPer_Min;
   newVal += time.second();

   if (m_makeIntervalRadio->isChecked())
   {
      m_start = time;
      emit changeStartTime(newVal);
   }
   else
   {
      m_singleShotTime = time;
      emit changeSingleshot(newVal);
   }
}

void CTimeIntervalWidget::stopEditChanged (const QTime &time)
{
   if(time <= m_start)
   {
      m_stopEdit->setTime(m_stop);
      return;
   }

   m_stop = time;

   int newVal = 0;
   newVal += m_stop.hour() * secsPer_Hour;
   newVal += m_stop.minute() * secsPer_Min;
   newVal += m_stop.second();
   emit changeStopTime(newVal);
}

void CTimeIntervalWidget::addClicked()
{
   disconnect(m_startEdit, SIGNAL(timeChanged(const QTime &)), 
      this, SLOT(startEditChanged(const QTime &)));

   disconnect(m_stopEdit , SIGNAL(timeChanged(const QTime &)), 
      this, SLOT(stopEditChanged (const QTime &)));

   if(m_intervalType == WAVELET || m_intervalType == STATIC)
   {
      m_removeButton->setEnabled(true);
      m_start = m_startEdit->time();
      m_stop  = m_stopEdit->time();
      estimateNewInterval(m_start, m_stop);
      m_startEdit->setTime(m_start);
      m_stopEdit ->setTime(m_stop);
      quint64 start =  m_start.hour() * secsPer_Hour + m_start.minute() * secsPer_Min + m_start.second();
      quint64 duration = (m_stop .hour() * secsPer_Hour + m_stop .minute() * secsPer_Min + m_stop .second()) - start;
      emit addInterval(start, duration, m_intervalType);
   }
   else if(m_intervalType == SINGLE)
   {
      m_singleShotTime = m_startEdit->time();
      estimateSingleshot(m_singleShotTime);
      emit addSingleShot(m_singleShotTime);
   }

   connect(m_startEdit, SIGNAL(timeChanged(const QTime &)), 
      this, SLOT(startEditChanged(const QTime &)));

   connect(m_stopEdit , SIGNAL(timeChanged(const QTime &)), 
      this, SLOT(stopEditChanged (const QTime &)));
}

void CTimeIntervalWidget::estimateSingleshot(QTime &time)
{
   int maxIterations = 48;
   INTERVAL_LIST intervals = m_timeLine->makeTimeIntervals();
   bool hasTurned = false;
   bool movingForward = true;
   for(int i=0; i<intervals.size(); i++)
   {
      SautoModel interval = intervals.at(i);
      QTime tmpStart(0,0,0);
      QTime tmpStop (0,0,0);
      tmpStart.addMSecs(interval.getStartTimeMSec());
      tmpStop .addMSecs(interval.getStartTimeMSec() + interval.getDuration());
      int count = 0;
      while(!(time < tmpStart && time > tmpStop) )
      {
         if(count > maxIterations)
         {
            break;
         }
         count++;

         if(time .addSecs(30*60) < QTime(23,59,59) && movingForward)
         {
            time = time.addSecs(30*60);
         }
         else if(!hasTurned)
         {
            hasTurned     = true ;
            movingForward = false;
         }
         else if(time .addSecs( -30*60) > QTime(0,31,0) && !movingForward)
         {
            time = time.addSecs( -30*60);
         }
         else
         {
            break;
         }
      }
   }
}

void CTimeIntervalWidget::estimateNewInterval(QTime &start, QTime &stop)
{
   int maxIterations = 48;
   stop = start.addSecs(60 * 60);
   INTERVAL_LIST intervals = m_timeLine->makeTimeIntervals();
   bool hasTurned = false;
   bool movingForward = true;
   for (int i = 0; i < intervals.size(); i++)
   {
      SautoModel interval = intervals.at(i);
      QTime tmpStart(0, 0, 0);
      QTime tmpStop(0, 0, 0);
      tmpStart.addMSecs(interval.getStartTimeMSec());
      tmpStop.addMSecs(interval.getStartTimeMSec() + interval.getDuration());

      int count = 0;
      while (!((start < tmpStart && start > tmpStop) ||
         (stop  < tmpStart && stop  > tmpStop)))
      {
         if (count > maxIterations)
         {
            break;
         }
         count++;

         if (start.addSecs(30 * 60) < QTime(23, 59, 59) &&
            stop.addSecs(30 * 60) < QTime(23, 59, 59) && movingForward)
         {
            start = start.addSecs(30 * 60);
            stop = stop.addSecs(30 * 60);
         }
         else if (!hasTurned)
         {
            hasTurned = true;
            movingForward = false;
         }
         else if (start.addSecs(-30 * 60) > QTime(0, 31, 0) &&
            stop.addSecs(-30 * 60) > QTime(0, 31, 0) && !movingForward)
         {
            start = start.addSecs(-30 * 60);
            stop = stop.addSecs(-30 * 60);
         }
         else
         {
            break;
         }
      }
   }

   if (start > stop)
   {
      QTime tmpTime = start;
      start = stop;
      stop = tmpTime;
   }

   while ((stop.hour() - start.hour()) > 3)
   {
      start = start.addSecs(5 * 30);
   }
}

void CTimeIntervalWidget::removeClicked()
{
   emit removeFocusTime();
}

void CTimeIntervalWidget::setMode(EIntervalType type)
{
   m_intervalType = type;
   if (m_intervalType == WAVELET || m_intervalType == STATIC)
   {
      m_makeIntervalRadio->setChecked(true);
      radioButtonClicked(0);
   }
   else
   {
      m_makeSingleshotRadio->setChecked(true);
      radioButtonClicked(1);
   }
}

void CTimeIntervalWidget::setDayOfWeek(DAYS dayOfWeek)
{
   m_weekDay = dayOfWeek;
}

void CTimeIntervalWidget::updateWidgetContent()
{
   m_timeLine->updateWidgetContent();
}

void CTimeLineView::updateWidgetContent()
{
   freqWidget->updateWidgetContent();
}