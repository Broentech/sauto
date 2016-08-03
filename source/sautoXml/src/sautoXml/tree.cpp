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
//  \file      tree.cpp
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

// local includes
#include "tree.h"

using namespace sauto;

CTreeBranch::CTreeBranch(const QStringPair &data, CTreeBranch *parent)
   :m_data(data),
    m_parent(parent)
{
   if (0 != parent)
   {
      parent->addChild(this);
   }
}

CTreeBranch::CTreeBranch(CTreeBranch *parent)
   :m_parent(parent)
{
   if (0 != parent)
   {
      parent->addChild(this);
   }
}

CTreeBranch::~CTreeBranch()
{
   qDeleteAll(m_children);
}

void CTreeBranch::setText(int index, const QString &val)
{
   if (index == 0)
   {
      m_data.first = val;
   }
   else if (index == 1)
   {
      m_data.second = val;
   }
}

QString CTreeBranch::text(int index) const
{
   if (index == 0)
   {
      return m_data.first;
   }
   else if (index == 1)
   {
      return m_data.second;
   }
   else
   {
      return "";
   }
}

CTreeBranch* CTreeBranch::parent() const
{
   return m_parent;
}

CTreeBranch* CTreeBranch::child(int index) const
{
   if (index < 0 || index >= childCount())
   {
      return 0;
   }
   return m_children.at(index);
}

int CTreeBranch::childCount() const
{
   return m_children.count();
}

void CTreeBranch::addChild(CTreeBranch *child)
{
   if (0 == child || m_children.contains(child))
   {
      return;
   }
   if (0 != child->parent())
   {
      child->parent()->removeChild(child);
   }
   child->setParent(this);
   m_children.append(child);
}

void CTreeBranch::removeChild(CTreeBranch *child)
{
   if(m_children.contains(child))
   {
      child->setParent(0);
      m_children.removeAll(child);
   }
}

CTreeBranch* CTreeBranch::invisibleRootItem()
{
   if (0 == m_parent)
   {
      return this;
   }
   else
   {
      return m_parent->invisibleRootItem();
   }
}

void CTreeBranch::setParent(CTreeBranch *parent)
{
   m_parent = parent;
}