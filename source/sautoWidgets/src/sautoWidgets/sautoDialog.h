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
//  \file      sautoDialog.h
//
//  \brief     Definition of a dialog class for specifying schedule
//
//  \author    Stian Broen
//
//  \date      27.11.2012
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

#ifndef _CLOCK_DIALOG_H
#define _CLOCK_DIALOG_H

// local includes
#include "megaCalendar.h"
#include "dialogBase.h"
#include "daySelector.h"

// Qt forward declarations
class QGroupBox;
class QGridLayout;
class QButtonGroup;
class QMenu;

namespace sauto {
   class CScheduleDialog : public CDialogBase
   {
      Q_OBJECT

   public:
      explicit CScheduleDialog(QWidget *parent = 0);
      ~CScheduleDialog();
      bool loadXml(const QString &filename);
      inline const SautoModel& getFreq() { return m_default_Frequency; }
      inline const INTERVAL_LIST& getIntervals() { return m_default_TimeIntervals; }
      inline const WEEK_DEF& getWeek() { return m_default_Week; }
      inline const CALENDAR_DEF& getCalendar() { return m_default_Calendar; }
      inline QString lastSavedFile() const { return m_lastSavedXML; }

   public slots:
      void updateWidgets();

   private slots:
      void fileSelectedFromBox(const QString &baseName);
      void acceptClicked();
      void clockButtonClicked(int id);
      void dayButtonClicked(int id);
      void weekButtonClicked(int id);
      void calButtonClicked(int id);
      void clockCancelled();
      void dayCancelled();
      void weekCancelled();
      void calCancelled();
      void receivedFreq(const SautoModel &data);
      void receiveTimeIntervals(INTERVAL_LIST intervals);
      void receiveWeek(WEEK_DEF week);
      void receiveCalendars(CALENDAR_DEF calendars);
      void setIsOnlySingleInterval(bool val);

   private:
      void initGUI();
      void resetGUI();
      bool saveToXmlFile(QString &path);
      QGridLayout* createTimeSettings();

   private:
      QToolButton *m_clockOn;
      QToolButton *m_clockOff;
      QToolButton *m_dayOn;
      QToolButton *m_dayOff;
      QToolButton *m_weekOn;
      QToolButton *m_weekOff;
      QToolButton *m_calOn;
      QToolButton *m_calOff;
      CWaveletWidget *m_freqEdit;
      CTimeIntervalWidget *m_timeIntervalWidget;
      CDaySelect *m_daySelect;
      CMegaCal *m_calendar;
      SautoModel m_default_Frequency;
      INTERVAL_LIST m_default_TimeIntervals;
      WEEK_DEF m_default_Week;
      CALENDAR_DEF m_default_Calendar;
      QString m_xmlpath;
   };
}

#endif