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
//////////////////////////////////////////////////////////////////////////////
//
//  \file      appwindow.h
//
//  \brief     Main window for sauto-demo
//
//  \author    Stian Broen
//
//  \date      02.08.2016
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
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QProgressBar>
#include <QVBoxLayout>

// solution includes
#include <sautoWidgets/sautoDialog.h>
#include <sauto/sautoManager.h>

namespace sauto {
   class SautoSingleCooldown : public QWidget
   {
      Q_OBJECT

   public:
      explicit SautoSingleCooldown(QWidget *parent = 0);
      ~SautoSingleCooldown();
      void stopClock();
      void pauseClock();
      void startClock();
      void clockFinished(const QString &endReport);
      void constantIntervals();
      void flushAll();
      void triggered();
      void triggered(const QString &taskID);
      void timeToNextSession(quint64 msecsLeft, quint64 msecsStarted, const QString &msg);
      void timeLeft(quint64 msecsLeft, quint64 msecsStarted);
      void timeToNextTrigger(quint64 msecsLeft, quint64 msecsStarted);

   private:
      void initGui();

   private:
      QProgressBar *m_nextInterval;
      QProgressBar *m_nextTrigger;
      QLabel *m_nextIntervalLabel;
      QLabel *m_nextTriggerLabel;
      QLabel *m_message;
      bool m_hasInterval;
   };

   class SautoCooldown : public QWidget
   {
      Q_OBJECT

   public:
      explicit SautoCooldown(QWidget *parent = 0);
      ~SautoCooldown();

   public slots:
      void stopClock(int id);
      void pauseClock(int id);
      void startClock(int id);
      void clockFinished(int id, const QString &endReport);
      void constantIntervals(int id);
      void flushAll(int id);
      void triggered(int id);
      void triggered(int clockId, const QString &taskID);
      void timeToNextSession(int id, quint64 msecsLeft, quint64 msecsStarted, const QString &msg);
      void timeLeft(int id, quint64 msecsLeft, quint64 msecsStarted);
      void timeToNextTrigger(int id, quint64 msecsLeft, quint64 msecsStarted);

   private:
      void initGui();
      void addCd(int id);
      void removeCd(int id);

   private:
      QVBoxLayout *m_lay;
      QMap<int, SautoSingleCooldown *> m_cds;
   };

	class AppWindow : public QMainWindow
	{
		Q_OBJECT
		
	public:
		explicit AppWindow(QWidget *parent = 0);
		~AppWindow();

   signals:

   private slots :
      void greenClick();
      void yellowClick();
      void redClick();
      void sautoClicked();
      void startClicked();
      void stopClicked();
      void itemDoubleClicked(QListWidgetItem *);
      void triggered(int, const QString &);

   private:
      void initGui();
      QWidget *makeTestGui();
      QWidget *makeSautoList();
      QWidget *makeCooldowns();

   private:
      SautoManager *m_sauto;
      QLabel *m_green;
      QLabel *m_yellow;
      QLabel *m_red;
      QPushButton *m_greenSwitch;
      QPushButton *m_yellowSwitch;
      QPushButton *m_redSwitch;
      QWidget *m_centralWidget;
      bool m_greenOn;
      bool m_yellowOn;
      bool m_redOn;
      QPushButton *m_sautoButton;
      QListWidget *m_sautoList;
      QWidget *m_sautoListWidget;
      QWidget *m_cooldownsWidget;
      QPushButton *m_start;
      QPushButton *m_stop;
      quint16 m_indexMax;
      SautoCooldown *m_sautoCd;
	};
}