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
//  \file      timeStuff.h
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

#ifndef _TIMEDEFS_H
#define _TIMEDEFS_H

// Qt includes
#include <QString>
#include <QMap>
#include <QDateTime>

namespace sauto {

   static const quint64 m_maxTimeletPeriod = (23 * 60 * 60 + 59 * 60 + 59) * 1000;
   static const QString default_task("Unspecified");

   enum MONTH_ID
   {
      NO_SPECIFIC_MONTH,
      JANUARY,
      FEBRUARY,
      MARCH,
      APRIL,
      MAY,
      JUNE,
      JULY,
      AUGUST,
      SEPTEMBER,
      OKTOBER,
      NOVEMBER,
      DECEMBER,
   };

   static const QString JAN_STR ("january");
   static const QString FEB_STR("february");
   static const QString MAR_STR("march");
   static const QString APR_STR("april");
   static const QString MAY_STR("may");
   static const QString JUN_STR("june");
   static const QString JUL_STR("july");
   static const QString AUG_STR("august");
   static const QString SEP_STR("september");
   static const QString OKT_STR("october");
   static const QString NOV_STR("november");
   static const QString DEC_STR("december");

   enum DAYS
   {
      NO_SPECIFIC_DAY,
      MONDAY,
      TUESDAY,
      WEDNESDAY,
      THURSDAY,
      FRIDAY,
      SATRUDAY,
      SUNDAY,
   };

   static const QString MON_STR("monday");
   static const QString TUE_STR("tuesday");
   static const QString WED_STR("wednesday");
   static const QString THU_STR("thursday");
   static const QString FRI_STR("friday");
   static const QString SAT_STR("saturday");
   static const QString SUN_STR("sunday");

   static const int secsPer_Min = 60;
   static const int secsPer_Hour = secsPer_Min * 60;
   static const int secsPer_Day = secsPer_Hour * 24;
   static const int msecsPer_Sec = 1000;
   static const int msecsPer_Min = msecsPer_Sec * secsPer_Min;
   static const int msecsPer_Hour = msecsPer_Sec * secsPer_Hour;
   static const int msecsPer_Day = msecsPer_Sec * secsPer_Day;
   static const int YEARS_IN_CALENDAR = 5;

   static const int CLOCK_COOLDOWN_MSEC = 10;
   static const int CLOCK_ADJUST_INTERV = 1000;

   enum EEventType
   {
      EVENT_UNDECIDED  , // initial mode
      EVENT_SINGLESHOT , // the next trigger will be a singleshot
      EVENT_CONSTFREQ  , // triggering on regular frequency
      EVENT_INTERVAL   , // custom intervals running in sessions
      EVENT_WAVELET    , // alternating events
   };

   enum EIntervalType
   {
      NOT_SPECIFIED,
      WAVELET,
      STATIC,
      SINGLE
   };

   enum INTERVAL_MODES
   { 
      NO_INTERVAL_MODE, 
      NEVER_MODE, 
      ALWAYS_MODE, 
      INTERVAL_MODE 
   };

   enum TIME_MODES
   {
      NO_TIME_MODE,
      ALL_DAY_MODE,
      SPECIFIC_TIME_MODE,
   };

   enum DAY_MODES
   {
      NO_DAY_MODE,
      EVERY_DAY_MODE,
      SPECIFIC_DAY_MODE,
   };

   enum MONTH_MODES
   {
      NO_MONTH_MODE,
      EVERY_MONTH_MODE,
      SPECIFIC_MONTH_MODE,
   };

   enum YEAR_MODES
   {
      NO_YEAR_MODE,
      EVERY_YEAR_MODE,
      YEAR_SPECIFIC_MODE,
   };

   enum EWavePoint
   {
      WP_NOT_SPECIFIED,
      SINKING,
      PEAK,
      RISING,
      VALLEY
   };

   const QMap<DAYS, QString>& makeWeek();
   const QMap<MONTH_ID, QString>& makeYear();
   bool dateIsValid(const QDate &date);
   qint64 msecsToTomorrow();
   int wholeDaysUntilEpochMS(quint64 msecs, bool &ok);
   int daysBetweenDates(const QDate &from, const QDate &to, bool &ok);
   qint64 get_SEC_sinceMidnight(const QTime &time);
   qint64 get_MSEC_sinceMidnight(const QTime &time);
   int getMonthAsInt(const QString &monthStr);
   QString msecToStr(qint64 msec);
   qint64 stringTimeToEpoch(const QString &str);
   QString englishToNorwegian_shortMonthName(const QString &englishMonth);
}

#endif