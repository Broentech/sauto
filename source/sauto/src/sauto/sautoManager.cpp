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
//  \file      sautoManager.cpp
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

// Qt includes
#include <QMutexLocker>
#include <QFileInfo>
#include <QtDebug>

// solution includes
#include <sautoXml/sautoXml.h>

// local includes
#include "sautoManager.h"

using namespace sauto;

SautoManager::SautoManager(QObject *parent)
   :QObject(parent)
{

}

SautoManager::~SautoManager()
{
   
}

bool SautoManager::addClock(int id, const SautoModel  &def_frequency, const INTERVAL_LIST &def_intervals, const WEEK_DEF &def_week, const CALENDAR_DEF &def_calendar)
{
   // create clock object and populate it with time-members
   Sauto *newClock = new Sauto(this);
   newClock->init(id, def_frequency, def_intervals, def_week, def_calendar);

   connect(newClock, SIGNAL(endReport(int, const QString &)), 
      this, SLOT(endReport(int, const QString &)));

   connect(newClock ,SIGNAL(triggered(int)), 
      this, SIGNAL(triggered(int)));

   connect(newClock, SIGNAL(triggered(int, const QString &)),
      this, SIGNAL(triggered(int, const QString &)));

   connect(newClock, SIGNAL(flushAll(int)), 
      this, SIGNAL(flushAll(int)));

   connect(newClock, SIGNAL(timeToNextSession(int, quint64, quint64, const QString &)), 
      this, SIGNAL(timeToNextSession(int, quint64, quint64, const QString &)));

   connect(newClock, SIGNAL(timeToNextTrigger(int, quint64, quint64)), 
      this, SIGNAL(timeToNextTrigger(int, quint64, quint64)));

   connect(newClock, SIGNAL(timeLeft(int, quint64, quint64)), 
      this, SIGNAL(timeLeft(int, quint64, quint64)));

   connect(newClock, SIGNAL(constantIntervals(int)), 
      this, SIGNAL(constantIntervals(int)));

   connect(this, SIGNAL(stopClock_sig(int)),
      newClock, SLOT(stopClock(int)));

   connect(this, SIGNAL(pauseClock_sig(int)),
      newClock, SLOT(pauseClock(int)));

   connect(this, SIGNAL(startClock_sig(int)),
      newClock, SLOT(startClock(int)));

   m_clocks.insert(id, newClock);

   return true;
}

bool SautoManager::hasClock(int id)
{
   QMutexLocker lock(&m_mutex);
   return m_clocks.contains(id);
}

bool SautoManager::startClock(int id)
{
   QMutexLocker lock(&m_mutex);
   if(m_clocks.contains(id))
   {
      emit startClock_sig(id);
      return true;
   }
   return false;
}

void SautoManager::removeClock(int id)
{
   if (!m_clocks.contains(id))
   {
      return;
   }

   stopClock(id);

   QMutexLocker lock(&m_mutex);
   QMutableHashIterator<int, Sauto*> it(m_clocks);
   while (it.hasNext())
   {
      it.next();
      if (it.key() == id)
      {
         delete it.value();
         it.remove();
         return;
      }
   }
}

void SautoManager::stopClock(int id)
{
   QMutexLocker lock(&m_mutex);
   QMutableHashIterator<int, Sauto*> it(m_clocks);
   while (it.hasNext())
   {
      it.next();
      if (it.key() == id)
      {
         emit stopClock_sig(id);
         it.remove();
         return;
      }
   }
}

void SautoManager::pauseClock(int id)
{
   QMutexLocker lock(&m_mutex);
   QHashIterator<int, Sauto*> it(m_clocks);
   while (it.hasNext())
   {
      it.next();
      if (it.key() == id)
      {
         emit pauseClock_sig(id);
         return;
      }
   }
}

void SautoManager::endReport(int id, const QString &str)
{
   QMutexLocker lock(&m_mutex);
   QMutableHashIterator<int, Sauto*> it(m_clocks);
   while (it.hasNext())
   {
      it.next();
      if (it.key() == id)
      {
         it.remove();
      }
   }
   emit clockFinished(id, str);
}

void SautoManager::setXml(const QString &xml)
{
   qDebug() << QString("%1").arg(xml);
   if(!QFileInfo(xml).exists())
   {
      qCritical() << QString("File not found");
      return;
   }

   SautoModel m_default_Frequency; 
   INTERVAL_LIST m_default_TimeIntervals;
   WEEK_DEF m_default_Week;
   CALENDAR_DEF m_default_Calendar;
 
   SautoXml xmlReader(this);
   if(!xmlReader.readClockFile(xml, m_default_Frequency, m_default_TimeIntervals, m_default_Week, m_default_Calendar))
   {
      qCritical() << QString("Failed at loading");
      return;
   }

   int newID = m_clocks.size();
   if(!addClock(newID, m_default_Frequency, m_default_TimeIntervals, m_default_Week, m_default_Calendar))
   {
      qCritical() << QString("Failed at adding clock");
      return;
   }

   qDebug() << QString("Done, starting clock");
   if (!startClock(newID))
   {
      qCritical() << QString("Failed at starting clock");
   }
}
