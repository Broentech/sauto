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
//  \file      sauto.h
//
//  \brief     Definition of a class to handle time and timeouts
//
//  \author    Stian Broen
//
//  \date      01.08.2012
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

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

// Qt includes
#include <QObject>
#include <QString>
#include <QTimer>

// solution includes
#include <sautoModel/sautoDefs.h>

namespace sauto {
   class Sauto : public QObject
   {
      Q_OBJECT

   public:
      explicit Sauto(QObject *parent = 0);
      ~Sauto();
      void init(int id, const SautoModel &def_frequency, const INTERVAL_LIST &def_intervals, const WEEK_DEF &def_week, const CALENDAR_DEF  &def_calendar);

   public slots:
      void stopClock(int id);
      void pauseClock(int id);
      void startClock(int id);

   signals:
      void endReport(int id, const QString &str);
      void constantIntervals(int id);
      void flushAll (int id);
      void triggered(int id);
      void triggered(int clockId, const QString &taskID);
      void timeToNextSession(int id, quint64 msecsLeft, quint64 msecsStarted, const QString &msg);
      void timeLeft(int id, quint64 msecsLeft, quint64 msecsStarted);
      void timeToNextTrigger(int id, quint64 msecsLeft, quint64 msecsStarted);

   private slots:
      void timeout();

   private: 
      void measureAccuracy();
      void analyzeDatasets();
      void handleTimeDelay(qint64 msecs);
      void inSession();
      void outOfSession();
      void calculateTime_Session();
      bool calculateTime_Calendar(CALENDAR_DEF &calendar);
      bool calculateTime_Month(MONTH_DEF &months, const QString &name, int year);
      bool calculateTime_Week(WEEK_DEF &week, int year = -1, int month = -1);
      bool calculateTime_Day(const QDate &date, bool  inherited , INTERVAL_LIST &interval);
      bool calculateTime_Intervals(INTERVAL_LIST &interval, bool ignoreCurrentTime = false, bool lookTomorrow = false);
      bool calculateTime_Frequency(SautoModel  &freq, bool ignoreCurrentTime = false);
      void calculateTime_Trigger(SautoModel &freq);
      void onTrigger();
      void onHasDuration();

   private:
      EEventType m_eventType;
      EWavePoint m_wp;
      int m_clockCD;
      int m_clockAdjustInterv;
      qint64 m_clockDiff;
      qint64 m_clockStart;
      qint64 m_clockStop;
      int m_clockCooldown;
      int m_id;
      CALENDAR_DEF m_default_calendar;
      WEEK_DEF m_default_week;
      INTERVAL_LIST m_default_intervals;
      SautoModel m_default_freq;
      SautoModel m_current_freq;
      bool isSingleSession;
      bool isInSession;
      QTimer *m_timer;
      bool hasNextSessionTime;
      qint64 msecsToNextSession_original;
      qint64 msecsToNextSession;
      bool hasNextTriggerTime;
      qint64 msecsToNextTrigger_original;
      qint64 msecsToNextTrigger;
      bool hasDuration;
      qint64 msecsDuration_original;
      qint64 msecsTimeLeft;
      qint64 msecEpoch_sessionStartTime;
      quint64 msecLastTrigger;
   };

   EEventType intervalType_to_eventType(EIntervalType intervalType);
   EWavePoint nextWp(EWavePoint wp);
}
#endif