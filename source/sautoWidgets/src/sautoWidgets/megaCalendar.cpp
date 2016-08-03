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
//  \brief     Implementation of widget to specify all times
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

// Qt includes
#include <QAction>
#include <QLabel>
#include <QIcon>
#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolBar>
#include <QGroupBox>
#include <QPushButton>
#include <QProgressDialog>

// local includes
#include "megaCalendar.h"

using namespace sauto;

CMegaCal::CMegaCal(QWidget *parent)
   :QDialog(parent),
   monthLay(0),
   m_prevYearAction(0),
   m_nextYearAction(0),
   m_yearLabel(0),
   m_OK_button(0),
   m_Cancel_button(0)
{

}

void CMegaCal::init()
{
   initGUI();
}

CMegaCal::~CMegaCal()
{

}

void CMegaCal::setData(const CALENDAR_DEF &cal)
{
   QMapIterator<int, CMonthSelector *> calit(m_allCalendars);
   while(calit.hasNext())
   {
      calit.next();
      calit.value()->reset();
      calit.value()->setActive(false);
   }

   m_allCalendars.value(0)->refresh();
   while(m_prevYearAction->isEnabled())
   {
      previousYearClicked();
   }

   m_data = cal;
   if(m_data.size() == 0)
   {
      return;
   }
   
   m_allCalendars.value(0)->setActive(false);

   CALENDAR_ITERATOR it(m_data);
   while(it.hasNext())
   {
      it.next();
      int year = it.key();
      int index = 0;
      for(index = 0; index < m_years.size(); index++)
      {
         if(m_years.at(index) == year)
         {
            break;
         }
      }

      if(!m_allCalendars.contains(index))
      {
         currIndex = m_years.size();
         nextYearClicked(); // insert a new calendar year
      }

      if(index == 0)
      {
         m_allCalendars.value(index)->refresh();
      }

      m_allCalendars.value(index)->setActive(it.value().first  );
      m_allCalendars.value(index)->setData  (it.value().second );
   }
}

void CMegaCal::initGUI()
{
   m_prevYearAction = new QAction(this);
   m_prevYearAction->setIcon(QIcon(":/images/arrow_previous.png"));
   m_prevYearAction->setEnabled(false);

   connect(m_prevYearAction, SIGNAL(triggered()), 
      this, SLOT(previousYearClicked()));

   QToolBar *prevBar = new QToolBar(this);
   prevBar->addAction(m_prevYearAction);

   m_nextYearAction = new QAction(this);

   connect(m_nextYearAction, SIGNAL(triggered()), 
      this, SLOT(nextYearClicked()));

   m_nextYearAction->setIcon(QIcon(":/images/arrow_next.png"));
   QToolBar *nextBar = new QToolBar(this);
   nextBar->addAction(m_nextYearAction);

   currIndex = 0;
   m_yearLabel = new QLabel(this);

   QHBoxLayout *actionLay = new QHBoxLayout;
   actionLay->addWidget(prevBar);
   actionLay->addWidget(m_yearLabel);
   actionLay->addWidget(nextBar);

   QGroupBox *actionBox = new QGroupBox(this);
   actionBox->setLayout(actionLay);
   actionBox->setTitle(tr("Year"));

   monthLay = new QVBoxLayout;
   nextYearClicked();
   m_allCalendars.value(0)->setVisible(true);

   m_OK_button = new QPushButton(this);
   m_OK_button->setText(tr("OK"));

   connect(m_OK_button, SIGNAL(clicked()), 
      this, SLOT(OKClicked()));

   m_Cancel_button = new QPushButton(this);
   m_Cancel_button ->setText(tr("Cancel"));

   connect(m_Cancel_button, SIGNAL(clicked()), 
      this, SLOT(cancelClicked()));

   QHBoxLayout *buttonLay = new QHBoxLayout;
   buttonLay->addWidget(m_OK_button);
   buttonLay->addWidget(m_Cancel_button);
   QGroupBox *buttonBox = new QGroupBox(this);
   buttonBox->setLayout(buttonLay);
   buttonBox->setTitle(tr("Control"));

   QHBoxLayout *bottomLay = new QHBoxLayout;
   bottomLay->addWidget(buttonBox);
   bottomLay->addWidget(actionBox);
   bottomLay->addStretch();

   QVBoxLayout *totLay = new QVBoxLayout;
   totLay->addLayout(monthLay);
   totLay->addLayout(bottomLay);
   this->setLayout(totLay);
}

void CMegaCal::previousYearClicked()
{
   if((currIndex - 1) < 0)
   {
      return;
   }

   if(m_allCalendars.contains(currIndex))
   {
      m_allCalendars.value(currIndex)->setVisible(false);
   }
   --currIndex;
   if(m_allCalendars.contains(currIndex))
   {
      m_allCalendars.value(currIndex)->setVisible(true);
   }

   updateYearActions();
}

void CMegaCal::addCalendar()
{
   int prevYear = 0;
   if(m_years.size() > 0)
   {
      prevYear = m_years.at(m_years.size() - 1);
   }
   else
   {
      prevYear = QDateTime::currentDateTime().date().year() - 1 ;
   }
   int addYear = prevYear + 1;
   m_years << addYear;
   currIndex = m_years.size() - 1;

   CMonthSelector *month = new CMonthSelector(m_years.at(currIndex), this);
   month->setActive();
   m_allCalendars.insert(currIndex, month);
   month->setVisible(false);
   monthLay->addWidget(month);

   updateYearActions();
}

void CMegaCal::nextYearClicked()
{
   bool keepIndex = false;
   if((currIndex + 1) >= m_years.size())
   {
      addCalendar();
      keepIndex = true;
   }

   QMapIterator<int, CMonthSelector *> it(m_allCalendars);
   while(it.hasNext())
   {
      it.next();
      if(it.value() != 0)
      {
         it.value()->setVisible(false);
      }
   }
   if(!keepIndex)
   {
      ++currIndex;
   }
   if(m_allCalendars.contains(currIndex))
   {
      m_allCalendars.value(currIndex)->setVisible(true);
   }

   updateYearActions();
}

void CMegaCal::updateYearActions()
{
   if(currIndex < 0 || currIndex >= m_years.size())
   {
      return;
   }
   currYear = m_years.at(currIndex);
   m_yearLabel->setText(QString("%1").arg(m_years.at(currIndex)));

   if(currIndex == 0)
   {
      m_prevYearAction->setEnabled(false);
   }
   else if(!m_prevYearAction->isEnabled())
   {
      m_prevYearAction->setEnabled(true);
   }
}

void CMegaCal::OKClicked()
{
   m_data.clear();
   QMapIterator<int, CMonthSelector *> it(m_allCalendars);
   while(it.hasNext())
   {
      it.next();
      CMonthSelector *year = it.value();
      if(0 == year || !year->getActive())
      {
         continue;
      }

      QPair<bool, MONTH_DEF> newPair;
      newPair.first = true;
      newPair.second = year->requestData();
      m_data.insert(m_years.at(it.key()), newPair);
   }

   emit contentSignal(m_data);
}

void CMegaCal::cancelClicked()
{
   emit cancelled();
   emit closeMe();
}

void CMegaCal::cancel()
{
   setData(m_data);
}

void CMegaCal::updateWidgetContent()
{
   QMapIterator<int, CMonthSelector *> it(m_allCalendars);
   while(it.hasNext())
   {
      it.next();
      CMonthSelector *selector = it.value();
      if(selector == 0) continue;
      selector->updateWidgetContent();
   }
}