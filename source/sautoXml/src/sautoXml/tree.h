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
//  \file      tree.h
//
//  \brief     Definition of a tree datastructure used for XML parsing
//
//  \author    Stian Broen
//
//  \date      07.10.2013
//
//  \par       Copyright: BroenTech AS
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

#ifndef _MANAS_XML_TREE_H_
#define _MANAS_XML_TREE_H_

// Qt includes
#include <QString>
#include <QPair>
#include <QList>

typedef QPair<QString, QString> QStringPair;

namespace sauto {
   class CTreeBranch
   {
   public:
      explicit CTreeBranch(const QStringPair &data, CTreeBranch *parent = 0);
      explicit CTreeBranch(CTreeBranch *parent = 0);
      ~CTreeBranch();
      void setText(int index, const QString &val);
      QString text(int index) const;
      CTreeBranch* parent() const;
      CTreeBranch* child(int index) const;
      int childCount() const;
      void addChild(CTreeBranch *child);
      void removeChild(CTreeBranch *child);
      CTreeBranch *invisibleRootItem();
      void setParent(CTreeBranch *parent);

   private:
      CTreeBranch *m_parent;
      QStringPair  m_data;
      QList<CTreeBranch *> m_children;
   };
}

#endif