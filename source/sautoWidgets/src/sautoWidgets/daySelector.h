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
//  \file      daySelector.h
//
//  \brief     Definition of widget to specify days
//
//  \author    Stian Broen
//
//  \date      01.07.2012
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

#ifndef _DAY_SELECTOR_H
#define _DAY_SELECTOR_H

// Qt includes
#include <QDialog>

// solution includes
#include <sautoModel/sautoDefs.h>

// local includes
#include "timeIntervals.h"

// Qt forward declarations
class QGroupBox;
class QPushButton;
class QCheckBox;
class QLabel;
class QToolBar;
class QGridLayout;
class QActionGroup;
class QWidgetAction;
class QButtonGroup;

namespace sauto {
   class CDaySelect : public QDialog
   {
      Q_OBJECT

   public:
      explicit CDaySelect(QWidget *parent = 0);
      ~CDaySelect();
      void init();
      void setData(const WEEK_DEF &someWeek, bool doUnselectAll = true);
      void cancel();
      void updateWidgetContent();

   signals:
      void cancelled();
      void closeMe();
      void weekDescription(WEEK_DEF week);

   private slots:
      void OKClicked();
      void cancelClicked();
      void customeTimeActionTriggered(QAction *action);
      void someEnableChecked(int id);
      void someInheritChecked(int id);
      void closeInterval();
      void receiveIntervals(const INTERVAL_LIST &intervals);

   private:
      void initGUI();
      void unselectAll();
      void selectAll();
      void initCustomTimeWidgets();
      void initDayRow(
         QString dayText,
         QCheckBox **useCheck,
         QLabel **label,
         QCheckBox **inheritCheck,
         QAction **timeAction,
         QToolBar *toolBar,
         QGridLayout *layout,
         int row);
      QGroupBox* makeDayTable();

   private:
      DAYS m_currDay;
      WEEK_DEF m_data;
      QGroupBox *intervalTable;
      CTimeIntervalWidget *m_intervalWidget;
      QPushButton *OKButton;
      QPushButton *cancelButton;
      QCheckBox *m_monday_check;
      QCheckBox *m_tuesday_check;
      QCheckBox *m_wednesday_check;
      QCheckBox *m_thursday_check;
      QCheckBox *m_friday_check;
      QCheckBox *m_saturday_check;
      QCheckBox *m_sunday_check;
      QLabel *m_mondayLabel;
      QLabel *m_tuesdayLabel;
      QLabel *m_wednesdayLabel;
      QLabel *m_thursdayLabel;
      QLabel *m_fridayLabel;
      QLabel *m_saturdayLabel;
      QLabel *m_sundayLabel;
      QCheckBox *m_monday_defaultTime;
      QCheckBox *m_tuesday_defaultTime;
      QCheckBox *m_wedneday_defaultTime;
      QCheckBox *m_thursday_defaultTime;
      QCheckBox *m_friday_defaultTime;
      QCheckBox *m_saturday_defaultTime;
      QCheckBox *m_sunday_defaultTime;
      QAction *m_monday_customTimeAction;
      QAction *m_tuesday_customTimeAction;
      QAction *m_wednesday_customTimeAction;
      QAction *m_thursday_customTimeAction;
      QAction *m_friday_customTimeAction;
      QAction *m_saturday_customTimeAction;
      QAction *m_sunday_customTimeAction;
      QMap<DAYS, QAction *> m_actionMap;
      QMap<DAYS, QCheckBox *> m_defaultChecks;
      QMap<DAYS, QLabel *> m_labels;
      QMap<DAYS, QCheckBox *> m_enableChecks;
      QButtonGroup *m_defaultButtons;
      QButtonGroup *m_enableButtons;
      QActionGroup *m_actions;
   };
}

#endif