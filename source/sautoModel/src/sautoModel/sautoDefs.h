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
//  \file      sautoDefs.h
//
//  \brief     Definition of calendar models
//
//  \author    Stian Broen
//
//  \date      18.02.2013
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

#ifndef _SCHEDULE_TIME_CALENDAR_H_
#define _SCHEDULE_TIME_CALENDAR_H_

// Qt includes
#include <QList>
#include <QMap>
#include <QPair>

// local includes
#include "sautoModel.h"

namespace sauto {

   // interval definition : this defines a list of pairs, where the first value is a to-from
   // time interval, and the second value is a time value representing frequency

   typedef QList <SautoModel> INTERVAL_LIST;
   typedef QListIterator <SautoModel> INTERVAL_ITERATOR;
   typedef QMutableListIterator <SautoModel> INTERVAL_ITERATOR_MUTABLE;

   // week definition : this defines a map containing the days of the week as key, and the value being a pair where the first
   // value is a bool representing whether or not the day is active, and the second value being another pair where the first
   // value is a bool representing whether or not the day shall inherit interval definition, and the second value being a custom interval

   typedef QPair<bool, bool> DAY_TOGGLE; // toggle day, toggle time inherit
   typedef QPair<DAY_TOGGLE, INTERVAL_LIST> DAY_OF_WEEK_DEF; // toggle day, toggle time inherit, list of interval
   inline bool dayIsToggled(const DAY_OF_WEEK_DEF &day) { return day.first.first; }
   inline bool dayInheritsTime(const DAY_OF_WEEK_DEF &day){ return day.first.second; }

   typedef QMap <int, DAY_OF_WEEK_DEF> WEEK_DEF;
   typedef QMapIterator <int, DAY_OF_WEEK_DEF> WEEK_ITERATOR;
   typedef QMutableMapIterator <int, DAY_OF_WEEK_DEF> MUT_WEEK_ITERATOR;

   // month definition : this defines a hash table with all the months of a year, where the key is a string indicating the name of the month,
   // and the value being a pair where the first value is a bool indicating whether or not the month is active, and the second value being a map
   // with all the days of this month : the key is the date-day, and the value is a pair where the first value is a QDate object and the second value
   // being another pair where the first value is a bool indicating whether or not the day is active and the second value being an interval. Note that if
   // the interval is empty, this is an indication that this day should inherit the interval definition

   typedef QPair < QDate, bool > DATE_TOGGLE; // date, toggle inherit
   typedef QPair < DATE_TOGGLE, INTERVAL_LIST> CALENDAR_DATE; //  list of intervals
   typedef QPair<bool, QMap<int, CALENDAR_DATE> > DAY_OF_MONTH_DEF; // inherit from weekdays, day number, calendar_date
   inline bool calendarDateInheritsTime(const CALENDAR_DATE &date) { return date.first.second; }
   inline QDate calendarDate_QDate(const CALENDAR_DATE &date) { return date.first.first; }

   typedef QHash < QString, DAY_OF_MONTH_DEF> MONTH_DEF;
   typedef QHashIterator < QString, DAY_OF_MONTH_DEF> MONTH_ITERATOR;

   // year definition : this defines a map with years, where the key is an integer representing the year and the value
   // is a pair where the first value is a bool indicating whether or not the year is active and the second value is
   // a months definition that defines all the months of the key-year
   typedef QMap<int, // ----------- : Map with year as key and
      QPair<bool, // --------- : Pair of "active year" and hash of months as value
      MONTH_DEF
      > > CALENDAR_DEF;

   typedef QMapIterator<int,
      QPair<bool,
      MONTH_DEF
      > > CALENDAR_ITERATOR;

   WEEK_DEF makeEmptyWeekData();
   QPair<int, QString> findNextMonthInt(const MONTH_DEF &month);
   bool dateHasSessionInFront(const QDate &date, const INTERVAL_LIST &interval);
   DAYS getDayStringAsID(const QString &dayStr);
   MONTH_ID getMonthStringAsID(const QString &monthName);
   QString getDayIDasString(DAYS day);
}

#endif