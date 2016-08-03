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
//  \file      sautoDefs.cpp
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

// local includes
#include "sautoDefs.h"

using namespace sauto;

WEEK_DEF sauto::makeEmptyWeekData()
{
   WEEK_DEF data;
   DAY_OF_WEEK_DEF emptyDay;
   INTERVAL_LIST emptyInterval;
   emptyDay.first.first = true;
   emptyDay.first.second = true;
   emptyDay.second = emptyInterval;
   data.insert(MONDAY, emptyDay);
   data.insert(TUESDAY, emptyDay);
   data.insert(WEDNESDAY, emptyDay);
   data.insert(THURSDAY, emptyDay);
   data.insert(FRIDAY, emptyDay);
   data.insert(SATRUDAY, emptyDay);
   data.insert(SUNDAY, emptyDay);
   return data;
}

QPair<int, QString> sauto::findNextMonthInt(const MONTH_DEF &month)
{
   QMap<int, QString> sortedList;
   QPair<int, QString> returnMe;
   returnMe.first = 0;
   returnMe.second = "";

   MONTH_ITERATOR it(month);
   while (it.hasNext())
   {
      it.next();
      sortedList.insert(getMonthAsInt(it.key()), it.key());
   }

   QMapIterator<int, QString> returnIt(sortedList);
   if (returnIt.hasNext())
   {
      returnIt.next();
      returnMe.first = returnIt.key();
      returnMe.second = returnIt.value();
   }

   return returnMe;
}

//  Check if there are any future sessions in the argument date, for the argument
//  intervals
bool sauto::dateHasSessionInFront(const QDate &date, const INTERVAL_LIST &interval)
{
   const QDateTime now = QDateTime::currentDateTime();
   if (now.date() > date)
   {
      // the argument date is in the past, no need the check the intervals
      return false;
   }

   if (interval.size() == 0)
   {
      // there are no intervals, assume session startup is at midnight
      if (now.date() <= date)
      {
         return true;
      }
      return false;
   }

   // the interval list is not empty, check them
   INTERVAL_ITERATOR it(interval);
   while (it.hasNext())
   {
      SautoModel def = it.next();
      quint64 startMs = def.getStartTimeMSec();
      QTime time = QTime(0, 0, 0).addMSecs(startMs);
      if (QDateTime(date, time) >= now)
      {
         // the current interval is in the future
         return true;
      }
   }
   return false;
}

DAYS sauto::getDayStringAsID(const QString &dayStr)
{
   DAYS day = NO_SPECIFIC_DAY;
   QMap<DAYS, QString> week = makeWeek();
   QMapIterator<DAYS, QString> it(week);
   while (it.hasNext())
   {
      it.next();
      if (it.value() == dayStr)
      {
         day = it.key();
         break;
      }
   }
   return day;
}

MONTH_ID sauto::getMonthStringAsID(const QString &monthName)
{
   QMap<MONTH_ID, QString> year = makeYear();
   QMapIterator<MONTH_ID, QString> it(year);
   while (it.hasNext())
   {
      it.next();
      if (it.value() == monthName)
      {
         return it.key();
      }
   }
   return NO_SPECIFIC_MONTH;
}

QString sauto::getDayIDasString(DAYS day)
{
   if (day == NO_SPECIFIC_DAY)
   {
      return "NO_SPECIFIC_DAY";
   }
   return makeWeek().value(day);
}