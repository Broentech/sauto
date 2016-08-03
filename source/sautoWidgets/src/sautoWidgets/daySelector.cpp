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
//  \file      daySelector.cpp
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

// Qt includes
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QAction>
#include <QActionGroup>
#include <QButtonGroup>
#include <QToolBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QWidgetAction>
#include <QtDebug>

// local includes
#include "daySelector.h"

using namespace sauto;

CDaySelect::CDaySelect(QWidget *parent)
   :QDialog(parent),
   intervalTable(0),
   m_intervalWidget(0),
   OKButton(0),
   cancelButton(0),
   m_monday_check(0),
   m_tuesday_check(0),
   m_wednesday_check(0),
   m_thursday_check(0),
   m_friday_check(0),
   m_saturday_check(0),
   m_sunday_check(0),
   m_mondayLabel(0),
   m_tuesdayLabel(0),
   m_wednesdayLabel(0),
   m_thursdayLabel(0),
   m_fridayLabel(0),
   m_saturdayLabel(0),
   m_sundayLabel(0),
   m_monday_defaultTime(0),
   m_tuesday_defaultTime(0),
   m_wedneday_defaultTime(0),
   m_thursday_defaultTime(0),
   m_friday_defaultTime(0),
   m_saturday_defaultTime(0),
   m_sunday_defaultTime(0),
   m_monday_customTimeAction(0),
   m_tuesday_customTimeAction(0),
   m_wednesday_customTimeAction(0),
   m_thursday_customTimeAction(0),
   m_friday_customTimeAction(0),
   m_saturday_customTimeAction(0),
   m_sunday_customTimeAction(0),
   m_defaultButtons(0),
   m_enableButtons(0),
   m_actions(0)
{
   m_data.clear();
}

void CDaySelect::init()
{
   initGUI();
}

CDaySelect::~CDaySelect()
{
   delete m_intervalWidget;
}

void CDaySelect::unselectAll()
{
   QMapIterator<DAYS, QCheckBox *> enaIt(m_enableChecks);
   while(enaIt.hasNext())
   {
      enaIt.next();
      QCheckBox *check = enaIt.value();
      if(0 != check)
      {
         check->setChecked(false); //< uncheck all days
      }
   }

   QMapIterator<DAYS, QLabel *> labIt(m_labels);
   while(labIt.hasNext())
   {
      labIt.next();
      QLabel *label = labIt.value();
      if(0 != label)
      {
         label->setEnabled(false); //< disable all labels
      }
   }

   QMapIterator<DAYS, QCheckBox *> defIt(m_defaultChecks);
   while(defIt.hasNext())
   {
      defIt.next();
      QCheckBox *check = defIt.value();
      if(0 != check)
      {
         check->setChecked(true); //< set all days to inherited
         check->setEnabled(false);
      }
   }

   QMapIterator<DAYS, QAction *> actIt(m_actionMap);
   while(actIt.hasNext())
   {
      actIt.next();
      QAction *action = actIt.value();
      if(0 != action)
      {
         action->setChecked(false); //< uncheck all actions
         action->setEnabled(false);
      }
   }

   MUT_WEEK_ITERATOR it(m_data);
   while(it.hasNext())
   {
      it.next();
      DAY_OF_WEEK_DEF day = it.value();
      day.first.first = false;
      day.first.second = true;
      it.setValue(day);
   }
}

void CDaySelect::selectAll()
{
   QMapIterator<DAYS, QCheckBox *> enaIt(m_enableChecks);
   while(enaIt.hasNext())
   {
      enaIt.next();
      QCheckBox *check = enaIt.value();
      if(0 != check)
      {
         check->setChecked(true); //< check all days
      }
   }

   QMapIterator<DAYS, QLabel *> labIt(m_labels);
   while(labIt.hasNext())
   {
      labIt.next();
      QLabel *label = labIt.value();
      if(0 != label)
      {
         label->setEnabled(true); //< enable all labels
      }
   }

   QMapIterator<DAYS, QCheckBox *> defIt(m_defaultChecks);
   while(defIt.hasNext())
   {
      defIt.next();
      QCheckBox *check = defIt.value();
      if(0 != check)
      {
         check->setChecked(true); //< set all days to inherited
         check->setEnabled(true);
      }
   }

   QMapIterator<DAYS, QAction *> actIt(m_actionMap);
   while(actIt.hasNext())
   {
      actIt.next();
      QAction *action = actIt.value();
      if(0 != action)
      {
         action->setChecked(false); //< uncheck all actions
         action->setEnabled(true);
      }
   }

   MUT_WEEK_ITERATOR it(m_data);
   while(it.hasNext())
   {
      it.next();
      DAY_OF_WEEK_DEF day = it.value();
      day.first.first = true;
      day.first.second = true;
      it.setValue(day);
   }
}

void CDaySelect::setData(const WEEK_DEF &someWeek, bool doUnselectAll)
{
   m_data = makeEmptyWeekData();
   if (doUnselectAll)
   {
      unselectAll();
   }

   MUT_WEEK_ITERATOR mutit(m_data);
   while (mutit.hasNext())
   {
      mutit.next();
      int day = mutit.key();
      if (someWeek.contains(day))
      {
         mutit.setValue(someWeek.value(day));
      }
   }

   WEEK_ITERATOR it(m_data);
   while (it.hasNext())
   {
      it.next();
      int daysNo = it.key();
      DAYS dayID = static_cast<DAYS>(daysNo);
      DAY_OF_WEEK_DEF dayData = it.value();
      bool isToggled = dayIsToggled(dayData);
      bool isInherit = dayInheritsTime(dayData);
      if (m_actionMap.contains(dayID))
      {
         QAction *action = m_actionMap[dayID];
         if (0 != action)
         {
            action->setChecked(!isInherit);
            action->setEnabled(isToggled);
            action = 0;
         }
      }
      if (m_defaultChecks.contains(dayID))
      {
         QCheckBox *box = m_defaultChecks[dayID];
         if (0 != box)
         {
            box->setChecked(isInherit);
            box->setEnabled(isToggled);
            box = 0;
         }
      }
      if (m_labels.contains(dayID))
      {
         QLabel *label = m_labels[dayID];
         if (0 != label)
         {
            label->setEnabled(isToggled);
            label = 0;
         }
      }
      if (m_enableChecks.contains(dayID))
      {
         QCheckBox *box = m_enableChecks[dayID];
         if (0 != box)
         {
            box->setChecked(isToggled);
            box = 0;
         }
      }
   }

   QMapIterator<DAYS, QAction *> j(m_actionMap);
   while (j.hasNext())
   {
      j.next();
      QAction *action = j.value();
      if (action->isChecked())
      {
         customeTimeActionTriggered(action);
         break;
      }
   }
}

void CDaySelect::cancel()
{
   if(m_data.size() > 0)
   {
      setData(m_data);
   }
   else
   {
      m_intervalWidget->cancel();
      selectAll();
   }
}

void CDaySelect::initGUI()
{
   OKButton = new QPushButton(this);
   cancelButton = new QPushButton(this);
   OKButton->setText(tr("OK"));
   cancelButton->setText(tr("Cancel"));

   connect(OKButton, SIGNAL(clicked()),
      this, SLOT(OKClicked()));

   connect(cancelButton, SIGNAL(clicked()),
      this, SLOT(cancelClicked()));

   QHBoxLayout *buttonLay = new QHBoxLayout;
   buttonLay->addWidget(OKButton);
   buttonLay->addWidget(cancelButton);
   buttonLay->addStretch();

   QGroupBox *dayTable = makeDayTable();
   dayTable->setAlignment(Qt::AlignHCenter);
   dayTable->setTitle(tr("Week Days"));

   intervalTable = new QGroupBox(this);
   intervalTable->setTitle("N/A");
   QHBoxLayout *intLay = new QHBoxLayout;
   intLay->addWidget(m_intervalWidget);
   intervalTable->setLayout(intLay);
   intervalTable->setAlignment(Qt::AlignHCenter);
   intervalTable->setEnabled(false);

   QHBoxLayout *widgLay = new QHBoxLayout;
   widgLay->addWidget(dayTable);
   widgLay->addWidget(intervalTable);

   QVBoxLayout *totalLay = new QVBoxLayout;
   totalLay->addLayout(widgLay);
   totalLay->addLayout(buttonLay);

   this->setLayout(totalLay);
}

void CDaySelect::initDayRow(
   const QString  dayText,
   QCheckBox **useCheck,
   QLabel **label,
   QCheckBox **inheritCheck,
   QAction **timeAction,
   QToolBar *toolBar,
   QGridLayout *layout,
   int row
   )
{
   delete *useCheck; *useCheck = 0;
   delete *label; *label = 0;
   delete *inheritCheck; *inheritCheck = 0;
   delete *timeAction; *timeAction = 0;

   DAYS day = static_cast<DAYS>(row + 1);

   QCheckBox *m_useCheck = new QCheckBox(this);
   m_useCheck->setChecked(true);
   m_enableChecks.insert(day, m_useCheck);
   m_enableButtons->addButton(m_useCheck, row + 1);

   QLabel *m_label = new QLabel(this);
   m_label->setText(dayText);
   m_labels.insert(day, m_label);

   QCheckBox *m_inheritCheck = new QCheckBox(this);
   m_inheritCheck->setChecked(true);
   m_defaultChecks.insert(day, m_inheritCheck);
   m_defaultButtons->addButton(m_inheritCheck, row + 1);

   QAction *m_timeAction = new QAction(this);
   m_timeAction->setData(day);
   m_timeAction->setText(tr(">"));
   m_timeAction->setCheckable(true);
   m_timeAction->setChecked(false);
   m_actions->addAction(m_timeAction);
   m_actionMap.insert(day, m_timeAction);

   toolBar->addAction(m_timeAction);

   layout->addWidget(m_useCheck, row, 0);
   layout->addWidget(m_label, row, 1);
   layout->addWidget(m_inheritCheck, row, 2);

   *useCheck = m_useCheck;
   *label = m_label;
   *inheritCheck = m_inheritCheck;
   *timeAction = m_timeAction;
}

QGroupBox* CDaySelect::makeDayTable()
{
   QToolBar *m_timeDefsBar = new QToolBar(this);
   m_actions = new QActionGroup(this);
   QGridLayout *allWeekWidgetsLay = new QGridLayout;

   m_enableButtons = new QButtonGroup(this);
   m_enableButtons->setExclusive(false);

   connect(m_enableButtons, SIGNAL(buttonClicked(int)),
      this, SLOT(someEnableChecked(int)));

   m_defaultButtons = new QButtonGroup(this);
   m_defaultButtons->setExclusive(false);

   connect(m_defaultButtons, SIGNAL(buttonClicked(int)),
      this, SLOT(someInheritChecked(int)));

   // setup custom time intervals
   initCustomTimeWidgets();

   initDayRow(MON_STR,
      &m_monday_check,
      &m_mondayLabel,
      &m_monday_defaultTime,
      &m_monday_customTimeAction,
      m_timeDefsBar,
      allWeekWidgetsLay,
      0);

   initDayRow(TUE_STR,
      &m_tuesday_check,
      &m_tuesdayLabel,
      &m_tuesday_defaultTime,
      &m_tuesday_customTimeAction,
      m_timeDefsBar,
      allWeekWidgetsLay,
      1);

   initDayRow(WED_STR,
      &m_wednesday_check,
      &m_wednesdayLabel,
      &m_wedneday_defaultTime,
      &m_wednesday_customTimeAction,
      m_timeDefsBar,
      allWeekWidgetsLay,
      2);

   initDayRow(THU_STR,
      &m_thursday_check,
      &m_thursdayLabel,
      &m_thursday_defaultTime,
      &m_thursday_customTimeAction,
      m_timeDefsBar,
      allWeekWidgetsLay,
      3);

   initDayRow(FRI_STR,
      &m_friday_check,
      &m_fridayLabel,
      &m_friday_defaultTime,
      &m_friday_customTimeAction,
      m_timeDefsBar,
      allWeekWidgetsLay,
      4);

   initDayRow(SAT_STR,
      &m_saturday_check,
      &m_saturdayLabel,
      &m_saturday_defaultTime,
      &m_saturday_customTimeAction,
      m_timeDefsBar,
      allWeekWidgetsLay,
      5);

   initDayRow(SUN_STR,
      &m_sunday_check,
      &m_sundayLabel,
      &m_sunday_defaultTime,
      &m_sunday_customTimeAction,
      m_timeDefsBar,
      allWeekWidgetsLay,
      6);

   QLabel *includeHeader = new QLabel(tr("Include"), this);
   QLabel *dayHeader = new QLabel(tr("Day"), this);
   QLabel *inheritTimeHeader = new QLabel(tr("Inherit"), this);
   QLabel *customTimeHeader = new QLabel(tr("Custom"), this);

   m_timeDefsBar->setOrientation(Qt::Vertical);

   connect(m_timeDefsBar, SIGNAL(actionTriggered(QAction *)),
      this, SLOT(customeTimeActionTriggered(QAction *)));

   QVBoxLayout *timeDefsLay = new QVBoxLayout;
   timeDefsLay->addWidget(m_timeDefsBar);

   QHBoxLayout *tableLay = new QHBoxLayout;
   tableLay->addLayout(allWeekWidgetsLay);
   tableLay->addLayout(timeDefsLay);

   QHBoxLayout *headersLay = new QHBoxLayout;
   headersLay->addWidget(includeHeader);
   headersLay->addWidget(dayHeader);
   headersLay->addWidget(inheritTimeHeader);
   headersLay->addWidget(customTimeHeader);

   QVBoxLayout *totlay = new QVBoxLayout;
   totlay->addLayout(headersLay);
   totlay->addStretch();
   totlay->addLayout(tableLay);
   QGroupBox *table = new QGroupBox(this);
   table->setLayout(totlay);
   return table;
}

void CDaySelect::initCustomTimeWidgets()
{
   m_intervalWidget = new CTimeIntervalWidget(this);
   m_intervalWidget->init();

   connect(m_intervalWidget, SIGNAL(closeMe()),
      this, SLOT(closeInterval()));

   connect(m_intervalWidget, SIGNAL(timeIntervals(const INTERVAL_LIST &)),
      this, SLOT(receiveIntervals(const INTERVAL_LIST &)));

   m_data = makeEmptyWeekData();
}

void CDaySelect::closeInterval()
{
   INTERVAL_LIST emptyList;
   m_intervalWidget->setData(emptyList);
   m_intervalWidget->cancel();
   intervalTable->setTitle("N/A");
   intervalTable->setEnabled(false);
}

void CDaySelect::receiveIntervals(const INTERVAL_LIST &intervals)
{
   bool dayToggled = false;
   bool dayInherited = false;
   switch(m_currDay)
   {
   case(MONDAY):
      dayToggled   = m_monday_check      ->isChecked();
      dayInherited = m_monday_defaultTime->isChecked();
      break;

   case(TUESDAY):
      dayToggled   = m_tuesday_check      ->isChecked();
      dayInherited = m_tuesday_defaultTime->isChecked();
      break;

   case(WEDNESDAY):
      dayToggled   = m_wednesday_check     ->isChecked();
      dayInherited = m_wedneday_defaultTime->isChecked();
      break;

   case(THURSDAY):
      dayToggled   = m_thursday_check      ->isChecked();
      dayInherited = m_thursday_defaultTime->isChecked();
      break;

   case(FRIDAY):
      dayToggled   = m_friday_check      ->isChecked();
      dayInherited = m_friday_defaultTime->isChecked();
      break;

   case(SATRUDAY):
      dayToggled   = m_saturday_check      ->isChecked();
      dayInherited = m_saturday_defaultTime->isChecked();
      break;

   case(SUNDAY):
      dayToggled   = m_sunday_check      ->isChecked();
      dayInherited = m_sunday_defaultTime->isChecked();
      break;

   case(NO_SPECIFIC_DAY):
   default:
      qCritical() << QString("Undefined day");
      return;
   }

   if(m_labels.contains(m_currDay))
   {
      QLabel *label = m_labels.value(m_currDay);
      if(0 != label)
      {
         label->setTextFormat(Qt::PlainText);
         QString labelTxt = label->text();
         if(dayToggled && !dayInherited)
         {
            label->setTextFormat(Qt::RichText);
            label->setText(QString("<b><font color=green>%1</font></b>").arg(labelTxt));
         }
         else
         {
            label->setText(labelTxt);
         }
         label = 0;
      }
   }

   DAY_OF_WEEK_DEF dayDef;
   dayDef.first.first = dayToggled;
   dayDef.first.second = dayInherited;
   dayDef.second = intervals;
   m_data.insert(m_currDay, dayDef);
}

void CDaySelect::someEnableChecked(int id)
{
   DAYS day     = static_cast<DAYS>(id);
   QString dayStr          = getDayIDasString(day);
   QLabel *label           = 0;
   QAction *action         = 0;
   QCheckBox *defaultCheck = 0;
   QAbstractButton *enableCheck = m_enableButtons->button(id);
   if(m_labels.contains(day))
   {
      label = m_labels.value(day);
   }
   else
   {
      return;
   }

   if(m_actionMap.contains(day))
   {
      action = m_actionMap.value(day);
   }
   else
   {
      return;
   }

   if(m_defaultChecks.contains(day))
   {
      defaultCheck = m_defaultChecks.value(day);
   }
   else
   {
      return;
   }

   bool qstate = enableCheck->isChecked();
   if(qstate == false)
   {
      action      ->setEnabled(false); 
      defaultCheck->setEnabled(false);
      label       ->setEnabled(false);
      m_data[day].first.first = false ; //< is NOT enabled

      closeInterval();
      if(0 != label)
      {
         label->setTextFormat(Qt::PlainText);
         label->setText(dayStr);
      }
   }
   else
   {
      action      ->setEnabled(true); 
      defaultCheck->setEnabled(true);
      label       ->setEnabled(true);
      m_data[day].first.first = true; // is enabled

      if(0 != label && m_data[day].second.size() > 0)
      {
         if(defaultCheck->isChecked())
         {
            label->setTextFormat(Qt::PlainText);
            label->setText(dayStr);
            closeInterval();
         }
         else
         {
            label->setTextFormat(Qt::RichText);
            label->setText(QString("<b><font color=green>%1</font></b>").arg(dayStr));
            customeTimeActionTriggered(action);
         }
      }
   }
}

void CDaySelect::someInheritChecked(int id)
{
   DAYS day = static_cast<DAYS>(id);
   QAbstractButton *inheritCheck = m_defaultButtons->button(id);
   if(0 == inheritCheck)
   {
      return;
   }

   if(!m_actionMap.contains(day))
   {
      return;
   }
   QAction *action = m_actionMap.value(day);
   if(0 == action)
   {
      return;
   }
   
   if(inheritCheck->isChecked())
   {
      action->setChecked(false); 
      m_data[day].first.first = true; //< is enabled
      m_data[day].first.second = true; //< is inherited
      closeInterval();
   }
   else
   {
      action->setChecked(true);
      m_data[day].first.first = true ; //< is enabled
      m_data[day].first.second = false; //< is NOT inherited, make custom intervals
      customeTimeActionTriggered(action);
   }
}

void CDaySelect::customeTimeActionTriggered(QAction *action)
{
   int dayInt = action->data().toInt();
   if(dayInt <= 0 || dayInt > 7)
   {
      return;
   }
   DAYS day = static_cast<DAYS>(dayInt);
   if(!m_defaultChecks.contains(day))
   {
      return;
   }
   m_currDay = day;
   QCheckBox *inheritCheck = m_defaultChecks.value(day);
   QString dayStr = getDayIDasString(day);
   if(action->isChecked())
   {
      inheritCheck->setChecked(false);
      if(m_data.contains(day))
      {
         m_data[day].first.first = true ; //< is enabled
         m_data[day].first.second = false; //< is NOT inherited 
         m_intervalWidget->setData(m_data[day].second);
      }
      intervalTable->setTitle(QString("%1 definition").arg(dayStr));
      intervalTable->setEnabled(true);
   }
   else
   {
      if(m_data.contains(day))
      {
         m_data[day].first.first = true; //< is enabled
         m_data[day].first.second = true; //< is inherited 
      }
      closeInterval();
      inheritCheck->setChecked(true);
   }
}

void CDaySelect::OKClicked()
{
   emit weekDescription(m_data);
}

void CDaySelect::cancelClicked()
{
   emit cancelled();
   this->close();
}

void CDaySelect::updateWidgetContent()
{
   m_intervalWidget->updateWidgetContent();
}