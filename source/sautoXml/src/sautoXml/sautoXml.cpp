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
//  \brief     Implementation of class to handle clock definition XML files.
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

// Qt includes
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QtDebug>

// local includes
#include "sautoXml.h"

using namespace sauto;

SautoXml::SautoXml(QObject *parent)
   :CXML_base(parent),
   m_ASAPChecked(true),
   m_allDayChecked(true),
   m_everyDayChecked(true),
   m_everyMonthChecked(true),
   m_defClockSet(false),
   m_defWeekSet(false),
   m_defCalendarSet(false),
   m_daysHasCustomInterval(false)
{
   this->setObjectName("SautoXml");
}

SautoXml::~SautoXml()
{

}

bool SautoXml::writeClockFile(
   const QString &fileName,
   const SautoModel  &frequency,
   const INTERVAL_LIST &interval,
   const WEEK_DEF &week,
   const CALENDAR_DEF  &calender
   )
{
   CTreeBranch *tree = createClockTree(frequency, interval, week, calender);
   if (0 == tree)
   {
      qCritical() << QString("%1 %2")
         .arg(tr("Failed to create data for file"))
         .arg(fileName);
      return false;
   }
   bool ret = writeToFile(XML_SAUTO, fileName, tree);
   delete tree;
   return ret;
}

bool SautoXml::readClockFile(
   const QString &fileName,
   SautoModel  &frequency,
   INTERVAL_LIST &interval,
   WEEK_DEF &week,
   CALENDAR_DEF  &calender
   )
{
   CTreeBranch tree;
   if (!readXml(fileName, &tree))
   {
      qCritical() << QString("%1 '%2'")
         .arg(tr("Failed at reading file"))
         .arg(fileName);
      return false;
   }

   return parseProcessTree(&tree, frequency, interval, week, calender);
}

void SautoXml::readEntryElement(QXmlStreamReader *xmlReader, CTreeBranch *parent)
{
   if (0 == xmlReader || 0 == parent)
   {
      return;
   }

   CTreeBranch *item = new CTreeBranch(parent);
   const QString termVal = xmlReader->attributes().value("term").toString();
   item->setText(0, termVal);
   xmlReader->readNext();
   while (!xmlReader->atEnd())
   {
      if (xmlReader->isEndElement())
      {
         xmlReader->readNext();
         break;
      }

      if (xmlReader->isStartElement())
      {
         const QString readerName = xmlReader->name().toString();
         if (readerName == "entry")
         {
            m_nameBuffer.clear();
            readEntryElement(xmlReader, item);
         }
         else if (
            readerName == "Always" ||
            readerName == "Custom" ||
            readerName == "Clock_Mode" ||
            readerName == "Week_Mode" ||
            readerName == "Calendar_Mode" ||
            readerName == "Enabled"
            )
         {
            m_nameBuffer.clear();
            readModeElement(xmlReader, item, termVal);
         }
         else if (readerName == "Name" ||
            readerName == "Inherited")
         {
            readNameElement(xmlReader, item, termVal);
         }
         else if (
            readerName == "Frequency_Mode" ||
            readerName == "Period_sec" ||
            readerName == "Phase_sec" ||
            readerName == "Start_time" ||
            readerName == "Duration" ||
            readerName == "HasCustomInterval" ||
            readerName == "Peak_task" ||
            readerName == "Valley_task" ||
            readerName == "Rising_task" ||
            readerName == "Sinking_task"
            )
         {
            readFreqElement(xmlReader, item, termVal);
         }
         else
         {
            readSkipUnknown(xmlReader);
         }
      }
      else
      {
         xmlReader->readNext();
      }
   }
}

void SautoXml::readModeElement(QXmlStreamReader *xmlReader, CTreeBranch *parent, const QString &)
{
   if (0 == xmlReader || 0 == parent)
   {
      return;
   }

   const QString name = xmlReader->name().toString();
   const QString val = xmlReader->readElementText();
   if (xmlReader->isEndElement())
   {
      xmlReader->readNext();
   }

   const QString addTxt = QString("%1=%2").arg(name).arg(val);
   if (name == "Always")
   {
      m_buffer = addTxt;
      return;
   }

   QString parTxt;
   if (!m_buffer.isEmpty())
   {
      parTxt = QString("%1,%2").arg(m_buffer).arg(addTxt);
      m_buffer.clear();
   }
   else
   {
      parTxt = addTxt;
   }
   parent->setText(1, parTxt);
}

void SautoXml::readNameElement(QXmlStreamReader *xmlReader, CTreeBranch *parent, const QString &)
{
   if (0 == xmlReader || 0 == parent)
   {
      return;
   }

   const QString name = xmlReader->name().toString();
   const QString val = xmlReader->readElementText();
   if (xmlReader->isEndElement())
   {
      xmlReader->readNext();
   }

   const QString parTxt = QString("%1=%2").arg(name).arg(val);
   if (!m_nameBuffer.isEmpty())
   {
      m_nameBuffer.append(QString(",%1").arg(parTxt));
   }
   else
   {
      m_nameBuffer = parTxt;
   }
   parent->setText(1, m_nameBuffer);
}

void SautoXml::readFreqElement(QXmlStreamReader *xmlReader, CTreeBranch *parent, const QString &)
{
   if (0 == xmlReader || 0 == parent)
   {
      return;
   }

   const QString name = xmlReader->name().toString();
   const QString val = xmlReader->readElementText();
   if (xmlReader->isEndElement())
   {
      xmlReader->readNext();
   }
   if (name == "Frequency_Mode" && val == "ASAP")
   {
      return;
   }

   m_freqBuffer << QString("%1=%2").arg(name).arg(val);
   QString freqMode = m_freqBuffer.at(0).split("=").at(1);
   if (freqMode == "WAVELET" && m_freqBuffer.size() == 10)
   {
      const QString parTxt = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10")
         .arg(m_freqBuffer.at(0))
         .arg(m_freqBuffer.at(1))
         .arg(m_freqBuffer.at(2))
         .arg(m_freqBuffer.at(3))
         .arg(m_freqBuffer.at(4))
         .arg(m_freqBuffer.at(5))
         .arg(m_freqBuffer.at(6))
         .arg(m_freqBuffer.at(7))
         .arg(m_freqBuffer.at(8))
         .arg(m_freqBuffer.at(9));
      m_freqBuffer.clear();
      parent->setText(1, parTxt);
   }
   else if (freqMode == "STATIC" && m_freqBuffer.size() == 6)
   {
      const QString parTxt = QString("%1,%2,%3,%4,%5,%6")
         .arg(m_freqBuffer.at(0))
         .arg(m_freqBuffer.at(1))
         .arg(m_freqBuffer.at(2))
         .arg(m_freqBuffer.at(3))
         .arg(m_freqBuffer.at(4))
         .arg(m_freqBuffer.at(5));
      m_freqBuffer.clear();
      parent->setText(1, parTxt);
   }
   else if (freqMode == "SINGLE" && m_freqBuffer.size() == 3)
   {
      const QString parTxt = QString("%1,%2,%3")
         .arg(m_freqBuffer.at(0))
         .arg(m_freqBuffer.at(1))
         .arg(m_freqBuffer.at(2));
      m_freqBuffer.clear();
      parent->setText(1, parTxt);
   }
}

CTreeBranch* SautoXml::createClockTree(
   const SautoModel  &frequency,
   const INTERVAL_LIST &interval,
   const WEEK_DEF      &week,
   const CALENDAR_DEF  &calender
   )
{
   CTreeBranch *root = new CTreeBranch();

   // create time entries
   CTreeBranch *timeBranch = new CTreeBranch();
   timeBranch->setText(0, "Time");

   CTreeBranch *timeBranch_child1 = new CTreeBranch();
   timeBranch_child1->setText(0, "Mode");

   if ((true == m_ASAPChecked) &&
      (true == m_allDayChecked) &&
      (true == m_everyDayChecked) &&
      (true == m_everyMonthChecked))
   {
      timeBranch_child1->setText(1, "Always=true,Custom=false");
   }
   else
   {
      timeBranch_child1->setText(1, "Always=false,Custom=true");

      CTreeBranch *timeBranch_freqDefs = new CTreeBranch();
      timeBranch_freqDefs->setText(0, "Frequency_Definitions");
      if (m_ASAPChecked)
      {
         timeBranch_freqDefs->setText(1, "Frequency_Mode=ASAP");
      }
      else if (frequency.isValid())
      {
         populateFreqItem(timeBranch_freqDefs, frequency);
      }
      timeBranch_child1->addChild(timeBranch_freqDefs);

      CTreeBranch *timeBranch_clockDefs = new CTreeBranch();
      timeBranch_clockDefs->setText(0, "Clock_Definitions");
      if (m_allDayChecked)
      {
         timeBranch_clockDefs->setText(1, "Clock_Mode=All_Day");
      }
      else
      {
         timeBranch_clockDefs->setText(1, "Clock_Mode=Custom");
         populateClockItem(timeBranch_clockDefs, interval);
      }
      timeBranch_child1->addChild(timeBranch_clockDefs);

      CTreeBranch *timeBranch_weekDefs = new CTreeBranch();
      timeBranch_weekDefs->setText(0, "Week_Definitions");
      if (m_everyDayChecked)
      {
         timeBranch_weekDefs->setText(1, "Week_Mode=Every_Day");
      }
      else
      {
         timeBranch_weekDefs->setText(1, "Week_Mode=Custom");
         populateWeekItem(timeBranch_weekDefs, week);
      }
      timeBranch_child1->addChild(timeBranch_weekDefs);

      CTreeBranch *timeBranch_calendarDefs = new CTreeBranch();
      timeBranch_calendarDefs->setText(0, "Calendar_Definitions");
      if (m_everyMonthChecked)
      {
         timeBranch_calendarDefs->setText(1, "Calendar_Mode=Every_Month");
      }
      else
      {
         timeBranch_calendarDefs->setText(1, "Calendar_Mode=Custom");
         populateCalendarItem(timeBranch_calendarDefs, calender);
      }
      timeBranch_child1->addChild(timeBranch_calendarDefs);
   }

   timeBranch->addChild(timeBranch_child1);
   root->addChild(timeBranch);
   return root;
}

void SautoXml::populateFreqItem(CTreeBranch *branch, const SautoModel &freq)
{
   if (freq.isValid() || (!freq.isValid() && freq.getHasCustomInterval()))
   {
      QString hasCustom = "false";
      if (freq.getHasCustomInterval())
      {
         hasCustom = "true";
      }
      if (freq.getType() == WAVELET)
      {
         qint64 localStartTime_ms = freq.getStartTimeMSec();
         qreal toWrite = 0;
         if (localStartTime_ms < 0)
         {
            toWrite = -1;
         }
         else
         {
            toWrite = qRound(static_cast<qreal>(localStartTime_ms) / 1000.0);
         }
         QString itemVal = QString("Frequency_Mode=%1,Period_sec=%2,Phase_sec=%3,Start_time=%4,Duration=%5,Peak_task=%6,Valley_task=%7,Rising_task=%8,Sinking_task=%9,HasCustomInterval=%10")
            .arg(freq.getTypeString())
            .arg(qRound(static_cast<qreal>(freq.getPeriodTotMSec()) / 1000.0)) //< calc from msec to sec
            .arg(freq.getPhase())
            .arg(toWrite) //< calc from msec to sec
            .arg(qRound(static_cast<qreal>(freq.getDuration()) / 1000.0)) //< calc from msec to sec
            .arg(freq.getOnPeak())
            .arg(freq.getOnValley())
            .arg(freq.getOnRising())
            .arg(freq.getOnSinking())
            .arg(hasCustom);
         branch->setText(1, itemVal);
      }
      else if (freq.getType() == STATIC)
      {
         QString itemVal = QString("Frequency_Mode=%1,Period_sec=%2,Start_time=%3,Duration=%4,Peak_task=%5,HasCustomInterval=%6")
            .arg(freq.getTypeString())
            .arg(qRound(static_cast<qreal>(freq.getPeriodTotMSec()) / 1000.0)) //< calc from msec to sec
            .arg(qRound(static_cast<qreal>(freq.getStartTimeMSec()) / 1000.0)) //< calc from msec to sec
            .arg(qRound(static_cast<qreal>(freq.getDuration()) / 1000.0)) //< calc from msec to sec
            .arg(freq.getOnPeak())
            .arg(hasCustom);
         branch->setText(1, itemVal);
      }
      else if (freq.getType() == SINGLE)
      {
         QString itemVal = QString("Frequency_Mode=%1,Start_time=%2,Peak_task=%3")
            .arg(freq.getTypeString())
            .arg(qRound(static_cast<qreal>(freq.getStartTimeMSec()) / 1000.0)) //< calc from msec to sec
            .arg(freq.getOnPeak());
         branch->setText(1, itemVal);
      }
      else
      {
         branch->setText(1, "INVALID");
      }
   }
   else
   {
      branch->setText(1, QString("INVALID"));
   }
}

void SautoXml::populateClockItem(CTreeBranch *branch, INTERVAL_LIST intervals)
{
   if(0 == branch || intervals.size() == 0)
   {
      return;
   }

   for(int i=0; i<intervals.count(); i++)
   {
      SautoModel freq = intervals.at(i);
      CTreeBranch *timeItem_clockInterval = new CTreeBranch();
      timeItem_clockInterval->setText(0, "INTERVAL");
      populateFreqItem(timeItem_clockInterval, freq);
      branch->addChild(timeItem_clockInterval);
   }
}

void SautoXml::populateWeekItem(CTreeBranch *branch, WEEK_DEF week)
{
   if(0 == branch || week.size() == 0)
   {
      return;
   }

   WEEK_ITERATOR it(week);
   while(it.hasNext())
   {
      it.next();
      QString dayName = QString("%1").arg(it.key());
      DAY_OF_WEEK_DEF day = it.value();

      CTreeBranch *timeItem_dayDef = new CTreeBranch();
      timeItem_dayDef->setText(0, dayName);
      bool dayEnabled = dayIsToggled(day);
      if(!dayEnabled)
      {
         timeItem_dayDef->setText(1, "Enabled=false");
         continue;
      }
      timeItem_dayDef->setText(1, "Enabled=true");
      CTreeBranch *timeItem_clock = new CTreeBranch();
      timeItem_clock->setText(0, "Clock_Definitions");

      bool dayInheritInterval = dayInheritsTime(day);
      if(dayInheritInterval)
      {
         timeItem_clock->setText(1, "Clock_Mode=Inherited");
         timeItem_dayDef->addChild(timeItem_clock);
         branch->addChild(timeItem_dayDef);
         continue;
      }

      timeItem_clock->setText(1, "Clock_Mode=Custom");
      INTERVAL_LIST dayIntervals = day.second;
      populateClockItem(timeItem_clock, dayIntervals);
      timeItem_dayDef->addChild(timeItem_clock);
      branch->addChild(timeItem_dayDef);
   }
}

void SautoXml::populateCalendarItem(CTreeBranch *branch, CALENDAR_DEF calendars)
{
   if (0 == branch || calendars.size() == 0)
   {
      return;
   }

   CALENDAR_ITERATOR year_it(calendars);
   while (year_it.hasNext())
   {
      year_it.next();
      int year = year_it.key();
      CTreeBranch *timeItem_yearDef = new CTreeBranch();
      timeItem_yearDef->setText(0, "Year_Definition");
      timeItem_yearDef->setText(1, QString("Name=%1").arg(year));
      MONTH_DEF months = year_it.value().second;
      MONTH_ITERATOR month_it(months);

      while (month_it.hasNext())
      {
         month_it.next();
         QString monthName = month_it.key();
         CTreeBranch *timeItem_monthDef = new CTreeBranch();
         timeItem_monthDef->setText(0, "Month_Definition");

         DAY_OF_MONTH_DEF days = month_it.value();
         bool monthIsInherited = days.first;
         QString tempStr;
         monthIsInherited ? tempStr = QString("true") : tempStr = QString("false");
         timeItem_monthDef->setText(1, QString("Name=%1,Inherited=%2").arg(monthName).arg(tempStr));

         if (monthIsInherited)
         {
            timeItem_yearDef->addChild(timeItem_monthDef);
            continue;
         }

         QMapIterator<int, CALENDAR_DATE> days_it(days.second);
         while (days_it.hasNext())
         {
            days_it.next();
            int dayNum = days_it.key();
            CALENDAR_DATE calDate = days_it.value();
            QDate date = calendarDate_QDate(calDate);
            bool dateInherit = calendarDateInheritsTime(calDate);
            INTERVAL_LIST interval = calDate.second;

            CTreeBranch *timeItem_dayDef = new CTreeBranch();
            timeItem_dayDef->setText(0, "Day_Definition");
            if (interval.size() == 0 || dateInherit) // XXX TODO stianb 09.01.2013 : supposed to be false for inherit ??? stianb 08.02.2013 : NO!
            {
               timeItem_dayDef->setText(1, QString("Name=%1,Inherited=%2").arg(dayNum).arg("true"));
            }
            else
            {
               timeItem_dayDef->setText(1, QString("Name=%1,Inherited=%2").arg(dayNum).arg("false"));
               populateClockItem(timeItem_dayDef, interval);
            }
            timeItem_monthDef->addChild(timeItem_dayDef);
         }
         timeItem_yearDef->addChild(timeItem_monthDef);
      }
      branch->addChild(timeItem_yearDef);
   }
}

bool SautoXml::parseProcessTree(
   CTreeBranch *root,
   SautoModel &frequency,
   INTERVAL_LIST &interval,
   WEEK_DEF &week,
   CALENDAR_DEF &calender
   )
{
   if (0 == root)
   {
      return false;
   }

   for (int i = 0; i < root->invisibleRootItem()->childCount(); i++)
   {
      CTreeBranch *item = root->invisibleRootItem()->child(i);
      if (0 == item)
      {
         continue;
      }
      if (!parseXmlItem(item, frequency, interval, week, calender))
      {
         return false;
      }
   }

   return true;
}

bool SautoXml::parseXmlItem(
   CTreeBranch *branch,
   SautoModel &frequency,
   INTERVAL_LIST &interval,
   WEEK_DEF &week,
   CALENDAR_DEF &calender
   )
{
   QString itemName = branch->text(0);
   QString itemVal = branch->text(1);
   if (itemName == "Time")
   {
      if (!parseXmlTimeSettingsItem(branch,
         frequency,
         interval,
         week,
         calender))
      {
         return false;
      }
   }

   for (int i = 0; i < branch->childCount(); i++)
   {
      if (!parseXmlItem(branch->child(i),
         frequency,
         interval,
         week,
         calender))
      {
         return false;
      }
   }
   return true;
}

bool SautoXml::parseXmlTimeSettingsItem(
   CTreeBranch *branch,
   SautoModel &frequency,
   INTERVAL_LIST &interval,
   WEEK_DEF &week,
   CALENDAR_DEF &calender
   )
{
   QString itemName = branch->text(0);
   QString itemVal = branch->text(1);
   if (itemName == "Frequency_Definitions")
   {
      if (!extractFrequencyDefs(branch, frequency, true))
      {
         return false;
      }
   }
   else if (itemName == "Clock_Definitions" && !m_defClockSet)
   {
      INTERVAL_LIST tmpInterv;
      if (!extractClockDefs(branch, tmpInterv))
      {
         return false;
      }
      m_defClockSet = true;
      interval = tmpInterv;
   }
   else if (itemName == "Week_Definitions" && !m_defWeekSet)
   {
      WEEK_DEF tmpWeek;
      if (!extractWeekDefs(branch, tmpWeek))
      {
         return false;
      }
      m_defWeekSet = true;
      week = tmpWeek;
   }
   else if (itemName == "Calendar_Definitions" && !m_defCalendarSet)
   {
      CALENDAR_DEF tmpCal;
      if (!extractCalendarDefs(branch, tmpCal))
      {
         return false;
      }
      m_defCalendarSet = true;
      calender = tmpCal;
   }

   for (int i = 0; i < branch->childCount(); i++)
   {
      if (!parseXmlTimeSettingsItem(branch->child(i), frequency, interval, week, calender))
      {
         return false;
      }
   }
   return true;
}

bool SautoXml::extractFrequencyDefs(CTreeBranch *branch, SautoModel &freq, bool isDefaultFreq)
{
   QString itemName = branch->text(0);
   QString itemVal  = branch->text(1);
   QStringList valSplit = itemVal.split(",");
   for(int i=0; i<valSplit.size(); i++)
   {
      if (!valSplit.at(i).contains("="))
      {
         continue;
      }
      QString paraName = valSplit.at(i).split("=").at(0);
      QString paraValu = valSplit.at(i).split("=").at(1);

      if(paraName == "Frequency_Mode")
      {
         if (isDefaultFreq)
         {
            m_ASAPChecked = false;
         }
         if (paraValu == "WAVELET")
         {
            freq.setType(WAVELET);
         }
         else if (paraValu == "STATIC")
         {
            freq.setType(STATIC);
         }
         else if (paraValu == "SINGLE")
         {
            freq.setType(SINGLE);
         }
         else if(paraValu == "NOT_SPECIFIED")
         {
            if (isDefaultFreq)
            {
               m_ASAPChecked = true;
            }
            freq.setType(NOT_SPECIFIED);
         }
         else
         {
            return false;
         }
      }
      else if(paraName == "Period_sec")
      {
         bool ok;
         quint64 sec = paraValu.toULongLong(&ok);
         if(!ok)
         {
            qCritical() << QString("Invalid parameter '%1'").arg(valSplit.at(i));
            return false;
         }
         freq.setPeriodTotMSecs(sec*1000);
      }
      else if(paraName == "Phase_sec")
      {
         bool ok;
         qreal phase = paraValu.toDouble(&ok);
         if(!ok)
         {
            qCritical() << QString("Invalid parameter '%1'").arg(valSplit.at(i));
            return false;
         }
         freq.setPhase(phase);
      }
      else if(paraName == "Start_time")
      {
         bool ok;
         qint64 start = paraValu.toLongLong(&ok);
         if(!ok)
         {
            qCritical() << QString("Invalid parameter '%1'").arg(valSplit.at(i));
            return false;
         }
         if (start < 0)
         {
            freq.setStartTimeMSecs(-1);
         }
         else
         {
            freq.setStartTimeMSecs(start * 1000);
         }
      }
      else if(paraName == "Duration")
      {
         bool ok;
         quint64 dur = paraValu.toULongLong(&ok);
         if(!ok)
         {
            qCritical() << QString("Invalid parameter '%1'").arg(valSplit.at(i));
            return false;
         }
         freq.setDuration(dur * 1000);
      }
      else if (paraName == "Peak_task")
      {
         freq.setOnPeak(paraValu);
      }
      else if (paraName == "Valley_task")
      {
         freq.setOnValley(paraValu);
      }
      else if (paraName == "Rising_task")
      {
         freq.setOnRising(paraValu);
      }
      else if (paraName == "Sinking_task")
      {
         freq.setOnSinking(paraValu);
      }
      else if(paraName == "HasCustomInterval")
      {
         if (paraValu.toLower() == "true")
         {
            freq.setHasCustomInterval(true);
         }
         else if (paraValu.toLower() == "false")
         {
            freq.setHasCustomInterval(false);
         }
         else
         {
            return false;
         }
      }
      else
      {
         return false;
      }
   }

   for(int i=0; i<branch->childCount(); i++)
   {
      if (!extractFrequencyDefs(branch->child(i), freq))
      {
         return false;
      }
   }
   return true;
}

bool SautoXml::extractClockDefs(CTreeBranch *branch, INTERVAL_LIST &interval)
{
   if(0 == branch)
   {
      return false;
   }

   QString itemName = branch->text(0);
   QString itemVal  = branch->text(1);
   QStringList itemSplit = itemVal.split("=");
   if(itemSplit.at(0) == "Clock_Mode")
   {
      if(itemSplit.size() >= 2 && itemSplit.at(1) == "Custom")
      {
         if (!m_defClockSet)
         {
            m_allDayChecked = false;
         }
      }
      else if (itemSplit.size() >= 2 && itemSplit.at(1) == "All_Day" ||
         itemSplit.size() >= 2 && itemSplit.at(1) == "Inherited")
      {
         if (!m_defClockSet)
         {
            m_allDayChecked = true;
         }
         return true;
      }
      else
      {
         return false;
      }
   }
   else if(itemName == "INTERVAL")
   {
      SautoModel freq;
      if (!extractFrequencyDefs(branch, freq))
      {
         return false;
      }
      if (freq.isValid() || freq.getHasCustomInterval())
      {
         interval << freq;
      }
   }
   else if (itemName != "Day_Definition")
   {
      return false;
   }
   for(int i=0; i<branch->childCount(); i++)
   {
      if (!extractClockDefs(branch->child(i), interval))
      {
         return false;
      }
   }
   return true;
}

bool SautoXml::extractWeekDefs(CTreeBranch *branch, WEEK_DEF &week)
{
   if (0 == branch)
   {
      return false;
   }

   QString itemName = branch->text(0);
   QString itemVal  = branch->text(1);
   QStringList itemSplit = itemVal.split("=");

   if(itemSplit.at(0) == "Week_Mode")
   {
      if(itemSplit.size() >= 2 && itemSplit.at(1) == "Custom")
      {
         if (!m_defWeekSet)
         {
            m_everyDayChecked = false;
         }
      }
      else if(itemSplit.size() >= 2 && itemSplit.at(1) == "Every_Day")
      {
         if (!m_defWeekSet)
         {
            m_everyDayChecked = true;
         }
      }
      else
      {
         return false;
      }
   }
   else if(itemSplit.at(0) == "Clock_Mode")
   {
      if(itemSplit.size() > 1)
      {
         if (itemSplit.at(1) == "Custom")
         {
            m_daysHasCustomInterval = true;
         }
         else
         {
            m_daysHasCustomInterval = false;
         }
      }
   }
   else if (
      itemName == "1" ||
      itemName == "2" ||
      itemName == "3" ||
      itemName == "4" ||
      itemName == "5" ||
      itemName == "6" ||
      itemName == "7"
      )
   {
      QStringList itemValSplit = itemVal.split("=");
      if (itemValSplit.size() >= 2)
      {
         // members needed to create a week entry
         int dayName = itemName.toInt();
         bool enabled = false;
         bool inherited = true;
         INTERVAL_LIST interv;
         QString boolStr = itemValSplit.at(1);
         if (boolStr.toLower() == "true")
         {
            enabled = true;
         }
         else if (boolStr.toLower() == "false")
         {
            enabled = false;
         }
         else
         {
            return false;
         }
         int childCount = branch->childCount();
         for (int i = 0; i < childCount; i++)
         {
            CTreeBranch *itemChild = branch->child(i); //< this day's clock mode
            QString child1txt = itemChild->text(0);
            if (child1txt != "Clock_Definitions")
            {
               continue;
            }
            QString child2txt = itemChild->text(1);
            if (!child2txt.contains("="))
            {
               continue;
            }
            QStringList child2txtSplit = child2txt.split("=");
            if (child2txtSplit.at(0) == "Clock_Mode")
            {
               if (child2txtSplit.at(1) == "Inherited")
               {
                  inherited = true;
                  break;
               }
               else if (child2txtSplit.at(1) == "Custom")
               {
                  inherited = false;
                  int grandChildrenCount = itemChild->childCount(); //< number of intervals
                  for (int j = 0; j < grandChildrenCount; j++)
                  {
                     CTreeBranch *itemGrandChild = itemChild->child(j);
                     if (!extractClockDefs(itemGrandChild, interv))
                     {
                        return false;
                     }
                  }
               }
               else
               {
                  return false;
               }
            }
         }

         DAY_OF_WEEK_DEF outerPair;
         outerPair.first.first = enabled;
         outerPair.first.second = inherited;
         outerPair.second = interv;
         week.insert(dayName, outerPair);
      }
   }

   for (int i = 0; i < branch->childCount(); i++)
   {
      if (!extractWeekDefs(branch->child(i), week))
      {
         return false;
      }
   }

   return true;
}

bool SautoXml::extractCalendarDefs(CTreeBranch *branch, CALENDAR_DEF &calendar )
{
   if(0 == branch)
   {
      return false;
   }

   QString itemName = branch->text(0);
   QString itemVal  = branch->text(1);
   QStringList itemSplit = itemVal.split("=");

   if(itemSplit.at(0) == "Calendar_Mode")
   {
      if(itemSplit.size() >= 2 && itemSplit.at(1) == "Custom")
      {
         if (!m_defCalendarSet)
         {
            m_everyMonthChecked = false;
         }
      }
      else if(itemSplit.size() >= 2 && itemSplit.at(1) == "Every_Month")
      {
         if (!m_defCalendarSet)
         {
            m_everyMonthChecked = true;
         }
      }
      else
      {
         return false;
      }
   }
   else if(itemName == "Year_Definition")
   {
      int yearNo = 0;
      bool yearIsActive = false;
      MONTH_DEF monthDef;
      QStringList itemValSplit = itemVal.split("=");
      if(itemValSplit.size() >= 2 && itemValSplit.at(0) == "Name")
      {
         yearIsActive = true;
         yearNo = itemValSplit.at(1).toInt();
      }
      int itemChildNo = branch->childCount(); //< number of defined months for this year
      for(int i=0; i<itemChildNo; i++)
      {
         QString monthName;
         DAY_OF_MONTH_DEF monthData;
         CTreeBranch *childItem = branch->child(i);
         if (0 == childItem)
         {
            continue;
         }
         QString childName = childItem->text(0);
         QString childVal  = childItem->text(1);
         if(childName == "Month_Definition")
         {
            QStringList childValSplit = childVal.split(",");
            for(int j=0; j<childValSplit.size(); j++)
            {
               QString tmpElem = childValSplit.at(j);
               if (!tmpElem.contains("="))
               {
                  continue;
               }
               QStringList elemSplit = tmpElem.split("=");
               if(elemSplit.at(0) == "Name")
               {
                  monthName = elemSplit.at(1);
                  if (getMonthAsInt(monthName) < 0)
                  {
                     return false;
                  }
               }
               else if(elemSplit.at(0) == "Inherited")
               {
                  QString elmSplt = elemSplit.at(1);
                  QString boolName = elmSplt.toLower();
                  if (boolName == "true")
                  {
                     monthData.first = true;
                  }
                  else if(boolName == "false")
                  {
                     monthData.first = false;

                     // extract days for this month
                     int grandchildCount = childItem->childCount();
                     QMap<int, CALENDAR_DATE> days;

                     // loop through all day-children
                     for(int k=0; k<grandchildCount; k++)
                     {
                        int dateInt;
                        CALENDAR_DATE dayData;
                        QDate date;
                        bool dateInherit;
                        INTERVAL_LIST intervals;
                        CTreeBranch *dayChild = childItem->child(k);
                        if(0 == dayChild)
                        {
                           continue;
                        }
                        QString dayName = dayChild->text(0);
                        QString dayVal  = dayChild->text(1);
                        if(dayName == "Day_Definition")
                        {
                           QStringList dayValSplit = dayVal.split(",");
                           for(int l=0; l<dayValSplit.size(); l++)
                           {
                              QString dayValElem = dayValSplit.at(l);
                              if (!dayValElem.contains("="))
                              {
                                 continue;
                              }

                              QStringList dayElemSplit = dayValElem.split("=");
                              QString dayElemName = dayElemSplit.at(0);
                              QString dayElemVal  = dayElemSplit.at(1);

                              if(dayElemName == "Name")
                              {
                                 dateInt = dayElemVal.toInt();

                                 // construct a QDate
                                 QString dateStr = QString("%1").arg(dateInt);
                                 int tmpYear  = dateStr.left(4).toInt();
                                 int tmpMonth = dateStr.mid(4,2).toInt();
                                 int tmpDay   = dateStr.right(2).toInt();

                                 date = QDate(tmpYear, tmpMonth, tmpDay);
                              }
                              else if(dayElemName == "Inherited")
                              {
                                 if (dayElemVal == "true")
                                 {
                                    dateInherit = true;
                                 }
                                 else if(dayElemVal == "false")
                                 {
                                    dateInherit = false;
                                    INTERVAL_LIST tmpInterv;
                                    if (!extractClockDefs(dayChild, tmpInterv))
                                    {
                                       dateInherit = true;
                                    }
                                    else if (tmpInterv.size() == 0)
                                    {
                                       dateInherit = true;
                                    }
                                    else
                                    {
                                       // accept this interval
                                       intervals = tmpInterv;
                                    }
                                 }
                                 else
                                 {
                                    return false;
                                 }
                              }
                              else
                              {
                                 return false;
                              }
                           }
                        }
                        else
                        {
                           return false;
                        }

                        // insert the day data into this day
                        dayData.first.first = date;
                        dayData.first.second = dateInherit;
                        dayData.second = intervals;
                        days.insert(dateInt, dayData);
                     }

                     // insert the list of days into the month definition
                     monthData.second = days;
                  }
                  else
                  {
                     return false;
                  }
               }
               else
               {
                  return false;
               }
            }
         }
         else
         {
            return false;
         }

         // insert this month to month definition list
         monthDef.insert(monthName, monthData);
      }

      // insert this year to the calendar list
      QPair<bool, MONTH_DEF> monthDefPair;
      monthDefPair.first = yearIsActive;
      monthDefPair.second = monthDef;
      calendar.insert(yearNo, monthDefPair);
   }

   for(int i=0; i<branch->childCount(); i++)
   {
      if(!extractCalendarDefs(branch->child(i), calendar))
      {
         return false;
      }
   }

   return true;
}

void SautoXml::setCheckers(
   bool asap,
   bool allDay,
   bool everyDay,
   bool everyMonth
   )
{
   m_ASAPChecked = asap;
   m_allDayChecked = allDay;
   m_everyDayChecked = everyDay;
   m_everyMonthChecked = everyMonth;
}

void SautoXml::getCheckers(
   bool &asap,
   bool &allDay,
   bool &everyDay,
   bool &everyMonth
   ) const
{
   asap = m_ASAPChecked;
   allDay = m_allDayChecked;
   everyDay = m_everyDayChecked;
   everyMonth = m_everyMonthChecked;
}

bool sauto::verifyScheduleSettings(
   const SautoModel &freq,
   const INTERVAL_LIST &intr,
   const WEEK_DEF &week,
   const CALENDAR_DEF &cal,
   QString &report
   )
{
   if (freq.getType() == NOT_SPECIFIED &&
      intr.size() == 0 &&
      week.size() == 0 &&
      cal.size() == 0)
   {
      report = "Empty parameters";
      return false;
   }
   return true;
}
