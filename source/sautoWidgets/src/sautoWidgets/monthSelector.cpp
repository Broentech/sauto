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
//  \file      monthSelector.cpp
//
//  \brief     Implementation of widget to specify months
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

// Qt includes
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QAction>
#include <QWidgetAction>
#include <QMenu>
#include <QToolBar>
#include <QDateTime>
#include <QButtonGroup>
#include <QActionGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>

// local includes
#include "monthSelector.h"

using namespace sauto;

CMonthSelector::CMonthSelector(int year, QWidget *parent)
   :QWidget(parent),
   m_activateButton(0),
   m_monthsGroupBox(0),
   m_includeMonthButtons(0),
   m_allInheritButtons(0),
   m_allCustomActions(0),
   jan_useCheck(0),
   feb_useCheck(0),
   mar_useCheck(0),
   apr_useCheck(0),
   may_useCheck(0),
   jun_useCheck(0),
   jul_useCheck(0),
   aug_useCheck(0),
   sep_useCheck(0),
   okt_useCheck(0),
   nov_useCheck(0),
   dec_useCheck(0),
   jan_nameLabel(0),
   feb_nameLabel(0),
   mar_nameLabel(0),
   apr_nameLabel(0),
   may_nameLabel(0),
   jun_nameLabel(0),
   jul_nameLabel(0),
   aug_nameLabel(0),
   sep_nameLabel(0),
   okt_nameLabel(0),
   nov_nameLabel(0),
   dec_nameLabel(0),
   jan_inheritCheck(0),
   feb_inheritCheck(0),
   mar_inheritCheck(0),
   apr_inheritCheck(0),
   may_inheritCheck(0),
   jun_inheritCheck(0),
   jul_inheritCheck(0),
   aug_inheritCheck(0),
   sep_inheritCheck(0),
   okt_inheritCheck(0),
   nov_inheritCheck(0),
   dec_inheritCheck(0),
   jan_customAction(0),
   feb_customAction(0),
   mar_customAction(0),
   apr_customAction(0),
   may_customAction(0),
   jun_customAction(0),
   jul_customAction(0),
   aug_customAction(0),
   sep_customAction(0),
   okt_customAction(0),
   nov_customAction(0),
   dec_customAction(0),
   m_calendar(0),
   m_calBox(0)
{
   if (year == -1)
   {
      // default : current year
      QDate now = QDateTime::currentDateTime().date();
      int loc_year = now.year();
      m_firstJan = QDate(loc_year, 1, 1);
   }
   else
   {
      m_firstJan = QDate(year, 1, 1);
   }

   initCalendars();
   initGUI();

   setCalendarMonth(static_cast<MONTH_ID>(QDate::currentDate().month()));
}

CMonthSelector::~CMonthSelector()
{

}

void CMonthSelector::refresh()
{
   m_activateButton->setChecked(true);
   m_monthsGroupBox->setEnabled(true);
   m_activateButton->setText(tr("Active - Click to Deactivate"));
   QDate now = QDateTime::currentDateTime().date();
   QMapIterator<MONTH_ID, QString> it(m_months);
   while (it.hasNext())
   {
      it.next();
      QDate checkDate(m_firstJan.year(), it.key(), 1);
      checkDate = checkDate.addDays(checkDate.daysInMonth() - 1);
      if (now > checkDate)
      {
         m_monthLabels.value(it.key())->setEnabled(false);
         m_monthCustom_actions.value(it.value())->setEnabled(false);
         m_monthUse_checkboxes.value(it.value())->setEnabled(false);
         m_monthInherit_checkboxes.value(it.value())->setEnabled(false);
      }
   }
}

void CMonthSelector::reset()
{
   QMapIterator<int, QLabel *> labelIt(m_monthLabels);
   while (labelIt.hasNext())
   {
      labelIt.next();
      labelIt.value()->setEnabled(true);
   }

   QHashIterator<QString, QAction *> actionIt(m_monthCustom_actions);
   while (actionIt.hasNext())
   {
      actionIt.next();
      actionIt.value()->setEnabled(true);
      actionIt.value()->setChecked(false);
   }

   QHashIterator<QString, QCheckBox *> monthIt(m_monthUse_checkboxes);
   while (monthIt.hasNext())
   {
      monthIt.next();
      monthIt.value()->setEnabled(true);
      monthIt.value()->setChecked(true);
   }

   QHashIterator<QString, QCheckBox *> inheritIt(m_monthInherit_checkboxes);
   while (inheritIt.hasNext())
   {
      inheritIt.next();
      inheritIt.value()->setEnabled(true);
      inheritIt.value()->setChecked(true);
   }

   m_calendar->reset();
   m_monthsGroupBox->setEnabled(false);
   m_activateButton->setChecked(false);
   m_activateButton->setText(tr("Not Active - Click to Activate"));
}

void CMonthSelector::setData(const MONTH_DEF &month)
{
   m_allMonths = month;

   QMutableHashIterator<QString, QCheckBox *> useIt(m_monthUse_checkboxes);
   while (useIt.hasNext())
   {
      useIt.next();
      if (m_allMonths.size() > 0)
      {
         useIt.value()->setChecked(false);
         m_monthCustom_actions.value(useIt.key())->setEnabled(false);
         m_monthInherit_checkboxes.value(useIt.key())->setEnabled(false);
         QMapIterator<MONTH_ID, QString> mIt(m_months);
         while (mIt.hasNext())
         {
            mIt.next();
            if (mIt.value() == useIt.key())
            {
               m_monthLabels.value(mIt.key())->setEnabled(false);
               break;
            }
         }
      }
      else
      {
         useIt.value()->setChecked(true);
      }
   }

   MONTH_ITERATOR it(m_allMonths);
   while (it.hasNext())
   {
      it.next();
      QString monthName = it.key();

      int labelKey = -1;
      QMapIterator<MONTH_ID, QString> m_months_it(m_months);
      while (m_months_it.hasNext())
      {
         m_months_it.next();
         if (m_months_it.value() == monthName)
         {
            labelKey = m_months_it.key();
            break;
         }
      }

      if (m_monthLabels.contains(labelKey))
      {
         m_monthLabels.value(labelKey)->setEnabled(true);
      }

      if (m_monthUse_checkboxes.contains(monthName))
      {
         m_monthUse_checkboxes.value(monthName)->setEnabled(true);
         m_monthUse_checkboxes.value(monthName)->setChecked(true);
      }

      bool doInherit = it.value().first;

      if (m_monthInherit_checkboxes.contains(monthName))
      {
         m_monthInherit_checkboxes.value(monthName)->setEnabled(true);
         m_monthInherit_checkboxes.value(monthName)->setChecked(doInherit);
      }
      if (m_monthCustom_actions.contains(monthName))
      {
         m_monthCustom_actions.value(monthName)->setEnabled(true);
         m_monthCustom_actions.value(monthName)->setChecked(!doInherit);
      }

      QMap<int, CALENDAR_DATE> days = it.value().second;
      m_calendar->setData(days);
   }
}

void CMonthSelector::initCalendars()
{
   m_months = makeYear();
   m_calendar = new CMultiDateCalendarWidget(this);

   connect(m_calendar, SIGNAL(dataSignal(const QMap<int, CALENDAR_DATE> &)),
      this, SLOT(receiveMonthData(const QMap<int, CALENDAR_DATE> &)));
}

void CMonthSelector::initGUI()
{
   m_activateButton = new QPushButton(this);
   m_activateButton->setCheckable(true);
   m_activateButton->setChecked(false);

   connect(m_activateButton, SIGNAL(clicked()),
      this, SLOT(activateClicked()));

   m_allInheritButtons = new QButtonGroup(this);
   m_allInheritButtons->setExclusive(false);

   connect(m_allInheritButtons, SIGNAL(buttonClicked(int)),
      this, SLOT(someInheritButtonClicked(int)));

   m_includeMonthButtons = new QButtonGroup(this);
   m_includeMonthButtons->setExclusive(false);

   connect(m_includeMonthButtons, SIGNAL(buttonClicked(int)),
      this, SLOT(someIncludeButtonClicked(int)));

   m_allCustomActions = new QActionGroup(this);
   m_allCustomActions->setExclusive(false);

   connect(m_allCustomActions, SIGNAL(triggered(QAction *)),
      this, SLOT(someCustomActionTriggered(QAction *)));

   QGridLayout *monthLay = new QGridLayout;

   // create labels
   QLabel *activeMonthLabel = new QLabel(tr("Active"), this);
   QLabel *monthNameLabel = new QLabel(tr("Month"), this);
   QLabel *inheritDaysLabel = new QLabel(tr("Inherit"), this);
   QLabel *customDaysLabel = new QLabel(tr("Custom"), this);
   monthLay->addWidget(activeMonthLabel, 0, 0);
   monthLay->addWidget(monthNameLabel, 0, 1);
   monthLay->addWidget(inheritDaysLabel, 0, 2);
   monthLay->addWidget(customDaysLabel, 0, 3);

   // create widgets
   createRow(JAN_STR, &jan_useCheck, &jan_nameLabel, &jan_inheritCheck, &jan_customAction, monthLay, 1);
   createRow(FEB_STR, &feb_useCheck, &feb_nameLabel, &feb_inheritCheck, &feb_customAction, monthLay, 2);
   createRow(MAR_STR, &mar_useCheck, &mar_nameLabel, &mar_inheritCheck, &mar_customAction, monthLay, 3);
   createRow(APR_STR, &apr_useCheck, &apr_nameLabel, &apr_inheritCheck, &apr_customAction, monthLay, 4);
   createRow(MAY_STR, &may_useCheck, &may_nameLabel, &may_inheritCheck, &may_customAction, monthLay, 5);
   createRow(JUN_STR, &jun_useCheck, &jun_nameLabel, &jun_inheritCheck, &jun_customAction, monthLay, 6);
   createRow(JUL_STR, &jul_useCheck, &jul_nameLabel, &jul_inheritCheck, &jul_customAction, monthLay, 7);
   createRow(AUG_STR, &aug_useCheck, &aug_nameLabel, &aug_inheritCheck, &aug_customAction, monthLay, 8);
   createRow(SEP_STR, &sep_useCheck, &sep_nameLabel, &sep_inheritCheck, &sep_customAction, monthLay, 9);
   createRow(OKT_STR, &okt_useCheck, &okt_nameLabel, &okt_inheritCheck, &okt_customAction, monthLay, 10);
   createRow(NOV_STR, &nov_useCheck, &nov_nameLabel, &nov_inheritCheck, &nov_customAction, monthLay, 11);
   createRow(DEC_STR, &dec_useCheck, &dec_nameLabel, &dec_inheritCheck, &dec_customAction, monthLay, 12);

   jan_customAction->setData(JANUARY);
   feb_customAction->setData(FEBRUARY);
   mar_customAction->setData(MARCH);
   apr_customAction->setData(APRIL);
   may_customAction->setData(MAY);
   jun_customAction->setData(JUNE);
   jul_customAction->setData(JULY);
   aug_customAction->setData(AUGUST);
   sep_customAction->setData(SEPTEMBER);
   okt_customAction->setData(OKTOBER);
   nov_customAction->setData(NOVEMBER);
   dec_customAction->setData(DECEMBER);

   QVBoxLayout *leftlay = new QVBoxLayout;
   leftlay->addWidget(m_activateButton);
   leftlay->addLayout(monthLay);

   m_monthsGroupBox = new QGroupBox(this);
   m_monthsGroupBox->setLayout(leftlay);
   m_monthsGroupBox->setTitle("Months");

   QVBoxLayout *calLay = new QVBoxLayout;
   calLay->addWidget(m_calendar);
   m_calBox = new QGroupBox(this);
   m_calBox->setLayout(calLay);
   //m_calBox->setTitle(tr("Intervals from Week definitions are inherited"));

   QHBoxLayout *totlay = new QHBoxLayout;
   totlay->addWidget(m_monthsGroupBox);
   totlay->addWidget(m_calBox);

   this->setLayout(totlay);

   activateClicked();
}

void CMonthSelector::activateClicked()
{
   if(m_activateButton->isChecked())
   {
      m_monthsGroupBox->setEnabled(true);
      m_activateButton->setText(tr("Active - Click to Deactivate"));
   }
   else
   {
      m_monthsGroupBox->setEnabled(false);
      m_activateButton->setText(tr("Not Active - Click to Activate"));
   }
}

void CMonthSelector::setActive(bool active)
{
   m_activateButton->setChecked(active);
   activateClicked();
}

bool CMonthSelector::getActive()
{
   return m_activateButton->isChecked();
}

MONTH_DEF CMonthSelector::requestData()
{
   return m_allMonths;
}

void CMonthSelector::createRow(
   const QString &name,
   QCheckBox **useBox,
   QLabel **nameLabel,
   QCheckBox **inheritBox,
   QAction **customAction,
   QGridLayout *lay,
   int row
   )
{
   delete *useBox; *useBox = 0;
   delete *nameLabel; *nameLabel = 0;
   delete *inheritBox; *inheritBox = 0;
   delete *customAction; *customAction = 0;

   QCheckBox *m_useBox = new QCheckBox(this);
   m_useBox->setChecked(false);
   m_monthUse_checkboxes.insert(name, m_useBox);
   m_includeMonthButtons->addButton(m_useBox, row);

   QLabel *m_nameLabel = new QLabel(this);
   m_nameLabel->setText(name);
   m_nameLabel->setEnabled(false);
   m_monthLabels.insert(row, m_nameLabel);

   QCheckBox *m_inheritBox = new QCheckBox(this);
   m_inheritBox->setChecked(true);
   m_inheritBox->setEnabled(false);
   m_monthInherit_checkboxes.insert(name, m_inheritBox);
   m_allInheritButtons->addButton(m_inheritBox, row);

   QAction *m_customAction = new QAction(this);
   m_customAction->setText(">");
   m_customAction->setCheckable(true);
   m_customAction->setChecked(false);
   m_customAction->setEnabled(false);
   m_monthCustom_actions.insert(name, m_customAction);

   QToolBar *actionBar = new QToolBar(this);
   actionBar->addAction(m_customAction);
   m_allCustomActions->addAction(m_customAction);

   QDate now = QDateTime::currentDateTime().date();
   QDate checkDate(m_firstJan.year(), row, 1);
   checkDate = checkDate.addDays(checkDate.daysInMonth() - 1);
   if (now > checkDate)
   {
      m_useBox->setEnabled(false);
      m_nameLabel->setEnabled(false);
      m_inheritBox->setEnabled(false);
      m_customAction->setEnabled(false);
   }

   lay->addWidget(m_useBox, row, 0);
   lay->addWidget(m_nameLabel, row, 1);
   lay->addWidget(m_inheritBox, row, 2);
   lay->addWidget(actionBar, row, 3);

   *useBox = m_useBox;
   *nameLabel = m_nameLabel;
   *inheritBox = m_inheritBox;
   *customAction = m_customAction;
}

void CMonthSelector::someInheritButtonClicked(int id)
{
   bool reverseChecked = !m_allInheritButtons->button(id)->isChecked();
   QList<QAction *> actionList = m_allCustomActions->actions();
   if(actionList.count() > (id - 1))
   {
      QAction *action = actionList.at(id - 1);
      if(0 != action)
      {
         action->setChecked(reverseChecked);
         QVariant data = action->data();
         int dataInt = data.toInt();
         MONTH_ID loc_month = static_cast<MONTH_ID>(dataInt);

         setMonthData_inherit(loc_month, !reverseChecked);
         if(reverseChecked)
         {   
            setCalendarMonth(loc_month);
         }
         else
         {
            //m_calBox->setTitle("Intervals from Week definitions are inherited");
            m_calendar->setValidDateRange(QDate(), QDate());
         }
      }
   }
}

void CMonthSelector::setMonthData_inherit(MONTH_ID month, bool isInherited)
{
   if(!m_months.contains(month))
   {
      return;
   }
   QString monthStr = m_months.value(month);
   if(!m_allMonths.contains(monthStr))
   {
      return;
   }

   DAY_OF_MONTH_DEF days = m_allMonths[monthStr];
   days.first = isInherited;
   m_allMonths.insert(monthStr, days);
}

void CMonthSelector::someIncludeButtonClicked(int id)
{
   QAbstractButton *button = m_includeMonthButtons->button(id);
   if (button == 0)
   {
      return;
   }

   MONTH_ID mID = static_cast<MONTH_ID>(id);
   QString currMonth = m_months.value(mID);
   QString lastSelectedMonth = m_calBox->title();
   m_calBox->setTitle(currMonth);

   bool monthIsEnabled = button->isChecked();
   m_allInheritButtons->button(id)->setEnabled(monthIsEnabled);
   m_monthLabels.value(id)->setEnabled(monthIsEnabled);
   m_allCustomActions->actions().at(id - 1)->setEnabled(monthIsEnabled);

   if (monthIsEnabled)
   {
      bool monthIsInheritFromWeek = m_allInheritButtons->button(id)->isChecked();
      DAY_OF_MONTH_DEF daysDef = m_allMonths.value(currMonth);
      daysDef.first = monthIsInheritFromWeek;
      QMap<int, CALENDAR_DATE> data = daysDef.second;

      if (lastSelectedMonth == currMonth)
      {
         data = m_calendar->getMonthDescription();
         if (data.size() > 0)
         {
            updateMonthColor(mID, true);
         }
         else
         {
            updateMonthColor(mID, false);
         }
      }
      else
      {
         updateMonthColor(mID, false);
      }
      daysDef.second = data;
      m_allMonths.insert(currMonth, daysDef);
   }
   else
   {
      updateMonthColor(mID, false);
      if (m_allMonths.contains(currMonth))
      {
         m_allMonths.remove(currMonth);
      }
   }
}

void CMonthSelector::someCustomActionTriggered(QAction *action)
{
   int index = 0;
   bool checkState;
   QHashIterator<QString, QAction*> it(m_monthCustom_actions);
   while (it.hasNext())
   {
      it.next();
      QAction *itaction = it.value();
      if (itaction == action)
      {
         checkState = itaction->isChecked();
         QVariant data = itaction->data();
         index = data.toInt();
         MONTH_ID loc_month = static_cast<MONTH_ID>(index);
         setMonthData_inherit(loc_month, !checkState);
         if (checkState)
         {
            setCalendarMonth(loc_month);
         }
         else
         {
            //m_calBox->setTitle(QString("Intervals from Week definitions are inherited for %1").arg(m_months.value(loc_month)));
            m_calendar->setValidDateRange(QDate(), QDate());
         }
         break;
      }
   }

   QAbstractButton *inheritButton = m_allInheritButtons->button(index);
   if (0 != inheritButton)
   {
      inheritButton->setChecked(!checkState);
      inheritButton = 0;
   }
}

void CMonthSelector::setCalendarMonth(MONTH_ID month)
{
   QDate startDate = m_firstJan.addMonths(month - 1);
   QDate stopDate = startDate.addDays(startDate.daysInMonth() - 1);
   m_calendar->setValidDateRange(startDate, stopDate);
   m_calBox->setTitle(m_months.value(month));

   if (m_months.contains(month))
   {
      QString monthStr = m_months.value(month);
      if (m_allMonths.contains(monthStr))
      {
         DAY_OF_MONTH_DEF days = m_allMonths.value(monthStr);
         if (!days.first && days.second.size() > 0)
         {
            m_calendar->setData(days.second);
         }
      }
   }
}

void CMonthSelector::receiveMonthData(const QMap<int, CALENDAR_DATE> &data)
{
   QString currMonth = m_calBox->title();
   DAY_OF_MONTH_DEF days;
   days.second = data;
   if(data.size() <= 0)
   {
      days.first = true; // inherit
   }
   else
   {
      days.first = false; // don't inherit
   }
   m_allMonths.insert(currMonth, days);
   updateMonthColor(currMonth, !days.first);
}

void CMonthSelector::updateMonthColor(MONTH_ID month, bool hasData)
{
   int labelID = static_cast<int>(month);
   QLabel *label = 0;
   if(!m_monthLabels.contains(labelID))
   {
      return;
   }

   label = m_monthLabels.value(labelID);
   QString name = m_months.value(month);
   if(hasData)
   {
      label->setText(QString("<b><font color=green>%1</font></b>").arg(name));
   }
   else
   {
      label->setText(QString("<font color=black>%1</font>").arg(name));
   }
}

void CMonthSelector::updateMonthColor(const QString &monthName, bool hasData)
{
   MONTH_ID month = getMonthStringAsID(monthName);
   updateMonthColor(month, hasData);
}

void CMonthSelector::updateWidgetContent()
{
   m_calendar->updateWidgetContent();
}