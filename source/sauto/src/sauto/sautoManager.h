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
//  \file      sautoManager.h
//
//  \brief     Definition of a class to handle sautos
//
//  \author    Stian Broen
//
//  \date      01.08.2012
//
//  \par       Copyright: BroenTech AS
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

#ifndef _SCHEDULE_MANAGER_H
#define _SCHEDULE_MANAGER_H

// Qt includes
#include <QObject>
#include <QHash>
#include <QMutex>

// solution includes
#include <sautoModel/sautoDefs.h>

// local includes
#include "sauto.h"

namespace sauto {
   class SautoManager : public QObject
   {
      Q_OBJECT

   public:
      explicit SautoManager(QObject *parent = 0);
      ~SautoManager();
      void removeClock(int id);
      void pauseClock(int id);
      void stopClock(int id);
      bool hasClock(int id);
      bool startClock(int id);
      bool addClock(
         int id,
         const SautoModel  &def_frequency,
         const INTERVAL_LIST &def_intervals,
         const WEEK_DEF &def_week,
         const CALENDAR_DEF &def_calendar
         );

   signals:
      void stopClock_sig(int id);
      void pauseClock_sig(int id);
      void startClock_sig(int id);
      void clockFinished(int id, const QString &endReport);
      void constantIntervals(int id);
      void flushAll (int id);
      void triggered(int id);
      void triggered(int clockId, const QString &taskID);
      void timeToNextSession(int id, quint64 msecsLeft, quint64 msecsStarted, const QString &msg);
      void timeLeft(int id, quint64 msecsLeft, quint64 msecsStarted);
      void timeToNextTrigger(int id, quint64 msecsLeft, quint64 msecsStarted);

   public slots:
      void setXml(const QString &xml);

   private slots:
      void endReport(int id, const QString &str);

   private: // members
      QMutex m_mutex;
      QHash<int, Sauto*> m_clocks;

   };
}

#endif