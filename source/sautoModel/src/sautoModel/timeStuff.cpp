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
//  \file      timeStuff.cpp
//
//  \brief     Module for generic time related definitions and functions
//
//  \author    Stian Broen
//
//  \date      02.04.2013
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
#include <QtCore/qmath.h>
#include <QStringList>
#include <QMetaType>

// local includes
#include "timeStuff.h"

using namespace sauto;
Q_DECLARE_METATYPE(MONTH_ID)
Q_DECLARE_METATYPE(DAYS)

const QMap<DAYS, QString>& sauto::makeWeek()
{
   static QMap<DAYS, QString> week;
   if (week.count() == 0)
   {
      week.insert(MONDAY, MON_STR);
      week.insert(TUESDAY, TUE_STR);
      week.insert(WEDNESDAY, WED_STR);
      week.insert(THURSDAY, THU_STR);
      week.insert(FRIDAY, FRI_STR);
      week.insert(SATRUDAY, SAT_STR);
      week.insert(SUNDAY, SUN_STR);
   }
   return week;
}

const QMap<MONTH_ID, QString>& sauto::makeYear()
{
   static QMap<MONTH_ID, QString> year;
   if (year.count() == 0)
   {
      year.insert(JANUARY, QString(JAN_STR));
      year.insert(FEBRUARY, QString(FEB_STR));
      year.insert(MARCH, QString(MAR_STR));
      year.insert(APRIL, QString(APR_STR));
      year.insert(MAY, QString(MAY_STR));
      year.insert(JUNE, QString(JUN_STR));
      year.insert(JULY, QString(JUL_STR));
      year.insert(AUGUST, QString(AUG_STR));
      year.insert(SEPTEMBER, QString(SEP_STR));
      year.insert(OKTOBER, QString(OKT_STR));
      year.insert(NOVEMBER, QString(NOV_STR));
      year.insert(DECEMBER, QString(DEC_STR));
   }
   return year;
}

bool sauto::dateIsValid(const QDate &date)
{
   int year  = date.year();
   int month = date.month();
   int day   = date.day();

   if (!date.isValid() ||
      (year > 2050 || year < 0) ||
      (month > 12 || month < 0) ||
      (day   > 31 || day < 0))
   {
      return false;
   }
   return true;
}

qint64 sauto::msecsToTomorrow()
{
   QDateTime tomorrow(QDateTime::currentDateTime().addDays(1).date(), QTime(0,0,0,0));
   return tomorrow.toMSecsSinceEpoch() - QDateTime::currentMSecsSinceEpoch();
}

int sauto::wholeDaysUntilEpochMS(quint64 msecs, bool &ok)
{
   ok = true;
   qint64 cleanMs = msecs - QDateTime::currentMSecsSinceEpoch() + sauto::msecsToTomorrow();
   if(0 > cleanMs)
   {
      ok = false;
      return -1;
   }
   return qFloor(static_cast<qreal>(cleanMs) / static_cast<qreal>(msecsPer_Day));
}

int sauto::daysBetweenDates(const QDate &from, const QDate &to, bool &ok)
{
   ok = true;
   int days = 0;
   int toYear = to.year();
   int fromYear = from.year();
   if (fromYear > toYear)
   {
      // this date is in the past
      ok = false;
      return 0;
   }
   else if (fromYear < toYear)
   {
      // the time to use is not this year, but a year in the future
      int daysLeftOfThisYear = from.daysInYear() - from.dayOfYear();
      int dummyYear = fromYear;
      while (dummyYear < toYear)
      {
         ++dummyYear;
         QDate dummyDate(dummyYear, 1, 1);
         if (dummyYear == toYear)
         {
            days += to.daysInYear() - to.dayOfYear();
         }
         else
         {
            days += dummyDate.daysInYear();
         }
      }
      days += daysLeftOfThisYear;
   }
   else
   {
      days += to.dayOfYear() - from.dayOfYear();
      if (days < 0)
      {
         // this date is in the past
         ok = false;
         return 0;
      }
   }

   return days;
}

qint64 sauto::get_SEC_sinceMidnight(const QTime &time)
{
   return time.hour  () * 60 * 60 +
          time.minute()      * 60 +
          time.second() ;
}

qint64 sauto::get_MSEC_sinceMidnight(const QTime &time)
{
   return (sauto::get_SEC_sinceMidnight(time) * 1000) + time.msec();
}

int sauto::getMonthAsInt(const QString &monthStr)
{
   if(monthStr == QString(JAN_STR))
   {
      return 1;
   }
   else if(monthStr == QString(FEB_STR))
   {
      return 2;
   }
   else if(monthStr == QString(MAR_STR))
   {
      return 3;
   }
   else if(monthStr == QString(APR_STR))
   {
      return 4;
   }
   else if(monthStr == QString(MAY_STR))
   {
      return 5;
   }
   else if(monthStr == QString(JUN_STR))
   {
      return 6;
   }
   else if(monthStr == QString(JUL_STR))
   {
      return 7;
   }
   else if(monthStr == QString(AUG_STR))
   {
      return 8;
   }
   else if(monthStr == QString(SEP_STR))
   {
      return 9;
   }
   else if(monthStr == QString(OKT_STR))
   {
      return 10;
   }
   else if(monthStr == QString(NOV_STR))
   {
      return 11;
   }
   else if(monthStr == QString(DEC_STR))
   {
      return 12;
   }
   else
   {
      // error
      return -1;
   }
}

QString sauto::msecToStr(qint64 msec)
{
   int seconds = qFloor(msec / 1000.0);
   int minutes = qFloor(seconds / 60.0);
   int hours = qFloor(minutes / 60.0);
   int days = qFloor(hours / 24.0);
   qint64 leftOverMSec = msec - (seconds * 1000);
   int clockHours = hours - (days * 24);
   int clockMinutes = minutes - (hours * 60);
   int clockSeconds = seconds - (minutes * 60);

   const QTime timeLeft(clockHours, clockMinutes, clockSeconds, leftOverMSec);
   QString str;
   if (days == 0)
   {
      str = QString("%1").arg(timeLeft.toString("hh:mm:ss:z"));
   }
   else
   {
      str = QString("%1 days, %2").arg(days).arg(timeLeft.toString("hh:mm:ss.z"));
   }
   return str;
}

qint64 sauto::stringTimeToEpoch(const QString &str)
{
   qint64 secs = 0;
   if (!str.contains(" - "))
   {
      // wrong format
      return -1;
   }

   const QStringList dateStr = str.split(" - ").at(0).split('.');
   const QStringList timeStr = str.split(" - ").at(1).split(':');

   if (dateStr.size() != 3 || timeStr.size() != 3)
   {
      // wrong format
      return -1;
   }

   const QDate date(dateStr.at(2).toInt(), dateStr.at(1).toInt(), dateStr.at(0).toInt());
   const QTime time(timeStr.at(0).toInt(), timeStr.at(1).toInt(), timeStr.at(2).toInt());

   secs = QDateTime(date, time).toTime_t();
   return secs;
}

QString sauto::englishToNorwegian_shortMonthName(const QString &englishMonth)
{
   if (englishMonth.toLower() == "may")
   {
      return "Mai";
   }
   if (englishMonth.toLower() == "oct")
   {
      return "Okt";
   }
   if (englishMonth.toLower() == "dec")
   {
      return "Des";
   }
   return englishMonth;
}