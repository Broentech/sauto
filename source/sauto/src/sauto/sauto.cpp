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
//  \file      sauto.cpp
//
//  \brief     Implementation of a class to handle time and timeouts
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

// Qt includes
#include <QLocale>

// local includes
#include "sauto.h"

using namespace sauto;

Sauto::Sauto(QObject *parent)
   :QObject(parent),
   m_eventType(EVENT_UNDECIDED),
   m_wp(WP_NOT_SPECIFIED),
   m_clockCD(-1),
   m_clockAdjustInterv(CLOCK_ADJUST_INTERV),
   m_clockDiff(0),
   m_clockStart(0),
   m_clockStop(0),
   m_clockCooldown(CLOCK_COOLDOWN_MSEC),
   m_id(-1),
   isSingleSession(false),
   isInSession(false),
   m_timer(0),
   hasNextSessionTime(false),
   msecsToNextSession_original(0),
   msecsToNextSession(0),
   hasNextTriggerTime(false),
   msecsToNextTrigger_original(0),
   msecsToNextTrigger(0),
   hasDuration(false),
   msecsDuration_original(0),
   msecsTimeLeft(0),
   msecEpoch_sessionStartTime(0),
   msecLastTrigger(0)
{
   m_timer = new QTimer(this);
   m_timer->setTimerType(Qt::PreciseTimer);
   m_timer->setSingleShot(false);

   connect(m_timer, SIGNAL(timeout()),
      this, SLOT(timeout()));
}

Sauto::~Sauto()
{

}

void Sauto::init(int id, const SautoModel &def_frequency, const INTERVAL_LIST &def_intervals, const WEEK_DEF &def_week, const CALENDAR_DEF  &def_calendar)
{
   m_id = id;
   m_default_freq = def_frequency;
   m_default_intervals = def_intervals;
   m_default_week = def_week;
   m_default_calendar = def_calendar;
   if(def_frequency.getStartTimeMSec() < 0)
   {
      isSingleSession = true;
   }
}

void Sauto::startClock(int id)
{
   if (m_id == id)
   {
      m_timer->start(m_clockCooldown);
   }
}

void Sauto::stopClock(int id)
{
   if (m_id == id)
   {
      if (m_timer != 0 && m_timer->isActive())
      {
         m_timer->stop();
      }
      this->deleteLater();
   }
}

void Sauto::pauseClock(int id)
{
   if (m_id == id)
   {
      m_timer->stop();
   }
}

void Sauto::handleTimeDelay(qint64 msecs)
{
   // add delay to countdown members
   msecsToNextSession -= msecs;
   msecsToNextTrigger -= msecs;
   msecsTimeLeft      -= msecs;
}

void Sauto::measureAccuracy()
{
   if(m_clockCD == -1)
   {
      // starting condition
      m_clockCD    = m_clockAdjustInterv;
      m_clockStart = QDateTime::currentMSecsSinceEpoch();
   }
   else if(m_clockCD == 0)
   {
      // measure time now
      // this calculation should ideally be 0 msecs each time
      m_clockCD   = m_clockAdjustInterv;
      m_clockStop = QDateTime::currentMSecsSinceEpoch();
      m_clockDiff = (m_clockStop - m_clockStart) - (m_clockAdjustInterv * m_clockCooldown);
      if(m_clockDiff >= m_clockCooldown)
      {
         handleTimeDelay(m_clockDiff);
         m_clockDiff = 0;
      }
      m_clockStart = QDateTime::currentMSecsSinceEpoch();
   }
   else
   {
      m_clockCD--;
   }
}

void Sauto::timeout()
{
   measureAccuracy();
   if (isInSession)
   {
      inSession();
   }
   else
   {
      outOfSession();
   }
}

void Sauto::inSession()
{
   if(!hasNextTriggerTime)
   {
      calculateTime_Trigger(m_current_freq);
      if(!hasNextTriggerTime)
      {
         hasNextSessionTime = false;
         isInSession = false;
         return;
      }
   }

   msecsToNextTrigger -= m_clockCooldown;
   if(msecsToNextTrigger < (0-m_clockCooldown))
   {
      // should never be the case
      hasNextTriggerTime = false;
      return;
   }

   if(msecsToNextTrigger <= 0)
   {
      quint64 msecOnTrigger = QDateTime::currentDateTime().toMSecsSinceEpoch();
      if(msecLastTrigger == 0)
      {
         msecLastTrigger = msecOnTrigger;
      }
      else if(msecOnTrigger < (msecLastTrigger + 100))
      {
         // TODO : Find out why this is sometimes the case
         hasNextTriggerTime = false;
         return;
      }
      else
      {
         msecLastTrigger = msecOnTrigger;
      }
      onTrigger();
   }
   else
   {
      emit timeToNextTrigger(m_id, msecsToNextTrigger, msecsToNextTrigger_original);
   }

   if (m_eventType == EVENT_SINGLESHOT)
   {
      return;
   }

   if(!hasDuration)
   {
      msecsDuration_original = m_current_freq.getDuration();
      msecsTimeLeft = msecsDuration_original;
      hasDuration = true;
      onHasDuration();
   }
   else
   {
      onHasDuration();
   }
}

void Sauto::outOfSession()
{
   if (hasNextSessionTime == false)
   {
      calculateTime_Session();
   }
   else
   {
      msecsToNextSession -= m_clockCooldown;
   }

   if(msecsToNextSession <= 0)
   {
      // the session start just now!
      msecsToNextSession = 0;
      msecEpoch_sessionStartTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
      isInSession = true;
      inSession();
   }
   else
   {
      // session has not yet started, report the time left until it starts
      switch(m_eventType)
      {
      
      case(EVENT_SINGLESHOT):
         emit timeToNextSession(m_id, msecsToNextSession, msecsToNextSession_original, "SingleShot");
         break;
      
      case(EVENT_INTERVAL):
         emit timeToNextSession(m_id, msecsToNextSession, msecsToNextSession_original, "Interval");
         break;
      
      case(EVENT_WAVELET):
         emit timeToNextSession(m_id, msecsToNextSession, msecsToNextSession_original, "Wavelet");
         break;

      case(EVENT_CONSTFREQ):
      case(EVENT_UNDECIDED):
      default:
         emit timeToNextSession(m_id, msecsToNextSession, msecsToNextSession_original, "");
         break;
      }
   }
}

void Sauto::calculateTime_Trigger(SautoModel &freq)
{
   if (!freq.isValid())
   {
      return;
   }

   bool ok;
   EIntervalType type = freq.calculateNextTrigger(msecsToNextTrigger, ok, m_wp);
   if(!ok)
   {
      hasNextTriggerTime = false;
      return;
   }

   switch(type)
   {
   case(WAVELET):
      msecsToNextTrigger_original = qRound(static_cast<qreal>(freq.getPeriodTotMSec()) / 4);
      break;

   case(STATIC):
      msecsToNextTrigger_original = freq.getPeriodTotMSec();
      break;

   case(SINGLE):
      msecsToNextTrigger_original = msecsToNextTrigger;
      break;

   default:
      hasNextTriggerTime = false;
      return;
   }

   m_eventType = intervalType_to_eventType(type);
   hasNextTriggerTime = true;
}

void Sauto::onTrigger()
{
   if(msecsToNextTrigger < (0-m_clockCooldown))
   {
      // should never be the case
      hasNextTriggerTime = false;
      return;
   }

   QString m_currTask = "SKIP";
   switch(m_eventType)
   {
   case(EVENT_SINGLESHOT):
      m_currTask = m_current_freq.getOnPeak();
      emit triggered(m_id, m_currTask);
      // the reason why the process is restarted on trigger if the trigger type is singleshot,
      // is that the cooldown is defined as trigger-cooldown, and not session cool-down, so next trigger time
      // must be recalculated after each trigger
      isInSession        = false;
      hasDuration        = false;
      hasNextSessionTime = false;
      hasNextTriggerTime = false;
      break;

   case(EVENT_CONSTFREQ):
      m_currTask = m_current_freq.getOnPeak();
      emit triggered(m_id, m_currTask);
      msecsToNextTrigger = msecsToNextTrigger_original;
      break;

   case(EVENT_INTERVAL):
      m_currTask = m_current_freq.getOnPeak();
      emit triggered(m_id, m_currTask);
      msecsToNextTrigger = msecsToNextTrigger_original;
      break;

   case(EVENT_WAVELET):
      switch(m_wp)
      {
      case(SINKING):
         if (m_current_freq.hasSinking())
         {
            m_currTask = m_current_freq.getOnSinking();
         }
         break;

      case(PEAK):
         if (m_current_freq.hasPeak())
         {
            m_currTask = m_current_freq.getOnPeak();
         }
         break;

      case(RISING):
         if (m_current_freq.hasRising())
         {
            m_currTask = m_current_freq.getOnRising();
         }
         break;

      case(VALLEY):
         if (m_current_freq.hasValley())
         {
            m_currTask = m_current_freq.getOnValley();
         }
         break;

      default:
         break;
      }

      m_wp = nextWp(m_wp);
      msecsToNextTrigger = msecsToNextTrigger_original;
      if (m_currTask != "SKIP")
      {
         emit triggered(m_id, m_currTask);
      }
      break;

   case(EVENT_UNDECIDED):
      break;
   default:
      break;
   }
}

EWavePoint sauto::nextWp(EWavePoint wp)
{
   switch(wp)
   {
   case(WP_NOT_SPECIFIED): 
      return WP_NOT_SPECIFIED ;
   case(SINKING): 
      return VALLEY;
   case(PEAK): 
      return SINKING;
   case(RISING): 
      return PEAK;
   case(VALLEY): 
      return RISING;
   default: 
      return WP_NOT_SPECIFIED;
   }
}

void Sauto::onHasDuration()
{
   msecsTimeLeft -= m_clockCooldown;
   if(msecsTimeLeft <= 0)
   {
      // this session has ended, find next set of times at next clock timeout
      if(isSingleSession)
      {
         // there are nothing more for this thread to do, report and stop
         stopClock(m_id);
         emit endReport(m_id, "The single session has finished");
         this->deleteLater();
         return;
      }
      isInSession = false;
      hasDuration = false;
      hasNextSessionTime = false;
      hasNextTriggerTime = false;
   }
   else
   {
      emit timeLeft(m_id, msecsTimeLeft, msecsDuration_original);
   }
}

void Sauto::calculateTime_Session()
{
   // CALENDAR has FIRST priority
   if(m_default_calendar.size() > 0)
   {
      // there exist a defined calendar, use it
      if(!calculateTime_Calendar(m_default_calendar))
      {
         // there are nothing more for this thread to do, report and stop
         stopClock(m_id);
         emit endReport(m_id, "No future sessions found");
         this->deleteLater();
      }
   }

   // WEEK has SECOND priority
   else if(m_default_week.size() > 0)
   {
      // there exist a week definition, use it
      if(!calculateTime_Week(m_default_week))
      {
         // there are nothing more for this thread to do, report and stop
         stopClock(m_id);
         emit endReport(m_id, "No future sessions found");
         this->deleteLater();
      }
   }

   // INTERVAL has THIRD priority
   else if(m_default_intervals.size() > 0)
   {
      if(!calculateTime_Intervals(m_default_intervals, false, true))
      {
         // there are nothing more for this thread to do, report and stop
         stopClock(m_id);
         emit endReport(m_id, "No future sessions found");
         this->deleteLater();
      }
   }

   // FREQUENCY has FOURTH and last priority
   else if(m_default_freq.isValid())
   {
      if(!calculateTime_Frequency(m_default_freq))
      {
         // there are nothing more for this thread to do, report and stop
         stopClock(m_id);
         emit endReport(m_id, "No future sessions found");
         this->deleteLater();
      }
   }
   else
   {
      // there are nothing more for this thread to do, report and stop
      stopClock(m_id);
      emit endReport(m_id, "No future sessions found");
      this->deleteLater();
   }
}

bool Sauto::calculateTime_Calendar(CALENDAR_DEF &calendar)
{
   if (calendar.size() == 0)
   {
      return calculateTime_Week(m_default_week);
   }

   bool result = false;
   QDateTime now = QDateTime::currentDateTime();

   MONTH_ID month = static_cast<MONTH_ID>(now.date().month());
   QString nowMonthName = makeYear().value(month);

   // calendar is defined
   CALENDAR_ITERATOR cal_it(calendar);
   while(cal_it.hasNext())
   {
      cal_it.next();
      if(cal_it.key() >= now.date().year())
      {
         // found the year at which the next session will occur
         MONTH_DEF months = cal_it.value().second;
         if(months.size() == 0)
         {
            // the year was defined, but it had no month definitions. Must use the inherit-option
            result = calculateTime_Week(m_default_week, cal_it.key());
            break;
         }
         else if(months.contains(nowMonthName))
         {
            // found the month, it is this current month
            result = calculateTime_Month(months, nowMonthName, cal_it.key());
            break;
         }
         else
         {
            QPair<int, QString> nextMonth = findNextMonthInt(months);
            if (nextMonth.first > now.date().month())
            {
               // found the month, it is not this one, but one in the future
               result = calculateTime_Month(months, nextMonth.second, cal_it.key());
            }
            else
            {
               // this thread has no sessions in the future, all the defined months belong to the past.
               // the thread should report that it is finished and stop.
               result = false;
            }
         }
      }
   }

   return result;
}

bool Sauto::calculateTime_Month(MONTH_DEF &months, const QString &name, int year)
{
   if (!months.contains(name))
   {
      // the argument list of months doesnt contain the argument month, which is an error and shouldn't occur.
      return false;
   }

   QDateTime now = QDateTime::currentDateTime();
   int today = now.date().day();
   DAY_OF_MONTH_DEF monthDef = months.value(name);

   if (monthDef.first)
   {
      // this month uses inherited settings, go to week calc
      return calculateTime_Week(m_default_week, year, getMonthAsInt(name));
   }

   QMap<int, CALENDAR_DATE> days = monthDef.second;
   if (days.size() == 0)
   {
      // this month does not have any days selected, inherit week settings
      return calculateTime_Week(m_default_week, year, getMonthAsInt(name));
   }

   QMapIterator<int, CALENDAR_DATE> days_it(days);
   // remember, the map is sorted, so the first day comes first
   while(days_it.hasNext())
   {
      days_it.next();
      int dayNo = days_it.key();
      CALENDAR_DATE date = days_it.value();
      int currDay = QString("%1").arg(dayNo).right(2).toInt();
      if(currDay >= today)
      {
         // found the day of the next session
         QDate calDate = calendarDate_QDate(date);
         INTERVAL_LIST intervals = date.second;
         return calculateTime_Day(calDate, false, intervals);
      }
   }

   // reaching this point means that the month has selected days, but they are all in the past
   // this thread should report this, and stop
   return false;
}

bool Sauto::calculateTime_Week(WEEK_DEF &week, int year, int month)
{
   if (week.size() == 0)
   {
      return calculateTime_Intervals(m_default_intervals);
   }

   QDateTime now = QDateTime::currentDateTime();
   QDate date = now.date();
   bool customDate = false;
   int firstCustomDay = 0;
   if(year > 0 && month > 0)
   {
      date.setDate(year, month, 1);
      while (now.date() > date)
      {
         date = date.addDays(1);
      }
      customDate = true;
      firstCustomDay = date.dayOfWeek();
   }

   int index = 0;
   while(true)
   {
      QDate checkDate = date.addDays(index);
      int checkDay = checkDate.dayOfWeek();
      if(week.contains(checkDay))
      {
         DAY_OF_WEEK_DEF day = week[checkDay];
         bool dayIsEnabled = dayIsToggled(day);
         if(dayIsEnabled)
         {
            INTERVAL_LIST intervals;
            bool doInheritInterval = dayInheritsTime(day);
            if (doInheritInterval)
            {
               intervals = m_default_intervals; //< inherit the default interval
            }
            else
            {
               intervals = day.second; //< use the interval provided
               if (intervals.size() <= 0)
               {
                  intervals = m_default_intervals; //< the interval provided was invalid, so inherit the default interval instead
               }
            }
            
            if(checkDate == now.date())
            {
               if (calculateTime_Intervals(intervals))
               {
                  return true;
               }
               else
               {
                  ++index;
                  continue;
               }
            }

            if(calculateTime_Intervals(intervals, true))
            {
               QDateTime tomorrow(now.date().addDays(1), QTime(0,0,0));
               quint64 msec_restOfToday = tomorrow.toMSecsSinceEpoch() - now.toMSecsSinceEpoch();
               while(tomorrow.date() < checkDate)
               {
                  msecsToNextSession += msecsPer_Day;
                  tomorrow = tomorrow.addDays(1);
               }
               msecsToNextSession += msec_restOfToday;
               msecsToNextSession_original = msecsToNextSession;
               return true;
            }
         }
      }

      ++index;
   }
   return false; //< reaching here is an error !!


   // ALTERNATIVE ALGORITHM : Starts on week-data day. Problem : can't count into next week
   //bool lastToggled = false;
   //WEEK_ITERATOR it(week);
   //while(it.hasNext())
   //{
   //   it.next();
   //   int dayOfWeek = it.key();

   //   if(customDate && firstCustomDay != dayOfWeek && !lastToggled)
   //   {
   //      continue;
   //   }

   //   DAY_OF_WEEK_DEF day = it.value();

   //   bool isToggled = day.get<0>();
   //   if(!isToggled)
   //   {
   //      // this day is turned off
   //      lastToggled = true;
   //      continue;
   //   }

   //   // find the date corresponding to this day-of-week
   //   if(date.dayOfWeek() > dayOfWeek) 
   //   {
   //      continue;
   //   }
   //   while(date.dayOfWeek() < dayOfWeek) 
   //   {
   //      date = date.addDays(1);
   //   }

   //   bool doInheritd         = day.get<1>();
   //   INTERVAL_LIST intervals = day.get<2>();

   //   if(doInheritd)
   //   {
   //      intervals = m_default_intervals;
   //   }

   //   if(date == now.date())
   //   {
   //      if(calculateTime_Intervals(intervals))
   //      {
   //         return true;
   //      }
   //      else
   //      {
   //         continue;
   //      }
   //   }

   //   if(calculateTime_Intervals(intervals, true))
   //   {
   //      QDateTime tomorrow(now.date().addDays(1), QTime(0,0,0));
   //      quint64 msec_restOfToday = tomorrow.toMSecsSinceEpoch() - now.toMSecsSinceEpoch();
   //      while(tomorrow.date() < date)
   //      {
   //         msecsToNextSession += msecsPer_Day;
   //         tomorrow = tomorrow.addDays(1);
   //      }
   //      msecsToNextSession += msec_restOfToday;
   //      msecsToNextSession_original = msecsToNextSession;
   //      return true;
   //   }
   //}

   //return false;
}

bool Sauto::calculateTime_Day(const QDate &date, bool inherited, INTERVAL_LIST &interval)
{
   bool iret = false;
   if(inherited || interval.size() == 0)
   {
      // the condition means that caller has specifically asked that the interval be inherited from
      // a lower level, OR the argument interval for this day doesnt contain any interval and therefore
      // it must be inherited. 
      if (m_default_week.size() == 0 || //< doesnt contain any specifications at all
         !m_default_week.contains(date.dayOfWeek()) || //< doesnt contain specifications for this day
         !dayIsToggled(m_default_week.value(date.dayOfWeek())) || //< day is toggled off
         dayInheritsTime(m_default_week.value(date.dayOfWeek())) || //< this day is specified to inherit further down
         m_default_week.value(date.dayOfWeek()).second.size() == 0) //< the week definition interval for this day had no entries
      {
         // this means that either there is no week definition, or it doesnt contain the
         // definition for this specific day of the week OR the week-level of inheritance specifies this day
         // to be toggled off or inherited (in which case the calendar priority "overrides" this and skips to next level of inheritance)
         // The next level of inheritance is therefore the default intervals
         return calculateTime_Intervals(m_default_intervals);
      }
      else
      {
         // inherit from the week definition
         //return calculateTime_Intervals(m_default_week.value(date.dayOfWeek()).second.second, true, date);
         INTERVAL_LIST weekIntervals = m_default_week.value(date.dayOfWeek()).second;
         return calculateTime_Intervals(weekIntervals);
      }
   }
   else
   {
      if(date > QDateTime::currentDateTime().date())
      {
         iret = calculateTime_Intervals(interval, true);
         msecsToNextSession_original += msecsToTomorrow();
         QDateTime datetime(date, QTime(0,0,0,0));
         bool ok = false;
         int wholeDays = wholeDaysUntilEpochMS(datetime.toMSecsSinceEpoch(), ok);
         if (ok)
         {
            msecsToNextSession_original += wholeDays * msecsPer_Day;
         }
         msecsToNextSession = msecsToNextSession_original;
      }
      else
      {
         iret = calculateTime_Intervals(interval);
      }
   }
   return iret;
}

bool Sauto::calculateTime_Intervals(INTERVAL_LIST &interval, bool ignoreCurrentTime, bool lookTomorrow)
{
   if (interval.size() == 0)
   {
      return calculateTime_Frequency(m_default_freq, ignoreCurrentTime);
   }

   QMap<int, int> startIdMap;
   int i = -1;
   bool ok = false;
   INTERVAL_ITERATOR_MUTABLE it(interval);
   while(it.hasNext())
   {
      ++i;
      SautoModel intervalDef = it.next();
      if(!intervalDef.isValid())
      {
         intervalDef.updateHasCustomInterval();
         if(intervalDef.getHasCustomInterval() && m_default_freq.isValid())
         {
            // combine the interval with the default frequency
            SautoModel combined = m_default_freq;
            combined.setStartTimeMSecs(intervalDef.getStartTimeMSec());
            combined.setDuration(intervalDef.getDuration());
            combined.updateHasCustomInterval();
            intervalDef = combined;
            it.setValue(intervalDef);
         }
         else
         {
            continue;
         }
      }
      quint64 time2next = intervalDef.calculateNextSession(ok, ignoreCurrentTime, lookTomorrow);
      if (!ok)
      {
         continue;
      }
      startIdMap.insert(time2next, i);
   }

   if (startIdMap.size() <= 0)
   {
      return false;
   }

   QMapIterator<int, int> indexIt(startIdMap);
   indexIt.next();
   msecsToNextSession_original = indexIt.key();
   int frontIndex = indexIt.value();
   
   msecsToNextSession = msecsToNextSession_original;
   m_current_freq = interval[frontIndex];
   hasNextSessionTime = true;
   m_eventType = intervalType_to_eventType(m_current_freq.getType());

   return true;
}

bool Sauto::calculateTime_Frequency(SautoModel &freq, bool ignoreCurrentTime)
{
   bool ok;
   bool localIgnoreTime = ignoreCurrentTime;
   if (freq.getStartTimeMSec() < 0)
   {
      localIgnoreTime = true;
   }

   msecsToNextSession_original = freq.calculateNextSession(ok, localIgnoreTime);
   if (!ok)
   {
      // unable to locate next session, meaning that it doesnt exist, or it is in the past
      return false;
   }

   // the next session was successfully found
   msecsToNextSession = msecsToNextSession_original;
   hasNextSessionTime = true;
   m_current_freq = freq;
   return true;
}

EEventType sauto::intervalType_to_eventType(EIntervalType intervalType)
{
   switch(intervalType)
   {
   case(NOT_SPECIFIED) : 
      return EVENT_UNDECIDED;
   case(WAVELET): 
      return EVENT_WAVELET;
   case(STATIC): 
      return EVENT_INTERVAL;
   case(SINGLE): 
      return EVENT_SINGLESHOT;
   default: 
      return EVENT_UNDECIDED;
   }
}