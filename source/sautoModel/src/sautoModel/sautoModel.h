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
//  \file      sautoModel.h
//
//  \brief     Definition of time interval
//
//  \author    Stian Broen
//
//  \date      18.02.2013
//
//  \par       Revision History
//
//
//
//
//
//h-//////////////////////////////////////////////////////////////////////////

#ifndef _SCHEDULE_TIME_INTERVAL_H_
#define _SCHEDULE_TIME_INTERVAL_H_

// Qt includes
#include <QString>

// local includes
#include "timeStuff.h"

namespace sauto {
   class SautoModel
   {
   public:
      explicit SautoModel();
      SautoModel(const SautoModel &other);
      explicit SautoModel(EIntervalType type,
         qreal   phase,
         quint64 durationMSecs,
         quint64 startTimeMSecs,
         quint64 periodTotalMSecs,
         bool    hasCustom = false,
         const   QString &onPeak = "",
         const   QString &onValley = "",
         const   QString &onRising = "",
         const   QString &onSinking = "");
      ~SautoModel();
      SautoModel& operator=(const SautoModel &arg);

      const QString getTypeString() const;
      bool isValid() const;
      void reset();
      EIntervalType calculateNextTrigger(qint64 &msecs, bool &ok, EWavePoint &wp);
      quint64 calculateNextSession(bool &ok, bool ignoreCurrentTime = false, bool lookTomorrow = false);

      inline EIntervalType getType()    const { return m_type; }
      inline qreal getPhase()           const { return m_phase; }
      inline QString getOnPeak()        const { return m_onPeak; }
      inline QString getOnValley()      const { return m_onValley; }
      inline QString getOnRising()      const { return m_onRising; }
      inline QString getOnSinking()     const { return m_onSinking; }
      inline quint64 getPeriodTotMSec() const { return m_periodTotalMSecs; }
      inline quint64 getDuration()      const { return m_durationMSecs; }
      inline qint64 getStartTimeMSec()  const { return m_startTimeMSecs; }
      inline unsigned short getMSecs()  const { return m_periodMSecs; }
      inline unsigned short getSecs()   const { return m_periodSeconds; }
      inline unsigned short getMins()   const { return m_periodMinutes; }
      inline unsigned short getHours()  const { return m_periodHours; }

      inline void setHasCustomInterval(bool val) { m_hasCustomInterval = val; }
      bool getHasCustomInterval() const;
      void updateHasCustomInterval();
      bool hasPeak() const;
      bool hasValley() const;
      bool hasRising() const;
      bool hasSinking() const;
      void setType(EIntervalType type);
      void setStartTimeMSecs(qint64 msecs);
      void setDuration(quint64 duration);
      void setPhase(qreal phase);
      void setOnPeak(const QString &file);
      void setOnValley(const QString &file);
      void setOnRising(const QString &file);
      void setOnSinking(const QString &file);
      void setPeriodTotMSecs(quint64 msecs);
      void setMSecs(unsigned short msecs);
      void setSeconds(unsigned short seconds);
      void setMinutes(unsigned short minutes);
      void setHours(unsigned short hours);

   private:
      void calcTimeToPeriod();
      void calcPeriodToTime();
      bool hasCleanPhase() const;
      bool currentInterval(qreal &start, qreal &stop);
      bool calculateNextTrigger_WAVELET(qint64 &msecs, EWavePoint &wp);
      bool calculateNextTrigger_STATIC(qint64 &msecs);
      bool calculateNextTrigger_SINGLE(qint64 &msecs);

   private:
      EIntervalType m_type;
      qreal   m_phase;
      quint64 m_durationMSecs;
      qint64  m_startTimeMSecs;
      quint64 m_periodTotalMSecs;
      unsigned short m_periodMSecs;
      unsigned short m_periodSeconds;
      unsigned short m_periodMinutes;
      unsigned short m_periodHours;
      bool m_hasCustomInterval;
      QString m_onPeak;
      QString m_onValley;
      QString m_onRising;
      QString m_onSinking;
   };
}

#endif