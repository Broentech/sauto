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
//  \file      sautoDialog.cpp
//
//  \brief     Implementation of a dialog class for specifying schedule
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

// Qt includes
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QWidgetAction>
#include <QGroupBox>
#include <QToolBar>
#include <QLineEdit>
#include <QToolButton>
#include <QFileDialog>
#include <QLabel>
#include <QStandardPaths>
#include <QtDebug>
#include <QDir>

// solution includes
#include <sautoXml/sautoXml.h>

// local includes
#include "sautoDialog.h"

const char* const DEFAULT_CLOCK_FILENAME = "default";

using namespace sauto;

CScheduleDialog::CScheduleDialog(QWidget *parent)
   :CDialogBase(parent),
   m_clockOn(0),
   m_clockOff(0),
   m_dayOn(0),
   m_dayOff(0),
   m_weekOn(0),
   m_weekOff(0),
   m_calOn(0),
   m_calOff(0),
   m_freqEdit(0),
   m_timeIntervalWidget(0),
   m_daySelect(0),
   m_calendar(0)
{
   m_xmlpath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/xml/";
   QDir dir(m_xmlpath);
   if (!dir.exists())
   {
      dir.mkpath(m_xmlpath);
   }

   QFileInfoList flist = dir.entryInfoList();
   for (int i = 0; i < flist.size(); i++)
   {
      QFileInfo fInfo = flist.at(i);
      if (fInfo.exists() && fInfo.baseName() != "")
      {
         m_files << fInfo.baseName();
      }
   }
   m_files << "[ New ]";

   this->setObjectName("CScheduleDialog");
   this->setWindowTitle(tr("Sauto Dialog"));
   this->setWindowIcon(QIcon(":/images/clock.png"));
   initGUI();
   updateFilesBox();
   m_filesBox->setCurrentIndex(m_filesBox->findText("[ New ]"));
}

CScheduleDialog::~CScheduleDialog()
{

}

void CScheduleDialog::initGUI()
{
   QGridLayout *clockLay = createTimeSettings();
   QVBoxLayout *baseLay = initBaseWidgets();
   if(0 != m_nameLine)
   {
      m_nameLine->setPlaceholderText(DEFAULT_CLOCK_FILENAME);
   }
   QGridLayout *totLay = new QGridLayout;
   totLay->addLayout(clockLay, 0, 0);
   totLay->addLayout(baseLay, 1, 0);
   this->setLayout(totLay);
}

bool CScheduleDialog::loadXml(const QString &filename)
{
   resetGUI();
   QString usepath = m_xmlpath + filename;
   QFileInfo fInfo(usepath);
   if (usepath == "" || !fInfo.exists())
   {
      qDebug() << QString("File not found : %1").arg(usepath);
      return false;
   }

   SautoXml xml;
   SautoModel default_Frequency; 
   INTERVAL_LIST default_TimeIntervals;
   WEEK_DEF default_Week;
   CALENDAR_DEF default_Calendar;
   if(!xml.readClockFile(usepath,
      default_Frequency,
      default_TimeIntervals,
      default_Week,
      default_Calendar))

   {
      qCritical() << QString("Failed at loading file '%1'").arg(usepath);
      return false;
   }
   QString err;
   if (!verifyScheduleSettings(
         default_Frequency,
         default_TimeIntervals,
         default_Week,
         default_Calendar,
         err)
      )
   {
      qCritical() << err;
      return false;
   }

   m_default_Frequency = default_Frequency;
   m_default_TimeIntervals = default_TimeIntervals;
   m_default_Week = default_Week;
   m_default_Calendar = default_Calendar;

   bool asap;
   bool allDay;
   bool everyDay;
   bool everyMonth;
   xml.getCheckers(asap, allDay, everyDay, everyMonth);
   m_clockOff->setChecked(asap);
   m_clockOn->setChecked(!asap);
   m_dayOff->setChecked(allDay);
   m_dayOn->setChecked(!allDay);
   m_weekOff->setChecked(everyDay);
   m_weekOn->setChecked(!everyDay);
   m_calOff->setChecked(everyMonth);
   m_calOn->setChecked(!everyMonth);

   const QString fInfoBase = fInfo.baseName();
   if(!m_files.contains(fInfoBase))
   {
      m_files << fInfoBase;
   }
   m_lastSavedXML = usepath;
   m_freqEdit->setData(m_default_Frequency);
   m_timeIntervalWidget->setData(m_default_TimeIntervals);
   m_daySelect->setData(m_default_Week);
   m_calendar->setData(m_default_Calendar);
   qDebug() << QString("Loaded file '%1'").arg(usepath);

   updateNameLine();

   if(m_default_Frequency.getStartTimeMSec() < 0)
   {
      setIsOnlySingleInterval(true);
   }

   return true;
}

void CScheduleDialog::resetGUI()
{
   SautoModel empty_Frequency; 
   INTERVAL_LIST empty_TimeIntervals;
   WEEK_DEF empty_Week;
   CALENDAR_DEF empty_Calendar;
   m_freqEdit->setData(empty_Frequency);
   m_timeIntervalWidget->setData(empty_TimeIntervals);
   m_daySelect->setData(empty_Week);
   m_calendar->setData(empty_Calendar);
   m_clockOff->setChecked(true);
   m_clockOn->setChecked(false);
   m_dayOff->setChecked(true);
   m_dayOn->setChecked(false);
   m_weekOff->setChecked(true);
   m_weekOn->setChecked(false);
   m_calOff->setChecked(true);
   m_calOn->setChecked(false);
}

void CScheduleDialog::fileSelectedFromBox(const QString &baseName)
{
   if (baseName == "[ New ]")
   {
      return resetGUI();
   }
   const QString fullPath = QString("%1%2.xml").arg(m_xmlpath).arg(baseName);
   const QFileInfo fInfo(fullPath);
   if(!fInfo.exists())
   {
      qCritical() << QString("Invalid file '%1'").arg(fullPath);
      return;
   }

   if (!loadXml(QString("%1.xml").arg(baseName)))
   {
      qCritical() << QString("Failed at loading file '%1'").arg(fullPath);
   }
}

void CScheduleDialog::acceptClicked()
{
   if(m_clockOff->isChecked())
   {
      m_default_Frequency.reset();
      m_freqEdit->setData(m_default_Frequency);
   }
   if(m_dayOff->isChecked())
   {
      m_default_TimeIntervals.clear();
      m_timeIntervalWidget->setData(m_default_TimeIntervals);
   }
   if(m_weekOff->isChecked())
   {
      m_default_Week.clear();
      m_daySelect->setData(m_default_Week);
   }
   if(m_calOff->isChecked())
   {
      m_default_Calendar.clear();
      m_calendar->setData(m_default_Calendar);
   }

   QString path;
   if(saveToXmlFile(path))
   {
      m_lastSavedXML = path;
   }

   this->accept();
}

bool CScheduleDialog::saveToXmlFile(QString &path)
{
   QString fileName = m_nameLine->text();
   if (fileName == "")
   {
      fileName = m_nameLine->placeholderText();
   }

   path = m_xmlpath + fileName + ".xml";
   bool asapCheck = m_clockOff->isChecked();
   bool alldayCheck = m_dayOff->isChecked();
   bool dailyCheck = m_weekOff->isChecked();
   bool everyMonth = m_calOff->isChecked();

   SautoXml xml;

   xml.setCheckers(asapCheck,
      alldayCheck,
      dailyCheck,
      everyMonth);

   if (!xml.writeClockFile(
         path,
         m_default_Frequency,
         m_default_TimeIntervals,
         m_default_Week,
         m_default_Calendar)
         )
   {
      qCritical() << QString("Failed to save settings to %1").arg(path);
      return false;
   }

   if(!m_files.contains(path))
   {
      m_files << path;
   }

   qDebug() << QString("Settings save to %1").arg(path);
   emit saved();
   return true;
}

QGridLayout* CScheduleDialog::createTimeSettings()
{
   m_freqEdit = new CWaveletWidget(this, true);

   connect(m_freqEdit, SIGNAL(dataSignal(const SautoModel &)),
      this, SLOT(receivedFreq(const SautoModel &)));

   connect(m_freqEdit, SIGNAL(isSingleInterval(bool)),
      this, SLOT(setIsOnlySingleInterval(bool)));

   m_timeIntervalWidget = new CTimeIntervalWidget(this);

   connect(m_timeIntervalWidget, SIGNAL(timeIntervals(INTERVAL_LIST)),
      this, SLOT(receiveTimeIntervals(INTERVAL_LIST)));

   m_daySelect = new CDaySelect(this);

   connect(m_daySelect, SIGNAL(weekDescription(WEEK_DEF)),
      this, SLOT(receiveWeek(WEEK_DEF)));

   m_calendar = new CMegaCal(this);

   connect(m_calendar, SIGNAL(contentSignal(CALENDAR_DEF)),
      this, SLOT(receiveCalendars(CALENDAR_DEF)));

   connect(m_freqEdit, SIGNAL(cancelled()),
      this, SLOT(clockCancelled()));

   connect(m_timeIntervalWidget, SIGNAL(cancelled()),
      this, SLOT(dayCancelled()));

   connect(m_daySelect, SIGNAL(cancelled()),
      this, SLOT(weekCancelled()));

   connect(m_calendar, SIGNAL(cancelled()),
      this, SLOT(calCancelled()));

   m_timeIntervalWidget->init();
   m_daySelect->init();
   m_calendar->init();

   m_freqEdit->hide();
   m_timeIntervalWidget->hide();
   m_daySelect->hide();
   m_calendar->hide();

   m_clockOn = new QToolButton(this);
   m_clockOff = new QToolButton(this);
   m_dayOn = new QToolButton(this);
   m_dayOff = new QToolButton(this);
   m_weekOn = new QToolButton(this);
   m_weekOff = new QToolButton(this);
   m_calOn = new QToolButton(this);
   m_calOff = new QToolButton(this);

   QSize buttonSize(75, 75);
   m_clockOn->setMinimumSize(buttonSize);
   m_dayOn->setMinimumSize(buttonSize);
   m_weekOn->setMinimumSize(buttonSize);
   m_calOn->setMinimumSize(buttonSize);
   m_clockOff->setMinimumSize(buttonSize);
   m_dayOff->setMinimumSize(buttonSize);
   m_weekOff->setMinimumSize(buttonSize);
   m_calOff->setMinimumSize(buttonSize);

   m_clockOn->setMaximumSize(buttonSize);
   m_dayOn->setMaximumSize(buttonSize);
   m_weekOn->setMaximumSize(buttonSize);
   m_calOn->setMaximumSize(buttonSize);
   m_clockOff->setMaximumSize(buttonSize);
   m_dayOff->setMaximumSize(buttonSize);
   m_weekOff->setMaximumSize(buttonSize);
   m_calOff->setMaximumSize(buttonSize);

   m_clockOn->setCheckable(true);
   m_dayOn->setCheckable(true);
   m_weekOn->setCheckable(true);
   m_calOn->setCheckable(true);
   m_clockOff->setCheckable(true);
   m_dayOff->setCheckable(true);
   m_weekOff->setCheckable(true);
   m_calOff->setCheckable(true);

   m_clockOn->setChecked(false);
   m_dayOn->setChecked(false);
   m_weekOn->setChecked(false);
   m_calOn->setChecked(false);
   m_clockOff->setChecked(true);
   m_dayOff->setChecked(true);
   m_weekOff->setChecked(true);
   m_calOff->setChecked(true);

   QButtonGroup *m_clockGroup = new QButtonGroup(this);
   QButtonGroup *m_dayGroup = new QButtonGroup(this);
   QButtonGroup *m_weekGroup = new QButtonGroup(this);
   QButtonGroup *m_calGroup = new QButtonGroup(this);

   connect(m_clockGroup, SIGNAL(buttonClicked(int)),
      this, SLOT(clockButtonClicked(int)));

   connect(m_dayGroup, SIGNAL(buttonClicked(int)),
      this, SLOT(dayButtonClicked(int)));

   connect(m_weekGroup, SIGNAL(buttonClicked(int)),
      this, SLOT(weekButtonClicked(int)));

   connect(m_calGroup, SIGNAL(buttonClicked(int)),
      this, SLOT(calButtonClicked(int)));

   m_clockGroup->addButton(m_clockOn, 1);
   m_clockGroup->addButton(m_clockOff, 0);
   m_dayGroup->addButton(m_dayOn, 1);
   m_dayGroup->addButton(m_dayOff, 0);
   m_weekGroup->addButton(m_weekOn, 1);
   m_weekGroup->addButton(m_weekOff, 0);
   m_calGroup->addButton(m_calOn, 1);
   m_calGroup->addButton(m_calOff, 0);

   m_clockOn->setText("ON");
   m_dayOn->setText("ON");
   m_weekOn->setText("ON");
   m_calOn->setText("ON");
   m_clockOff->setText("OFF");
   m_dayOff->setText("OFF");
   m_weekOff->setText("OFF");
   m_calOff->setText("OFF");

   QLabel *clockLabel = new QLabel(tr("Timer"), this);
   QLabel *dayLabel = new QLabel(tr("Day"), this);
   QLabel *weekLabel = new QLabel(tr("Week"), this);
   QLabel *calLabel = new QLabel(tr("Calendar"), this);

   QHBoxLayout *lay1 = new QHBoxLayout;
   lay1->addStretch();
   lay1->addWidget(clockLabel);
   lay1->addStretch();

   QHBoxLayout *lay2 = new QHBoxLayout;
   lay2->addStretch();
   lay2->addWidget(dayLabel);
   lay2->addStretch();

   QHBoxLayout *lay3 = new QHBoxLayout;
   lay3->addStretch();
   lay3->addWidget(weekLabel);
   lay3->addStretch();

   QHBoxLayout *lay4 = new QHBoxLayout;
   lay4->addStretch();
   lay4->addWidget(calLabel);
   lay4->addStretch();

   QGridLayout *buttonLay = new QGridLayout;
   buttonLay->addLayout(lay1, 0, 0);
   buttonLay->addLayout(lay2, 0, 1);
   buttonLay->addLayout(lay3, 0, 2);
   buttonLay->addLayout(lay4, 0, 3);
   buttonLay->addWidget(m_clockOn, 1, 0);
   buttonLay->addWidget(m_dayOn, 1, 1);
   buttonLay->addWidget(m_weekOn, 1, 2);
   buttonLay->addWidget(m_calOn, 1, 3);
   buttonLay->addWidget(m_clockOff, 2, 0);
   buttonLay->addWidget(m_dayOff, 2, 1);
   buttonLay->addWidget(m_weekOff, 2, 2);
   buttonLay->addWidget(m_calOff, 2, 3);
   return buttonLay;
}

void CScheduleDialog::clockButtonClicked(int id)
{
   m_daySelect->hide();
   m_calendar->hide();
   m_timeIntervalWidget->hide();

   if(id == 1)
   {
      m_freqEdit->exec();
   }
   else
   {
      m_freqEdit->hide();
   }
}

void CScheduleDialog::dayButtonClicked(int id)
{
   m_daySelect->hide();
   m_calendar->hide();
   m_freqEdit->hide();

   if(id == 1)
   {
      m_timeIntervalWidget->exec();
   }
   else
   {
      m_timeIntervalWidget->hide();
   }
}

void CScheduleDialog::weekButtonClicked(int id)
{
   m_calendar->hide();
   m_freqEdit->hide();
   m_timeIntervalWidget->hide();

   if(id == 1)
   {
      m_daySelect->show();
   }
   else
   {
      m_daySelect->hide();
   }
}

void CScheduleDialog::calButtonClicked(int id)
{
   m_daySelect->hide();
   m_freqEdit->hide();
   m_timeIntervalWidget->hide();

   if(id == 1)
   {
      m_calendar->exec();
   }
   else
   {
      m_calendar->hide();
   }
}

void CScheduleDialog::receivedFreq(const SautoModel &data)
{
   m_freqEdit->hide();
   if(!data.isValid() && !data.getHasCustomInterval())
   {
      m_clockOff->setChecked(true);
      return; 
   }
   m_default_Frequency = data;
}

void CScheduleDialog::receiveTimeIntervals(INTERVAL_LIST intervals)
{
   m_timeIntervalWidget->hide();
   if(intervals.size() <= 0)
   {
      m_dayOff->setChecked(true);
      return;
   }
   m_default_TimeIntervals = intervals;
}

void CScheduleDialog::receiveWeek(WEEK_DEF week)
{
   m_daySelect->hide();
   if(week.size() <= 0)
   {
      m_weekOff->setChecked(true);
      return;
   }
   m_default_Week = week;
}

void CScheduleDialog::receiveCalendars(CALENDAR_DEF calendars)
{
   m_calendar->hide();
   if(calendars.size() <= 0)
   {
      m_calOff->setChecked(true);
      return;
   }
   m_default_Calendar = calendars;
}

void CScheduleDialog::setIsOnlySingleInterval(bool val)
{
   m_dayOn->setEnabled(!val);
   m_dayOff->setEnabled(!val);
   m_weekOn->setEnabled(!val);
   m_weekOff->setEnabled(!val);
   m_calOn->setEnabled(!val);
   m_calOff->setEnabled(!val);
   m_daySelect->hide();
   m_calendar->hide();
   m_timeIntervalWidget->hide();
}

void CScheduleDialog::clockCancelled()
{
   m_freqEdit->hide();
   m_timeIntervalWidget->hide();
   m_daySelect->hide();
   m_calendar->hide();
   m_clockOff->setChecked(true);
}

void CScheduleDialog::dayCancelled()
{
   m_freqEdit->hide();
   m_timeIntervalWidget->hide();
   m_daySelect->hide();
   m_calendar->hide();
   m_dayOff->setChecked(true);
}

void CScheduleDialog::weekCancelled()
{
   m_freqEdit->hide();
   m_timeIntervalWidget->hide();
   m_daySelect->hide();
   m_calendar->hide();
   m_weekOff->setChecked(true);
}

void CScheduleDialog::calCancelled()
{
   m_freqEdit->hide();
   m_timeIntervalWidget->hide();
   m_daySelect->hide();
   m_calendar->hide();
   m_calOff->setChecked(true);
}

void CScheduleDialog::updateWidgets()
{
   m_freqEdit->updateWidgetContent();
   m_timeIntervalWidget->updateWidgetContent();
   m_daySelect->updateWidgetContent();
   m_calendar->updateWidgetContent();
}