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
//  \file      multiDateCalendar.h
//
//  \brief     Definition of widget to specify multiple dates in a calendar
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

#ifndef _MULTI_DATE_CAL_H
#define _MULTI_DATE_CAL_H

// Qt includes
#include <QCalendarWidget>
#include <QPixmap>

// local includes
#include "timeIntervals.h"

// Qt forward declarations
class QPainter;
class QPushButton;
class QToolButton;
class QGroupBox;

namespace sauto {
   enum EDateState
   {
      UNDEFINED_DATESTATE,
      INACTIVE_DATESTATE,
      ACTIVE_BUT_NOT_MARKED,
      ACTIVE_AND_MARKED,
   };

   class CMultiDateCalendar : public QCalendarWidget
   {
      Q_OBJECT

   public:
      explicit CMultiDateCalendar(QWidget *parent = 0);
      ~CMultiDateCalendar();
      void clearMe();
      void selectDate(const QDate &date);
      QList<QPair<QDate, int>> getSelectedDates();
      QList<QDate> getMarkedDays();
      void selectAllDates();
      QList<int> setMarkedToInherit();
      void setMarkedToInterval(const INTERVAL_LIST &intervals, QList<QDate> markedDays);

   signals:
      void dateState(int day, EDateState state);

   protected:
      void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;

   private slots:
      void cellClicked(const QDate &date);

   private:
      typedef QPair<QDate, int> DATE_INDEX; // index to interval hasIntervalList map (should be same as the day of the month)
      typedef QPair<bool, bool> TOGGLED_MARKED;
      typedef QPair<DATE_INDEX, TOGGLED_MARKED> SELECTABLE_DAY_DEF;

      typedef QMap<int, SELECTABLE_DAY_DEF> DAYMAP_DEF;
      typedef QMapIterator<int, SELECTABLE_DAY_DEF> DAYMAP_ITERATOR;
      typedef QMutableMapIterator<int, SELECTABLE_DAY_DEF> DAYMAP_ITERATOR_MUT;
      DAYMAP_DEF m_selections;
      QMap<int, bool> hasIntervalList;
   };

   class CMultiDateCalendarWidget : public QWidget
   {
      Q_OBJECT

   public:
      explicit CMultiDateCalendarWidget(QWidget *parent = 0);
      ~CMultiDateCalendarWidget();
      void setValidDateRange(const QDate &start, const QDate &stop);
      void cancel();
      void reset();
      void setData(QMap<int, CALENDAR_DATE> days);
      QMap<int, CALENDAR_DATE> getMonthDescription();
      void updateWidgetContent();

   signals:
      void dataSignal(const QMap<int, CALENDAR_DATE> &data);

   private slots:
      void inheritIntervalClicked();
      void OKClicked();
      void cancelClicked();
      void clearClicked();
      void receiveTimeInterval(const INTERVAL_LIST &intervals);
      void dateState(int day, EDateState state);

   private:
      void initGUI();
      void updateMonthDescription();

   private:
      CMultiDateCalendar *m_calendar;
      QPushButton *m_OKButton;
      QPushButton *m_cancelButton;
      QPushButton *m_clearSelection;
      QToolButton *m_inheritTimeIntervalAction;
      CTimeIntervalWidget *m_timeIntervals;
      QGroupBox *m_intervalBox;
      QMap<int, CALENDAR_DATE> m_monthDescription;
      QMap<int, INTERVAL_LIST> m_intervals;
   };
}

#endif