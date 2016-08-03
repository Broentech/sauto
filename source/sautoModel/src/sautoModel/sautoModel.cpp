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
//  \file      sautoModel.cpp
//
//  \brief     Definition of time interval
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

// Qt includes
#include <QtCore/qmath.h>

// local includes
#include "sautoModel.h"

using namespace sauto;

SautoModel::SautoModel()
   :m_type(NOT_SPECIFIED),
   m_phase(0.0),
   m_durationMSecs(msecsPer_Day),
   m_startTimeMSecs(0),
   m_periodTotalMSecs(0),
   m_periodMSecs(0),
   m_periodSeconds(0),
   m_periodMinutes(0),
   m_periodHours(0),
   m_hasCustomInterval(false)
{

}

SautoModel::SautoModel(EIntervalType type,
   qreal   phase,
   quint64 durationMSecs,
   quint64 startTimeMSecs,
   quint64 periodTotalMSecs,
   bool    hasCustom,
   const   QString &onPeak,
   const   QString &onValley,
   const   QString &onRising,
   const   QString &onSinking)
   :m_type(NOT_SPECIFIED),
   m_phase(0.0),
   m_durationMSecs(msecsPer_Day),
   m_startTimeMSecs(startTimeMSecs),
   m_periodTotalMSecs(0),
   m_periodSeconds(0),
   m_periodMinutes(0),
   m_periodHours(0),
   m_hasCustomInterval(false)
{
   setType(type);
   setPhase(phase);
   setDuration(durationMSecs);
   setStartTimeMSecs(startTimeMSecs);
   setPeriodTotMSecs(periodTotalMSecs);
   setOnPeak(onPeak);
   setOnValley(onValley);
   setOnRising(onRising);
   setOnSinking(onSinking);
   setHasCustomInterval(hasCustom);
}

SautoModel::SautoModel(const SautoModel &other)
{
   *this = other;
}

SautoModel::~SautoModel()
{

}

SautoModel& SautoModel::operator=(const SautoModel &arg)
{
   this->setType(arg.getType());
   this->setPhase(arg.getPhase());
   this->setDuration(arg.getDuration());
   this->setStartTimeMSecs(arg.getStartTimeMSec());
   this->setPeriodTotMSecs(arg.getPeriodTotMSec());
   this->setOnPeak(arg.getOnPeak());
   this->setOnValley(arg.getOnValley());
   this->setOnRising(arg.getOnRising());
   this->setOnSinking(arg.getOnSinking());
   this->setHasCustomInterval(arg.getHasCustomInterval());
   return *this;
}

void SautoModel::calcTimeToPeriod()
{
   m_periodTotalMSecs = ((m_periodHours * 60 * 60 +
      m_periodMinutes * 60 +
      m_periodSeconds) * 1000) + m_periodMSecs;
}

void SautoModel::calcPeriodToTime()
{
   m_periodHours = qFloor(static_cast<qreal>(m_periodTotalMSecs) / (1000 * 60 * 60));
   m_periodMinutes = qFloor(static_cast<qreal>(m_periodTotalMSecs) / (1000 * 60.0) - (m_periodHours * 60));
   m_periodSeconds = qFloor(static_cast<qreal>(m_periodTotalMSecs) / (1000) - (m_periodHours * 60 * 60) - (m_periodMinutes * 60));
   m_periodMSecs = m_periodTotalMSecs - (m_periodHours * 60 * 60 * 1000) - (m_periodMinutes * 60 * 1000) - (m_periodSeconds * 1000);
}

void SautoModel::setDuration(quint64 duration)
{
   if (duration <= msecsPer_Day)
   {
      // only allow wavelet durations that are less than 1 day
      m_durationMSecs = duration;
   }
}

void SautoModel::setStartTimeMSecs(qint64 msecs)
{
   m_startTimeMSecs = msecs;
   updateHasCustomInterval();
}

void SautoModel::setType(EIntervalType type)
{
   m_type = type;
}

void SautoModel::setPeriodTotMSecs(quint64 msecs)
{
   if (msecs <= m_maxTimeletPeriod)
   {
      m_periodTotalMSecs = msecs;
      calcPeriodToTime();
      getHasCustomInterval();
   }
}

void SautoModel::setMSecs(unsigned short msecs)
{
   if (msecs < 1000)
   {
      m_periodMSecs = msecs;
      calcTimeToPeriod();
   }
}

void SautoModel::setSeconds(unsigned short seconds)
{
   if (seconds < 60)
   {
      m_periodSeconds = seconds;
      calcTimeToPeriod();
   }
}

void SautoModel::setMinutes(unsigned short minutes)
{
   if (minutes < 60)
   {
      m_periodMinutes = minutes;
      calcTimeToPeriod();
   }
}

void SautoModel::setHours(unsigned short hours)
{
   if (hours < 24)
   {
      m_periodHours = hours;
      calcTimeToPeriod();
   }
}

bool SautoModel::isValid() const
{
   if (m_type == NOT_SPECIFIED)
   {
      return false;
   }
   else if (m_type == SINGLE)
   {
      if (m_startTimeMSecs > msecsPer_Day)
      {
         return false;
      }
   }
   else if (m_type == STATIC)
   {
      if (m_periodTotalMSecs >= msecsPer_Day ||
         m_periodTotalMSecs <= 0)
      {
         return false;
      }
   }
   else if (m_type == WAVELET)
   {
      if (m_periodTotalMSecs <= 0 ||
         (m_periodSeconds <= 0 && m_periodMinutes <= 0 && m_periodHours <= 0 && m_periodMSecs <= 0))
      {
         return false;
      }
   }
   return true;
}

const QString SautoModel::getTypeString() const
{
   if (m_type == SINGLE)
   {
      return "SINGLE";
   }
   else if (m_type == STATIC)
   {
      return "STATIC";
   }
   else if (m_type == WAVELET)
   {
      return "WAVELET";
   }
   else
   {
      return "NOT_SPECIFIED";
   }
}

void SautoModel::reset()
{
   m_type = NOT_SPECIFIED;
   m_phase = 0.0;
   m_durationMSecs = 0;
   m_startTimeMSecs = 0;
   m_periodTotalMSecs = 0;
   m_periodMSecs = 0;
   m_periodSeconds = 0;
   m_periodMinutes = 0;
   m_periodHours = 0;
   m_hasCustomInterval = false;
   m_onPeak = "";
   m_onValley = "";
   m_onRising = "";
   m_onSinking = "";
}


//  Find the amount of seconds until the interval starts, starting from now.
//  NOTE : this function will not care about what type of interval it is
//  so it can be WAVELET, STATIC or SINGLE.
quint64 SautoModel::calculateNextSession(bool &ok, bool ignoreCurrentTime, bool lookTomorrow)
{
   ok = true;
   if (ignoreCurrentTime)
   {
      if (m_startTimeMSecs < 0)
      {
         return 0;
      }
      return m_startTimeMSecs;
   }

   qint64 mSecsToNextSession = 0;
   QDateTime now = QDateTime::currentDateTime();
   quint64 msecRightNow = get_MSEC_sinceMidnight(now.time());
   QDateTime midnight(now.date(), QTime(0, 0, 0));
   quint64 midnightMsec = midnight.toMSecsSinceEpoch();
   quint64 msecStart = midnightMsec + m_startTimeMSecs;
   bool hasEnded = true;
   if (m_type != SINGLE)
   {
      quint64 endTime = msecStart + m_durationMSecs - 200;
      quint64 nowMsecEpo = now.toMSecsSinceEpoch();
      if (endTime < nowMsecEpo)
      {
         // this interval has ended
         if (lookTomorrow)
         {
            return msecsPer_Day - msecRightNow + m_startTimeMSecs;
         }
         else
         {
            ok = false;
            return 0;
         }
      }
      else
      {
         hasEnded = false;
      }
   }

   // now, find the amount of seconds from the start of this day, until the session begins
   if (m_hasCustomInterval)
   {
      mSecsToNextSession = msecStart - msecRightNow - midnightMsec;
   }

   if (0 > mSecsToNextSession)
   {
      // the start of the interval is in the past, find out if it has ended or not
      if (!hasEnded)
      {
         return 0;
      }
      ok = false;
   }

   return mSecsToNextSession;
}


//  NOTE : Assumes UTC time
//  this function will see when the next trigger from the wavelet will occur, given that
//  the current time is within the boundaries of the wavelet
EIntervalType SautoModel::calculateNextTrigger(qint64 &msecs, bool &ok, EWavePoint &wp)
{
   ok = true;
   switch (m_type)
   {

   case(WAVELET) :
      if (ok = (calculateNextTrigger_WAVELET(msecs, wp))) return WAVELET;
      break;

   case(STATIC) :
      wp = WP_NOT_SPECIFIED;
      if (ok = (calculateNextTrigger_STATIC(msecs))) return STATIC;
      break;

   case(SINGLE) :
      wp = WP_NOT_SPECIFIED;
      if (ok = (calculateNextTrigger_SINGLE(msecs))) return SINGLE;
      break;

   default:
      break;
   }
   return NOT_SPECIFIED;
}


//  Given the current time, this function will find if, and when, the next trigger
//  in the wavelet will occur, and what kind of task this trigger will perform
bool SautoModel::calculateNextTrigger_WAVELET(qint64 &msecs, EWavePoint &wp)
{
   // check if there are any tasks at all
   if (!hasPeak() && !hasRising() && !hasSinking() && !hasValley())
   {
      wp = WP_NOT_SPECIFIED;
      return false; //< There wasn't, no need to do anything
   }

   qreal epochMSecs_waveStart = 0;
   qreal epochMSecs_waveStop = 0;
   if (!currentInterval(epochMSecs_waveStart, epochMSecs_waveStop))
   {
      wp = WP_NOT_SPECIFIED;
      return false;
   }

   quint64 epochMSeconds_RightNow = QDateTime::currentDateTime().toMSecsSinceEpoch();
   qreal   epochMSeconds_RightNow_real = static_cast<qreal>(epochMSeconds_RightNow);
   qreal   startMSec = static_cast<qreal>(getStartTimeMSec());
   if (startMSec < 0)
   {
      startMSec = 0;
   }
   qreal phaseOffsetMSecs = static_cast<qreal>(getPeriodTotMSec() * getPhase()) / 360.0;
   int noPeriods = qRound((startMSec + static_cast<qreal>(getDuration())) / static_cast<qreal>(getPeriodTotMSec()));

   // go through all the periods to locate the next one to occur
   for (int i = 0; i < noPeriods; i++)
   {
      // start of this period
      qreal periodStart = epochMSecs_waveStart + static_cast<qreal>(i * getPeriodTotMSec());

      // end of this period
      qreal periodStop = periodStart + static_cast<qreal>(getPeriodTotMSec());

      // the wavelet may be backward phased up to 359 degrees, adjust for that
      periodStart -= phaseOffsetMSecs;
      periodStop -= phaseOffsetMSecs;

      // see if the current time is within this period
      if ((epochMSeconds_RightNow_real >= periodStart &&
         epochMSeconds_RightNow_real < periodStop) == false)
      {
         // it wasn't, check next
         continue;
      }

      // the amount of seconds for one quarter
      qreal ninetyDegreesTime = static_cast<qreal>(getPeriodTotMSec()) / 4.0;

      // find the quarter of the period that the current time is within
      for (int j = 0; j < 4; j++)
      {
         // quarter interval time definitions
         qreal quarterStart = periodStart + (static_cast<qreal>(j)* ninetyDegreesTime);
         qreal quarterStop = quarterStart + ninetyDegreesTime;

         // see if the current time is within this quartet
         if ((epochMSeconds_RightNow_real >= quarterStart &&
            epochMSeconds_RightNow_real < quarterStop) == false)
         {
            // it wasn't, check next
            continue;
         }

         // locate the correct task at the correct time
         switch (j)
         {
         case(0) :
            if (quarterStart == epochMSeconds_RightNow_real && hasCleanPhase() &&
               hasSinking()) {
               quarterStop = quarterStart; wp = SINKING;
            }
            else if (hasPeak())
            {
               quarterStop += 0 * ninetyDegreesTime;
               wp = PEAK;
            }
            else if (hasRising())
            {
               quarterStop += 1 * ninetyDegreesTime;
               wp = RISING;
            }
            else if (hasValley())
            {
               quarterStop += 2 * ninetyDegreesTime;
               wp = VALLEY;
            }
            else if (hasSinking())
            {
               quarterStop += 3 * ninetyDegreesTime;
               wp = SINKING;
            }
            break;

         case(1) :
            if (quarterStart == epochMSeconds_RightNow_real && hasCleanPhase() &&
               hasPeak())
            {
               quarterStop = quarterStart; wp = PEAK;
            }
            else if (hasRising())
            {
               quarterStop += 0 * ninetyDegreesTime;
               wp = RISING;
            }
            else if (hasValley())
            {
               quarterStop += 1 * ninetyDegreesTime;
               wp = VALLEY;
            }
            else if (hasSinking())
            {
               quarterStop += 2 * ninetyDegreesTime;
               wp = SINKING;
            }
            else if (hasPeak())
            {
               quarterStop += 3 * ninetyDegreesTime;
               wp = PEAK;
            }
            break;

         case(2) :
            if (quarterStart == epochMSeconds_RightNow_real && hasCleanPhase() &&
               hasRising())
            {
               quarterStop = quarterStart; wp = RISING;
            }
            else if (hasValley())
            {
               quarterStop += 0 * ninetyDegreesTime;
               wp = VALLEY;
            }
            else if (hasSinking())
            {
               quarterStop += 1 * ninetyDegreesTime;
               wp = SINKING;
            }
            else if (hasPeak())
            {
               quarterStop += 2 * ninetyDegreesTime;
               wp = PEAK;
            }
            else if (hasRising())
            {
               quarterStop += 3 * ninetyDegreesTime;
               wp = RISING;
            }
            break;

         case(3) :
            if (quarterStart == epochMSeconds_RightNow_real && hasCleanPhase() &&
               hasValley())
            {
               quarterStop = quarterStart; wp = VALLEY;
            }
            else if (hasSinking())
            {
               quarterStop += 0 * ninetyDegreesTime;
               wp = SINKING;
            }
            else if (hasPeak())
            {
               quarterStop += 1 * ninetyDegreesTime;
               wp = PEAK;
            }
            else if (hasRising())
            {
               quarterStop += 2 * ninetyDegreesTime;
               wp = RISING;
            }
            else if (hasValley())
            {
               quarterStop += 3 * ninetyDegreesTime;
               wp = VALLEY;
            }
            break;

         default:
         wp = WP_NOT_SPECIFIED;
         return false;
         }

         msecs = qFloor(quarterStop - epochMSeconds_RightNow_real);
         if (msecs <= 100 && quarterStart != epochMSeconds_RightNow_real)
         {
            // 2 consecutive triggers that are less than 0.1 seconds between each other is discarded
            return false;
         }
         if (msecs > epochMSecs_waveStop)
         {
            // the time found is outside the wavelet interval, so it is rejected
            return false;
         }

         // success, task and time found
         return true;

      }
   }

   // some decision should have been made at this point, if there were any valid task to identify within the wavelet at this time
   return false;
}

bool SautoModel::calculateNextTrigger_STATIC(qint64 &msecs)
{
   if (!hasPeak())
   {
      return false;
   }

   qreal epochMSecs_intervalStart = 0;
   qreal epochMSecs_intervalStop = 0;
   if (!currentInterval(epochMSecs_intervalStart, epochMSecs_intervalStop))
   {
      return false;
   }

   quint64 epochMSeconds_RightNow = QDateTime::currentDateTime().toMSecsSinceEpoch();
   qreal   epochMSeconds_RightNow_r = static_cast<qreal>(epochMSeconds_RightNow);
   qreal   periodMSec = static_cast<qreal>(this->getPeriodTotMSec());
   qreal   dur = static_cast<qreal>(this->getDuration());
   quint64 candidate = 0;
   int     noTriggers = qRound(dur / periodMSec);
   bool    hit = false;

   for (int i = 0; i<noTriggers; i++)
   {
      qreal start = epochMSecs_intervalStart + (static_cast<qreal>(i)*periodMSec);
      qreal stop = start + periodMSec;
      if (start == epochMSeconds_RightNow)
      {
         candidate = 0;
         hit = true;
         break;
      }
      else if (stop == epochMSeconds_RightNow_r)
      {
         candidate = qRound(periodMSec);
         hit = true;
         break;
      }
      else if (epochMSeconds_RightNow_r > start && epochMSeconds_RightNow_r < stop)
      {
         candidate = qRound(stop - epochMSeconds_RightNow_r);
         hit = true;
         break;
      }
   }

   if (!hit || candidate > qRound(epochMSecs_intervalStop))
   {
      return false;
   }
   msecs = candidate;
   return true;
}

bool SautoModel::calculateNextTrigger_SINGLE(qint64 &msecs)
{
   if (m_type != SINGLE)
   {
      return false;
   }
   quint64 epochMSeconds_MidnightToday = QDateTime(QDate::currentDate(), QTime(0, 0, 0, 0)).toMSecsSinceEpoch();
   quint64 epochMSeconds_RightNow = QDateTime::currentDateTime().toMSecsSinceEpoch();
   quint64 triggerTime = epochMSeconds_MidnightToday + this->getStartTimeMSec();
   if (epochMSeconds_RightNow > triggerTime)
   {
      return false;
   }
   msecs = triggerTime - epochMSeconds_RightNow;
   return true;
}

bool SautoModel::currentInterval(qreal &start, qreal &stop)
{
   if (m_type == SINGLE || m_type == NOT_SPECIFIED)
   {
      return false;
   }

   // the amount of epoch milliseconds at midnight, today
   quint64 epochMSeconds_MidnightToday = QDateTime(QDate::currentDate(), QTime(0, 0, 0, 0)).toMSecsSinceEpoch();

   // the amount of epoch milliseconds right now
   quint64 epochMSeconds_RightNow = QDateTime::currentDateTime().toMSecsSinceEpoch();

   // first check if the wavelet last for 1 day, starting midnight (when no interval is defined)
   if (m_startTimeMSecs == 0 && m_durationMSecs == msecsPer_Day && m_hasCustomInterval == false)
   {
      // what this means, is that the defined interval for the wavelet starts at midnight, and last the entire day
      // based on this, it is assumed that since the client asks for the next trigger, it is being done for the first time
      // on this wavelet, for this running session

      // start it now, so that the behaviour reflects the start of the wavelet, as indicated by the GUI
      setStartTimeMSecs(epochMSeconds_RightNow - epochMSeconds_MidnightToday);

      // last the rest of the day
      setDuration(msecsPer_Day - (epochMSeconds_RightNow - epochMSeconds_MidnightToday));
   }

   if (m_startTimeMSecs < 0 && m_hasCustomInterval && m_durationMSecs > 0)
   {
      start = epochMSeconds_RightNow;
      stop = epochMSeconds_RightNow + m_durationMSecs;
      return true;
   }

   // the time when this wavelet will start
   qreal startMSec = static_cast<qreal>(m_startTimeMSecs);
   qreal epochMSecs_intervalStart = epochMSeconds_MidnightToday + startMSec;

   // see if this wavelet has already started
   if (epochMSeconds_RightNow < epochMSecs_intervalStart)
   {
      // this wavelet has not yet started, it will start in the future
      return false;
   }

   // the time when this wavelet will end
   qreal epochMSecs_intervalStop = epochMSecs_intervalStart + getDuration();

   // see if this wavelet has already ended
   if (epochMSeconds_RightNow > epochMSecs_intervalStop)
   {
      // this wavelet has finished in the past
      return false;
   }

   start = epochMSecs_intervalStart;
   stop = epochMSecs_intervalStop;
   return true;
}

bool SautoModel::hasCleanPhase() const
{
   if (getPhase() == 0 ||
      getPhase() == 90 ||
      getPhase() == 180 ||
      getPhase() == 270)
   {
      return true;
   }
   return false;
}

void SautoModel::setPhase(qreal phase)
{
   if (phase > 0 && phase < 360)
   {
      m_phase = phase;
   }
   else
   {
      m_phase = 0;
   }
}

void SautoModel::setOnPeak(const QString &file)
{
   m_onPeak = file;
}

void SautoModel::setOnValley(const QString &file)
{
   m_onValley = file;
}

void SautoModel::setOnRising(const QString &file)
{
   m_onRising = file;
}

void SautoModel::setOnSinking(const QString &file)
{
   m_onSinking = file;
}

bool SautoModel::hasPeak() const
{
   if (m_onPeak != "")
   {
      return true;
   }
   return false;
}

bool SautoModel::hasValley() const
{
   if (m_onValley != "")
   {
      return true;
   }
   return false;
}

bool SautoModel::hasRising() const
{
   if (m_onRising != "")
   {
      return true;
   }
   return false;
}

bool SautoModel::hasSinking() const
{
   if (m_onSinking != "")
   {
      return true;
   }
   return false;
}

void SautoModel::updateHasCustomInterval()
{
   if (m_durationMSecs - m_startTimeMSecs == msecsPer_Day || m_durationMSecs > msecsPer_Day)
   {
      m_hasCustomInterval = false;
   }
   else
   {
      m_hasCustomInterval = true;
   }
}

bool SautoModel::getHasCustomInterval() const
{
   return m_hasCustomInterval;
}