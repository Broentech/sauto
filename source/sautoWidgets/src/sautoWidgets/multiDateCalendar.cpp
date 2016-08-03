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
//  \file      multiDateCalendar.cpp
//
//  \brief     Implementation of widget to specify multiple dates in a calendar
//
//  \author    Stian Broen
//
//  \date      07.07.2012
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
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QWidgetAction>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

// local includes
#include "multiDateCalendar.h"
#include "timeIntervals.h"

using namespace sauto;

CMultiDateCalendar::CMultiDateCalendar(QWidget *parent)
   : QCalendarWidget(parent)
{
   connect(this, SIGNAL(clicked(const QDate &)),
      this, SLOT(cellClicked(const QDate &)));

   this->setNavigationBarVisible(false);
   this->setToolTip(tr("Click to toggle date on/off, Shift+click to mark."));
   this->setHorizontalHeaderFormat(QCalendarWidget::LongDayNames);
}

CMultiDateCalendar::~CMultiDateCalendar()
{

}

void CMultiDateCalendar::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
   int dateInt = date.toString("yyyyMMdd").toInt();
   int currentSel = this->selectedDate().toString("yyyyMMdd").toInt();
   if (!m_selections.contains(dateInt))
   {
      return;
   }

   DAYMAP_ITERATOR it(m_selections);
   while (it.hasNext())
   {
      it.next();
      if (it.key() != dateInt)
      {
         continue;
      }

      SELECTABLE_DAY_DEF val = it.value();
      QPainter m_paint(painter->device());

      QPen origPen = m_paint.pen();
      if (dateInt == currentSel)
      {
         QPen widerPen;
         widerPen.setWidth(2);
         m_paint.setPen(widerPen);
      }
      m_paint.drawText(rect.translated(QPoint((rect.width() / 2) - 5, rect.height() / 8)), QString("%1").arg(date.day()));
      m_paint.drawRect(rect);
      m_paint.setPen(origPen);

      int  thumbIndex = val.first.second;
      bool dateEnabled = val.second.first;
      bool dateMarked = val.second.second;

      if (dateEnabled && !hasIntervalList.contains(thumbIndex))
      {
         QLinearGradient grad(rect.topLeft(), rect.topRight());
         grad.setColorAt(0, QColor(0, 0, 255, 70));
         grad.setColorAt(1, QColor(0, 0, 55, 90));
         QBrush gradBrush(grad);
         m_paint.fillRect(rect, gradBrush);
      }
      else if (dateEnabled && hasIntervalList.contains(thumbIndex) && hasIntervalList.value(thumbIndex))
      {
         QLinearGradient grad(rect.topLeft(), rect.topRight());
         grad.setColorAt(0, Qt::green);
         grad.setColorAt(1, Qt::white);
         QBrush gradBrush(grad);
         m_paint.fillRect(rect, gradBrush);
         m_paint.drawText(rect.translated(QPoint((rect.width() / 2) - 5, rect.height() / 8)), QString("%1").arg(date.day()));
      }
      else // this date is not enabled
      {
         m_paint.fillRect(rect, QColor(255, 255, 255, 70));
      }

      QRect markRect(QPoint(rect.topRight().x() - 13, rect.topRight().y() + 3),
         QPoint(rect.topRight().x() - 3, rect.topRight().y() + 13));
      int mid = markRect.topLeft().x() + qRound(static_cast<qreal>(markRect.topRight().x() - markRect.topLeft().x()) / 2.0) - 4;
      QPoint midBot(mid, markRect.bottomRight().y());
      m_paint.setRenderHint(QPainter::Antialiasing);
      if (dateMarked)
      {
         QPen pen = m_paint.pen();
         QPen widePen;
         widePen.setWidth(2);
         widePen.setColor(Qt::darkGreen);
         m_paint.setPen(widePen);
         m_paint.drawLine(markRect.topLeft(), midBot);
         m_paint.drawLine(markRect.topRight(), midBot);
         m_paint.setPen(pen);
      }
      return;
   }
}

void CMultiDateCalendar::clearMe()
{
   DAYMAP_ITERATOR_MUT it(m_selections);
   while(it.hasNext())
   {
      it.next();
      SELECTABLE_DAY_DEF val = it.value();
      int day = val.first.first.day();
      val.first.second = day;
      val.second.first = false;
      val.second.second = false;
      it.setValue(val);
   }
   updateCells();
}

void CMultiDateCalendar::selectDate(const QDate &date)
{
   DAYMAP_ITERATOR_MUT it(m_selections);
   while(it.hasNext())
   {
      it.next();
      SELECTABLE_DAY_DEF val = it.value();
      QDate itDate = val.first.first;
      if(itDate == date)
      {
         val.second.first = true;
         it.setValue(val);
         break;
      }
   }
   updateCells();
}

void CMultiDateCalendar::cellClicked(const QDate &date)
{
   bool hasShift = false;
   if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
   {
      hasShift = true;
   }

   int dateInt = date.toString("yyyyMMdd").toInt();
   if(!m_selections.contains(dateInt))
   {
      SELECTABLE_DAY_DEF newPair;
      newPair.first.first = date;
      newPair.second.first = true;
      if(hasShift)
      {
         newPair.second.second = true;
      }
      else
      {
         newPair.second.second = false;
      }
      m_selections.insert(dateInt, newPair);
      updateCells();
      return;
   }
   
   EDateState state;
   DAYMAP_ITERATOR_MUT it(m_selections);
   while(it.hasNext())
   {
      it.next();
      if(it.key() != dateInt)
      {
         continue;
      }

      bool isMark = false;
      bool isToggl = false;

      SELECTABLE_DAY_DEF pair = it.value();
      if(hasShift)
      {
         if(pair.second.first)
         {
            // allow only toggled dates to be checked
            isToggl = true;
            isMark = !pair.second.second;
            pair.second.second = isMark;
         }
         else
         {
            return;
         }
      }
      else
      {
         pair.second.second = false;
         isToggl = !pair.second.first;
         pair.second.first = isToggl;
      }

      if(!isToggl)
      {
         state = INACTIVE_DATESTATE;
      }
      else
      {
         if(isMark)
         {
            state = ACTIVE_AND_MARKED;
         }
         else
         {
            state = ACTIVE_BUT_NOT_MARKED;
         }
      }
      it.setValue(pair);
      break;
   }
   updateCells();
   emit dateState(dateInt, state);
}

void CMultiDateCalendar::selectAllDates()
{
   m_selections.clear();
   QDate start = this->minimumDate();
   QDate stop = this->maximumDate();
   if(!dateIsValid(start) || !dateIsValid(stop))
   {
      QDate now = QDateTime::currentDateTime().date();
      QDate lastInMonth(now.year(), now.month(), now.daysInMonth());
      this->setDateRange(now, lastInMonth);
      start = this->minimumDate();
      stop  = this->maximumDate();
      if(!dateIsValid(start) || !dateIsValid(stop))
      {
         return;
      }
   }

   while(start <= stop)
   {
      QDate selectDate = start;
      QString selectDateStr = selectDate.toString("yyyyMMdd");
      int dateInt = selectDateStr.toInt();
      SELECTABLE_DAY_DEF newPair;
      newPair.first.first = selectDate;
      newPair.second.first = true;
      newPair.second.second = false;
      m_selections.insert(dateInt, newPair);
      start = start.addDays(1);
   }

   updateCells();
}

QList<QPair<QDate, int>> CMultiDateCalendar::getSelectedDates()
{
   QList<QPair<QDate, int>> returnMe;
   DAYMAP_ITERATOR it(m_selections);
   while(it.hasNext())
   {
      it.next();
      SELECTABLE_DAY_DEF val = it.value();
      bool isToggled = val.second.first;
      if(isToggled)
      {
         QPair<QDate, int> toList;
         QDate date = val.first.first;
         int index  = val.first.second;
         toList.first  = date;
         toList.second = index;
         returnMe << toList;
      }
   }

   return returnMe;
}

QList<QDate> CMultiDateCalendar::getMarkedDays()
{
   QList<QDate> returnMe;
   DAYMAP_ITERATOR it(m_selections);
   while(it.hasNext())
   {
      it.next();
      SELECTABLE_DAY_DEF val = it.value();
      bool isMarked = val.second.second;
      if(isMarked)
      {
         QDate date = val.first.first;
         returnMe << date;
      }
   }
   return returnMe;
}

QList<int> CMultiDateCalendar::setMarkedToInherit()
{
   QList<int> returnMe;
   DAYMAP_ITERATOR_MUT it(m_selections);
   while(it.hasNext())
   {
      it.next();
      SELECTABLE_DAY_DEF val = it.value();
      bool isMarked = val.second.second;
      if(isMarked)
      {
         val.first.second = -1;
      }
      returnMe << val.first.second;
      it.setValue(val);
   }
   updateCells();
   return returnMe;
}

void CMultiDateCalendar::setMarkedToInterval(const INTERVAL_LIST &, QList<QDate> markedDays)
{
   for(int i=0; i<markedDays.size(); i++)
   {
      int dayNo = markedDays.at(i).toString("yyyyMMdd").toInt();
      if(!m_selections.contains(dayNo))
      {
         hasIntervalList.insert(dayNo, false);
         continue;
      }

      // update the value
      SELECTABLE_DAY_DEF val = m_selections.value(dayNo);
      val.first.second = dayNo;
      val.second.first = true;
      val.second.second = true; 
      m_selections.insert(dayNo, val);
      hasIntervalList.insert(dayNo, true);
   }

   updateCells();
}

CMultiDateCalendarWidget::CMultiDateCalendarWidget(QWidget *parent)
   :QWidget(parent),
   m_calendar(0),
   m_OKButton(0),
   m_cancelButton(0),
   m_clearSelection(0),
   m_inheritTimeIntervalAction(0),
   m_timeIntervals(0),
   m_intervalBox(0)
{
   initGUI();
}

CMultiDateCalendarWidget::~CMultiDateCalendarWidget()
{

}

void CMultiDateCalendarWidget::initGUI()
{
   m_calendar = new CMultiDateCalendar(this);

   connect(m_calendar, SIGNAL(dateState(int, EDateState)), 
      this, SLOT(dateState(int, EDateState)));

   m_OKButton = new QPushButton(this);
   m_OKButton->setText(tr("OK"));

   connect(m_OKButton, SIGNAL(clicked()), 
      this, SLOT(OKClicked()));

   m_cancelButton = new QPushButton(this);
   m_cancelButton->setText(tr("Cancel"));

   connect(m_cancelButton, SIGNAL(clicked()), 
      this, SLOT(cancelClicked()));

   m_clearSelection = new QPushButton(this);
   m_clearSelection->setText(tr("Clear"));

   connect(m_clearSelection, SIGNAL(clicked()), 
      this, SLOT(clearClicked()));

   m_inheritTimeIntervalAction = new QToolButton(this);
   m_inheritTimeIntervalAction->setText(tr("Inherit Time Intervals"));

   connect(m_inheritTimeIntervalAction, SIGNAL(clicked()), 
      this, SLOT(inheritIntervalClicked()));

   m_timeIntervals = new CTimeIntervalWidget(this);
   m_timeIntervals->init();

   connect(m_timeIntervals, SIGNAL(timeIntervals(const INTERVAL_LIST &)),
      this, SLOT(receiveTimeInterval(const INTERVAL_LIST &)));

   QVBoxLayout *intervalLay = new QVBoxLayout;
   intervalLay->addWidget(m_timeIntervals);
   m_intervalBox = new QGroupBox(this);
   m_intervalBox->setTitle(tr("Shift+Click dates to define intervals"));
   m_intervalBox->setLayout(intervalLay);
   m_intervalBox->setAlignment(Qt::AlignHCenter);
   m_intervalBox->setEnabled(false);

   QHBoxLayout *buttonsLay = new QHBoxLayout;
   buttonsLay->addWidget(m_OKButton);
   buttonsLay->addWidget(m_cancelButton);
   buttonsLay->addWidget(m_clearSelection);
   buttonsLay->addWidget(m_inheritTimeIntervalAction);

   QVBoxLayout *totLay = new QVBoxLayout;
   totLay->addWidget(m_calendar);
   totLay->addLayout(buttonsLay);
   totLay->addWidget(m_intervalBox);
   this->setLayout(totLay);

   OKClicked();
}

void CMultiDateCalendarWidget::inheritIntervalClicked()
{
   QList<int> keys = m_calendar->setMarkedToInherit();
   for(int i=0; i<keys.size(); i++)
   {
      if(m_intervals.contains(keys.at(i)))
      {
         m_intervals.remove(keys.at(i));
      }
   }
}

void CMultiDateCalendarWidget::setValidDateRange(const QDate &start, const QDate &stop)
{
   if(start == QDate() && stop == QDate())
   {
      this->setEnabled(false);
   }
   else
   {
      this->setEnabled(true);
   }

   QDate today = QDateTime::currentDateTime().date();
   QDate useStart = start;
   QDate useStop  = stop;
   if(useStart > useStop)
   {
      QDate tmpDate = useStart;
      useStart = useStop;
      useStop = tmpDate;
   }
   
   if(today >= useStart)
   {
      if(today.year() > useStart.year())
      {
         // add a year
         useStart = useStart.addYears(1);
         useStop  = useStop .addYears(1);
      }
      else if(today.month() == useStart.month())
      {
         // append days
         while(today > useStart && useStart < useStop)
         {
            useStart = useStart.addDays(1);
         }
      }
   }

   m_calendar->setCurrentPage(useStart.year(), useStart.month());
   m_calendar->setMinimumDate(useStart);
   m_calendar->setMaximumDate(useStop);
   m_calendar->selectAllDates();
   updateMonthDescription();
}

void CMultiDateCalendarWidget::setData(QMap<int, CALENDAR_DATE> days)
{
   m_monthDescription = days;
   m_calendar->clearMe();

   QMapIterator<int, CALENDAR_DATE> daysIt (m_monthDescription);
   while(daysIt.hasNext())
   {
      daysIt.next();
      int daysID = daysIt.key();
      CALENDAR_DATE calDate = daysIt.value();
      QDate date = calDate.first.first;
      bool isInherited = calDate.first.second;
      INTERVAL_LIST interv = calDate.second;
      if(!isInherited)
      {
         int intervSize = interv.size();
         if(intervSize > 0)
         {
            // its a custom interval
            QList<QDate> list;
            list << date;
            m_calendar->setMarkedToInterval(interv, list);
            m_timeIntervals->setData(interv);
         }
      }
      m_calendar->selectDate(date);
      m_intervals.insert(daysID, interv);
   }
}

void CMultiDateCalendarWidget::reset()
{
   m_intervals.clear();
   INTERVAL_LIST empty;
   m_timeIntervals->setData(empty);
   m_calendar->clearMe();
   m_calendar->selectAllDates();
}

void CMultiDateCalendarWidget::OKClicked()
{
   updateMonthDescription();
   emit dataSignal(m_monthDescription);
}

void CMultiDateCalendarWidget::updateMonthDescription()
{
   m_monthDescription.clear();
   QList<QPair<QDate, int>> selDates = m_calendar->getSelectedDates();
   for(int i=0; i<selDates.size(); i++)
   {
      QPair<QDate, int> currDate = selDates.at(i);
      int dateKey = currDate.first.toString("yyyyMMdd").toInt();
      CALENDAR_DATE calDate;
      calDate.first.first = currDate.first;
      INTERVAL_LIST tmpIntervals;
      
      if(!m_intervals.contains(currDate.second))
      {
         calDate.first.second = true; // inherit interval from week
         calDate.second = tmpIntervals;
      }
      else
      {
         calDate.first.second = false; // has custom interval, DONT inherit from week
         INTERVAL_LIST interval = m_intervals.value(currDate.second);
         if(interval.size() > 0)
         {
            calDate.second = interval;
         }
      }
      m_monthDescription.insert(dateKey, calDate);
   }
}

void CMultiDateCalendarWidget::cancelClicked()
{
   cancel();
}

void CMultiDateCalendarWidget::cancel()
{
   if(m_monthDescription.size() == 0)
   {
      reset();
   }
   else
   {
      setData(m_monthDescription);
   }
}

QMap<int, CALENDAR_DATE> CMultiDateCalendarWidget::getMonthDescription()
{
   updateMonthDescription();
   return m_monthDescription;
}

void CMultiDateCalendarWidget::clearClicked()
{
   m_intervals.clear();
   m_monthDescription.clear();
   m_calendar->clearMe();
}

void CMultiDateCalendarWidget::receiveTimeInterval(const INTERVAL_LIST &intervals)
{
   QList<QDate> markedDays = m_calendar->getMarkedDays();
   for(int i=0; i<markedDays.size(); i++)
   {
      m_intervals.insert(markedDays.at(i).toString("yyyyMMdd").toInt() ,intervals);
   }
   m_calendar->setMarkedToInterval(intervals, markedDays);
}

void CMultiDateCalendarWidget::dateState(int day, EDateState state)
{
   QList<QDate> markedList = m_calendar->getMarkedDays();
   if (markedList.size() <= 0)
   {
      m_intervalBox->setEnabled(false);
      m_intervalBox->setTitle(tr("Shift+Click dates to define intervals"));
   }
   else
   {
      m_intervalBox->setEnabled(true);
      QString newBoxText = "Intervals for";
      for (int i = 0; i < markedList.size(); i++)
      {
         newBoxText.append(QString(" %1").arg(markedList.at(i).day()));
      }
      m_intervalBox->setTitle(newBoxText);
   }

   if (state != ACTIVE_AND_MARKED || !m_intervals.contains(day))
   {
      INTERVAL_LIST empty;
      m_timeIntervals->setData(empty);
      m_timeIntervals->cancel();
   }
   else
   {
      m_timeIntervals->setData(m_intervals.value(day));
   }
}

void CMultiDateCalendarWidget::updateWidgetContent()
{
   m_timeIntervals->updateWidgetContent();
}