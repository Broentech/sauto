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
//  \file      megaCalendar.h
//
//  \brief     Definition of widget to specify all times
//
//  \author    Stian Broen
//
//  \date      17.07.2012
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

#ifndef _MEGA_CAL_H
#define _MEGA_CAL_H

// Qt includes
#include <QDialog>

// local includes
#include "monthSelector.h"

// Qt forward declarations
class QAction;
class QLabel;
class QPushButton;
class QVBoxLayout;

namespace sauto {
   class CMegaCal : public QDialog
   {
      Q_OBJECT

   public:
      explicit CMegaCal(QWidget *parent = 0);
      ~CMegaCal();
      void init();
      void setData(const CALENDAR_DEF &cal);
      void cancel();
      void updateWidgetContent();

   signals:
      void cancelled();
      void closeMe();
      void contentSignal(CALENDAR_DEF data);

   private slots:
      void previousYearClicked();
      void nextYearClicked();
      void OKClicked();
      void cancelClicked();

   private: 
      void initGUI();
      void updateYearActions();
      void addCalendar();

   private:
      QVBoxLayout *monthLay;
      QAction *m_prevYearAction;
      QAction *m_nextYearAction;
      QLabel *m_yearLabel;
      QPushButton *m_OK_button;
      QPushButton *m_Cancel_button;
      int currYear;
      int currIndex;
      QList<int> m_years;
      QMap<int, CMonthSelector *> m_allCalendars;
      CALENDAR_DEF m_data;
   };
}

#endif