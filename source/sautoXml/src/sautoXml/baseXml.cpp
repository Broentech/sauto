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
//  \file      baseXml.cpp
//
//  \brief     Implementation of base class to handle XML writing and reading
//
//  \author    Stian Broen
//
//  \date      25.07.2012
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
#include <QtDebug>

// local includes
#include "baseXml.h"

using namespace sauto;

CXML_base::CXML_base(QObject *parent)
   :QObject(parent)
{

}

CXML_base::~CXML_base()
{

}

bool CXML_base::writeToFile(EXML_type type, const QString &fileName, CTreeBranch *root)
{
   if(0 == root)
   {
      qCritical() << QString("%1 %2 : %3")
         .arg(tr("Unable to write XML to"))
         .arg(fileName)
         .arg(tr("Invalid argument data"));
      return false;
   }

   QFile file(fileName);
   if(!file.open(QFile::WriteOnly | QFile::Text))
   {
      qCritical() << QString("Unable to write XML to %1 : %2")
         .arg(fileName)
         .arg(file.errorString());
      return false;
   }

   qDebug() << QString(" %1 %2")
      .arg(tr("Writing argument data as XML to"))
      .arg(fileName);

   QXmlStreamWriter stream(&file);
   stream.setAutoFormatting(true);
   stream.writeStartDocument();

   stream.writeStartElement(xmlTypeString(type));
   stream.writeAttribute(xml_header_ref_key, xml_header_ref_value);

   for(int i=0; i<root->childCount(); i++)
   {
      writeIndexEntry(&stream, root->child(i));
   }

   stream.writeEndElement(); // Session
   stream.writeEndDocument();

   return true;
}

void CXML_base::writeIndexEntry(QXmlStreamWriter *xmlWriter, CTreeBranch *branch)
{
   const QString entryStr = branch->text(0);
   xmlWriter->writeStartElement("entry");
   xmlWriter->writeAttribute("term", entryStr);
   const QString entries = branch->text(1);
   if(!entries.isEmpty())
   {
      if(entries.contains(","))
      {
         const QStringList entriesSplit = entries.split(",");
         for(int i=0; i<entriesSplit.size(); i++)
         {
            QString spltAt = entriesSplit.at(i);
            QStringList secondSplit;
            if (spltAt.contains('=') && !spltAt.contains(':'))
            {
               secondSplit = spltAt.split('=');
            }
            else if (!spltAt.contains('=') && spltAt.contains(':'))
            {
               secondSplit = spltAt.split(':');
            }
            else if(spltAt.contains('=') && spltAt.contains(':'))
            {
               // in this case, the '=' counts as the variable/value separator
               secondSplit = spltAt.split('=');
            }
            if (secondSplit.size() == 2)
            {
               xmlWriter->writeTextElement(secondSplit.at(0), secondSplit.at(1));
            }
            else
            {
               qCritical() << "XML writing error, non-accepted variable/value pair :" << spltAt;
            }
         }
      }
      else if(entries.contains("="))
      {
         const QStringList secondSplit = entries.split("=");
         if (secondSplit.size() == 2)
         {
            xmlWriter->writeTextElement(secondSplit.at(0), secondSplit.at(1));
         }
      }
      else if(entries.contains(":"))
      {
         const QStringList secondSplit = entries.split(":");
         if (secondSplit.size() == 2)
         {
            xmlWriter->writeTextElement(secondSplit.at(0), secondSplit.at(1));
         }
      }
      else
      {
         xmlWriter->writeTextElement(entryStr, entries);
      }
   }
   for (int i = 0; i < branch->childCount(); i++)
   {
      writeIndexEntry(xmlWriter, branch->child(i));
   }
   xmlWriter->writeEndElement();
}

bool CXML_base::readXml(const QString &fileName, CTreeBranch *root)
{
   if(0 == root)
   {
      qCritical() << QString("%1 %2 : %3")
         .arg(tr("Unable to read XML from"))
         .arg(fileName)
         .arg(tr("Invalid argument"));
      return false;
   }

   QFile file(fileName);
   if(!file.open(QFile::ReadOnly | QFile::Text))
   {
      qCritical() << QString("%1 %2 : %3")
         .arg(tr("Unable to read XML from"))
         .arg(fileName)
         .arg(tr("Can't open file for reading"));
      return false;
   }

   qDebug() << QString("%1 %2")
      .arg(tr("Reading XML from"))
      .arg(fileName);

   QXmlStreamReader stream;
   stream.setDevice(&file);
   stream.readNext();
   while(!stream.atEnd())
   {
      if(stream.isStartElement())
      {
         readStream(&stream, root);
      }
      else
      {
         stream.readNext();
      }
   }

   file.close();
   if(stream.hasError())
   {
      return false;
   }
   if(file.error() != QFile::NoError)
   {  
      return false;
   }

   return true;
}

bool CXML_base::readXml(const QByteArray &data , CTreeBranch *root)
{
   if(0 == root || data.size() <= 0)
   {
      qCritical() << "Unable to read XML, invalid argument(s)";
      return false;
   }

   QXmlStreamReader stream(data);
   stream.readNext();
   while(!stream.atEnd())
   {
      if (stream.isStartElement())
      {
         readStream(&stream, root);
      }
      else
      {
         stream.readNext();
      }
   }

   if(stream.hasError())
   {
      qCritical() << QString("%1 '%2'")
         .arg(tr("QXmlStreamReader failed, last error :"))
         .arg(stream.errorString());
      return false;
   }

   return true;
}

void CXML_base::readStream(QXmlStreamReader *xmlReader, CTreeBranch *root)
{
   if(0 == xmlReader || 0 == root)
   {
      return;
   }

   xmlReader->readNext();
   while(!xmlReader->atEnd())
   {
      if(xmlReader->isEndElement())
      {
         xmlReader->readNext();
         break;
      }

      if(xmlReader->isStartElement())
      {
         if(xmlReader->name() == "entry")
         {
            readEntryElement(xmlReader, root->invisibleRootItem());
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

void CXML_base::readSkipUnknown(QXmlStreamReader *xmlReader)
{
   if(0 == xmlReader)
   {
      return;
   }

   xmlReader->readNext();
   while(!xmlReader->atEnd())
   {
      if(xmlReader->isEndElement())
      {
         xmlReader->readNext();
         break;
      }

      if(xmlReader->isStartElement())
      {
         readSkipUnknown(xmlReader);
      }
      else
      {
         xmlReader->readNext();
      }
   }
}

EXML_type sauto::xmlType(const QString &arg)
{
   if (arg == "sauto")
   {
      return XML_SAUTO;
   }
   else
   {
      return XML_UNKNOWN;
   }
}

QString sauto::xmlTypeString(EXML_type type)
{
   switch (type)
   {
   case(XML_SAUTO) :
      return "sauto";
   default:
   return "Unknown_XML_type";
   }
}