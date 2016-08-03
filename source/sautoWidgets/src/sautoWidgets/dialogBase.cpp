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
//  \file      dialogBase.cpp
//
//  \brief     Implementation of a base class for dialog. Can't be used as a 
//             standalone dialog, its only for inheriting
//
//  \author    Stian Broen
//
//  \date      04.11.2012
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
#include <QToolButton>
#include <QLineEdit>
#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QTableWidget>
#include <QtDebug>

// local includes
#include "dialogBase.h"

const int hardcodedIconWidth = 64;
const int hardcodedIconHeight = 64;
using namespace sauto;

CDialogBase::CDialogBase(QWidget *parent)
   :QDialog(parent),
   m_closeIsOK(false),
   m_sendFileOnClose(false),
   m_nameLine(0),
   m_filesBox(0),
   m_acceptButton(0),
   m_cancelButton(0)
{
   if (0 == parent)
   {
      this->setObjectName("CDialogBase");
   }
   else
   {
      this->setObjectName(parent->objectName());
   }

   QList<QSize> sizes = QIcon(":/images/OK.png").availableSizes();
   if (sizes.size() > 0)
   {
      m_buttonSize = sizes.at(0);
      m_buttonSize.setWidth(m_buttonSize.width() / 2);
      m_buttonSize.setHeight(m_buttonSize.height() / 2);
   }
   else
   {
      m_buttonSize.setWidth(hardcodedIconWidth);
      m_buttonSize.setHeight(hardcodedIconHeight);
   }
}

void CDialogBase::updateFilesBox()
{
   if(0 == m_filesBox)
   {
      return;
   }
   m_filesBox->clear();
   m_filesBox->addItems(m_files);
   updateNameLine();
}

void CDialogBase::updateNameLine()
{
   if (m_files.size() == 0)
   {
      m_nameLine->setPlaceholderText("default");
   }
   else
   {
      const QFileInfo fInfo(m_lastSavedXML);
      const QString lastSavedBase = fInfo.baseName();
      if(m_files.contains(lastSavedBase))
      {
         int index = m_filesBox->findText(lastSavedBase);
         if (index > 0)
         {
            m_filesBox->setCurrentIndex(index);
         }
         m_nameLine->setText(lastSavedBase);
      }
   }
}

QVBoxLayout* CDialogBase::initBaseWidgets()
{
   m_acceptButton = new QToolButton(this);
   m_acceptButton->setToolTip(tr("Accept"));
   m_acceptButton->setIcon(QIcon(":/images/OK.png"));
   m_acceptButton->setIconSize(m_buttonSize);
   m_acceptButton->setMinimumSize(m_buttonSize);
   m_acceptButton->setMaximumSize(m_buttonSize);

   connect(m_acceptButton, SIGNAL(clicked()), 
      this, SLOT(acceptClicked()));

   m_cancelButton = new QToolButton(this);
   m_cancelButton->setToolTip(tr("Cancel"));
   m_cancelButton->setIcon(QIcon(":/images/NOT_OK.png"));
   m_cancelButton->setIconSize(m_buttonSize);
   m_cancelButton->setMinimumSize(m_buttonSize);
   m_cancelButton->setMaximumSize(m_buttonSize);

   connect(m_cancelButton, SIGNAL(clicked()), 
      this, SLOT(cancelClicked()));

   QHBoxLayout *buttonLay = new QHBoxLayout;
   buttonLay->addWidget(m_acceptButton);
   buttonLay->addWidget(m_cancelButton);
   buttonLay->addStretch();

   m_nameLine = new QLineEdit(this);
   m_nameLine->setText(QString(m_defaultLineText));
   QLabel *nameLabel = new QLabel(this);
   nameLabel->setText(tr("Settings Name : "));
   m_filesBox = new QComboBox(this);

   connect(m_filesBox, SIGNAL(activated(const QString &)), 
      this, SLOT(fileSelectedFromBox(const QString &))); 

   QHBoxLayout *nameLay = new QHBoxLayout;
   nameLay->addWidget(nameLabel);
   nameLay->addWidget(m_nameLine);
   nameLay->addWidget(m_filesBox);
   nameLay->addStretch();

   QVBoxLayout *returnLay = new QVBoxLayout;
   returnLay->addLayout(nameLay);
   returnLay->addLayout(buttonLay);
   return returnLay;
}

void CDialogBase::cancelClicked()
{
   m_closeIsOK = true;
   emit finished();
   this->reject();
}

bool CDialogBase::checkTableItem(QTableWidget *table, int row, int column)
{
   if (0 == table || row < 0 || column < 0)
   {
      return false;
   }
   if (0 == table->item(row, column))
   {
      return false;
   }
   return true;
}
