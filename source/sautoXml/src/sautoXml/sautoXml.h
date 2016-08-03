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
//  \file      sautoXml.h
//
//  \brief     Definition of class to handle clock definition XML files.
//
//  \author    Stian Broen
//
//  \date      02.12.2012
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

#ifndef _XML_CLOCK_H
#define _XML_CLOCK_H

// Qt includes
#include <QStringList>

// solution includes
#include <sautoModel/sautoDefs.h>

// local includes
#include "baseXml.h"

namespace sauto {
   class SautoXml : public CXML_base
   {
      Q_OBJECT

   public:
      explicit SautoXml(QObject *parent = 0);
      ~SautoXml();
      bool writeClockFile(const QString &fileName,
         const SautoModel  &frequency,
         const INTERVAL_LIST &interval,
         const WEEK_DEF &week,
         const CALENDAR_DEF  &calender);

      bool readClockFile(const QString &fileName,
         SautoModel  &frequency,
         INTERVAL_LIST &interval,
         WEEK_DEF &week,
         CALENDAR_DEF &calender);

      void setCheckers(bool asap,
         bool allDay,
         bool everyDay,
         bool everyMonth);

      void getCheckers(bool &asap,
         bool &allDay,
         bool &everyDay,
         bool &everyMonth) const;

   private:
      CTreeBranch *createClockTree(const SautoModel  &frequency,
         const INTERVAL_LIST &interval,
         const WEEK_DEF &week,
         const CALENDAR_DEF  &calender);

      void populateFreqItem(CTreeBranch *branch, const SautoModel &freq);
      void populateClockItem(CTreeBranch *branch, INTERVAL_LIST intervals);
      void populateWeekItem(CTreeBranch *branch, WEEK_DEF week);
      void populateCalendarItem(CTreeBranch *branch, CALENDAR_DEF calendars);

      void readEntryElement(QXmlStreamReader *xmlReader, CTreeBranch *parent);
      void readModeElement(QXmlStreamReader *xmlReader, CTreeBranch *parent, const QString &termVal);
      void readNameElement(QXmlStreamReader *xmlReader, CTreeBranch *parent, const QString &termVal);
      void readFreqElement(QXmlStreamReader *xmlReader, CTreeBranch *parent, const QString &termVal);

      bool parseProcessTree(CTreeBranch  *root,
         SautoModel &frequency,
         INTERVAL_LIST &interval,
         WEEK_DEF &week,
         CALENDAR_DEF &calender);

      bool parseXmlItem(CTreeBranch *branch,
         SautoModel &frequency,
         INTERVAL_LIST &interval,
         WEEK_DEF &week,
         CALENDAR_DEF &calender);

      bool parseXmlTimeSettingsItem(CTreeBranch *branch,
         SautoModel &frequency,
         INTERVAL_LIST &interval,
         WEEK_DEF &week,
         CALENDAR_DEF &calender);

      bool extractFrequencyDefs(CTreeBranch *branch, SautoModel  &freq, bool isDefaultFreq = false);
      bool extractClockDefs(CTreeBranch *branch, INTERVAL_LIST &interval);
      bool extractWeekDefs(CTreeBranch *branch, WEEK_DEF &week);
      bool extractCalendarDefs(CTreeBranch *branch, CALENDAR_DEF &calendar);

   private:
      bool m_ASAPChecked;
      bool m_allDayChecked;
      bool m_everyDayChecked;
      bool m_everyMonthChecked;
      bool m_defClockSet;
      bool m_defWeekSet;
      bool m_defCalendarSet;
      bool m_daysHasCustomInterval;
      QString m_nameBuffer;
      QString m_buffer;
      QStringList m_freqBuffer;
   };

   bool verifyScheduleSettings(const SautoModel &freq, const INTERVAL_LIST &intr, const WEEK_DEF &week, const CALENDAR_DEF &cal, QString &report);
}

#endif