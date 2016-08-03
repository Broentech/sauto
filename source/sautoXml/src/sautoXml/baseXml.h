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
//  \file      baseXml.h
//
//  \brief     Definition of base class to handle XML writing and reading
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

#ifndef _XML_BASE_H
#define _XML_BASE_H

// Qt includes
#include <QString>
#include <QStringList>
#include <QMultiHash>
#include <QByteArray>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

// local includes
#include "tree.h"

namespace sauto {
   static const QString xml_header_ref_key = "href";
   static const QString xml_header_ref_value = "http://www.broentech.no/";

   enum EXML_type
   {
      XML_UNKNOWN,
      XML_SAUTO,
   };
   EXML_type xmlType(const QString &arg);
   QString xmlTypeString(EXML_type type);

   class CXML_base : public QObject
   {
      Q_OBJECT

   public:
      explicit CXML_base(QObject *parent = 0);
      virtual ~CXML_base();
      bool writeToFile(EXML_type type, const QString &fileName, CTreeBranch *root); 
      bool readXml(const QString &fileName, CTreeBranch *root);
      bool readXml(const QByteArray &data , CTreeBranch *root);

   protected: 
      void writeIndexEntry (QXmlStreamWriter *xmlWriter, CTreeBranch *branch);
      void readStream(QXmlStreamReader *xmlReader, CTreeBranch *root);
      void readSkipUnknown(QXmlStreamReader *xmlReader);

      // read xml elements of the class-specific type
      virtual void readEntryElement (QXmlStreamReader *xmlReader, CTreeBranch *parent) = 0;
   };
}

#endif