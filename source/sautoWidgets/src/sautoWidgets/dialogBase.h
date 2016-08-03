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
//  \file      dialogBase.h
//
//  \brief     Definition of a base class for dialog. Can't be used as a 
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

#ifndef _DIALOG_BASE_H
#define _DIALOG_BASE_H

// Qt includes
#include <QDialog>
#include <QComboBox>
#include <QFileInfo>
#include <QDir>
#include <QTableWidgetItem>

// Qt forward declarations
class QToolButton;
class QComboBox;
class QLineEdit;
class QVBoxLayout;
class QTableWidget;

namespace sauto {
   class CDialogBase : public QDialog
   {
      Q_OBJECT

   public:
      CDialogBase(QWidget *parent = 0);
      virtual ~CDialogBase() {}
      inline void sendMeFile() { m_sendFileOnClose = true; }

   public slots:
      virtual void updateWidgets() = 0; //< MUST IMPLEMENT

   protected:
      virtual bool saveToXmlFile(QString &path) = 0; //< MUST IMPLEMENT
      virtual void updateFilesBox();
      virtual void updateNameLine();
      bool checkTableItem(QTableWidget *table, int row, int column);
      QVBoxLayout* initBaseWidgets();

   private slots:
      virtual void cancelClicked();
      virtual void fileSelectedFromBox(const QString &baseName) = 0; //< MUST IMPLEMENT
      virtual void acceptClicked() = 0; //< MUST IMPLEMENT

   signals:
      void saved();
      void finished();
      void settingsSig(const QString &filePath);

   protected:
      QStringList m_files;
      bool m_closeIsOK;
      bool m_sendFileOnClose;
      QSize m_buttonSize;
      QString m_defaultLineText;
      QString m_lastSavedXML;
      QLineEdit *m_nameLine;
      QComboBox *m_filesBox;
      QToolButton *m_acceptButton;
      QToolButton *m_cancelButton;
   };
}

#endif