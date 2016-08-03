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
//  \file      monthSelector.h
//
//  \brief     Definition of widget to specify months
//
//  \author    Stian Broen
//
//  \date      03.07.2012
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

#ifndef _MONTH_SELECTOR_H
#define _MONTH_SELECTOR_H

// Qt includes
#include <QWidget>
#include <QString>

// solution includes
#include <sautoModel/sautoDefs.h>

// local includes
#include "multiDateCalendar.h"

// Qt forward declarations
class QPushButton;
class QCheckBox;
class QLabel;
class QAction;
class QGridLayout;
class QButtonGroup;
class QActionGroup;
class QGroupBox;

namespace sauto {
   class CMonthSelector : public QWidget
   {
      Q_OBJECT

   public:
      explicit CMonthSelector(int year = -1, QWidget *parent = 0);
      ~CMonthSelector();
      void refresh();
      void reset();
      void setData(const MONTH_DEF &month);
      MONTH_DEF requestData();
      void setActive(bool active = true);
      bool getActive();
      void updateWidgetContent();

   private slots:
      void receiveMonthData(const QMap<int, CALENDAR_DATE> &data);
      void activateClicked();
      void someInheritButtonClicked(int id);
      void someIncludeButtonClicked(int id);
      void someCustomActionTriggered(QAction *action);

   private: // functions
      void initGUI();
      void initCalendars();
      void setMonthData_inherit(MONTH_ID month, bool isInherited);
      void setCalendarMonth(MONTH_ID month);
      void updateMonthColor(MONTH_ID month, bool hasData);
      void updateMonthColor(const QString &monthName, bool hasData);
      void createRow(
         const QString &name,
         QCheckBox **useBox,
         QLabel **nameLabel,
         QCheckBox **inheritBox,
         QAction **customAction,
         QGridLayout *lay,
         int row
         );

   private:
      QPushButton *m_activateButton;
      QGroupBox *m_monthsGroupBox;
      QButtonGroup *m_includeMonthButtons;
      QButtonGroup *m_allInheritButtons;
      QActionGroup *m_allCustomActions;
      QCheckBox *jan_useCheck;
      QCheckBox *feb_useCheck;
      QCheckBox *mar_useCheck;
      QCheckBox *apr_useCheck;
      QCheckBox *may_useCheck;
      QCheckBox *jun_useCheck;
      QCheckBox *jul_useCheck;
      QCheckBox *aug_useCheck;
      QCheckBox *sep_useCheck;
      QCheckBox *okt_useCheck;
      QCheckBox *nov_useCheck;
      QCheckBox *dec_useCheck;
      QLabel *jan_nameLabel;
      QLabel *feb_nameLabel;
      QLabel *mar_nameLabel;
      QLabel *apr_nameLabel;
      QLabel *may_nameLabel;
      QLabel *jun_nameLabel;
      QLabel *jul_nameLabel;
      QLabel *aug_nameLabel;
      QLabel *sep_nameLabel;
      QLabel *okt_nameLabel;
      QLabel *nov_nameLabel;
      QLabel *dec_nameLabel;
      QCheckBox *jan_inheritCheck;
      QCheckBox *feb_inheritCheck;
      QCheckBox *mar_inheritCheck;
      QCheckBox *apr_inheritCheck;
      QCheckBox *may_inheritCheck;
      QCheckBox *jun_inheritCheck;
      QCheckBox *jul_inheritCheck;
      QCheckBox *aug_inheritCheck;
      QCheckBox *sep_inheritCheck;
      QCheckBox *okt_inheritCheck;
      QCheckBox *nov_inheritCheck;
      QCheckBox *dec_inheritCheck;
      QAction *jan_customAction;
      QAction *feb_customAction;
      QAction *mar_customAction;
      QAction *apr_customAction;
      QAction *may_customAction;
      QAction *jun_customAction;
      QAction *jul_customAction;
      QAction *aug_customAction;
      QAction *sep_customAction;
      QAction *okt_customAction;
      QAction *nov_customAction;
      QAction *dec_customAction;
      CMultiDateCalendarWidget *m_calendar;
      QGroupBox *m_calBox;
      QMap<int, QLabel *> m_monthLabels;
      QHash<QString, QAction *> m_monthCustom_actions;
      QHash<QString, QCheckBox *> m_monthUse_checkboxes;
      QHash<QString, QCheckBox *> m_monthInherit_checkboxes;
      QDate m_firstJan;
      QMap<MONTH_ID, QString> m_months;
      MONTH_DEF m_allMonths;
   };
}

#endif