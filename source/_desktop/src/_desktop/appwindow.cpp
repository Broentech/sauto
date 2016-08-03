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
//  \file      appwindow.cpp
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
#include <QPixmap>
#include <QHBoxLayout>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDesktopServices>
#include <QUrl>
#include <QtDebug>
#include <QGridLayout>
#include <QDateTime>

// solution includes
#include <sautoXml/sautoXml.h>

// local includes
#include "appwindow.h"

using namespace sauto;

AppWindow::AppWindow(QWidget *parent)
   :QMainWindow(parent),
   m_sauto(0),
   m_green(0),
   m_yellow(0),
   m_red(0),
   m_greenSwitch(0),
   m_yellowSwitch(0),
   m_redSwitch(0),
   m_centralWidget(0),
   m_greenOn(false),
   m_yellowOn(false),
   m_redOn(false),
   m_sautoButton(0),
   m_sautoList(0),
   m_sautoListWidget(0),
   m_cooldownsWidget(0),
   m_start(0),
   m_stop(0),
   m_indexMax(0),
   m_sautoCd(0)
{
   m_sauto = new SautoManager(this);

   connect(m_sauto, SIGNAL(triggered(int, const QString &)),
      this, SLOT(triggered(int, const QString &)));

   initGui();

   // lets make some example tasks
   QString _tasksdir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/tasks";
   QDir dir(_tasksdir);
   if (!dir.exists())
   {
      dir.mkpath(_tasksdir);
      QFile greenOn(_tasksdir + "/greenOn.xml");
      greenOn.open(QIODevice::WriteOnly);
      greenOn.close();

      QFile greenOff(_tasksdir + "/greenOff.xml");
      greenOff.open(QIODevice::WriteOnly);
      greenOff.close();

      QFile yellowOn(_tasksdir + "/yellowOn.xml");
      yellowOn.open(QIODevice::WriteOnly);
      yellowOn.close();

      QFile yellowOff(_tasksdir + "/yellowOff.xml");
      yellowOff.open(QIODevice::WriteOnly);
      yellowOff.close();

      QFile redOn(_tasksdir + "/redOn.xml");
      redOn.open(QIODevice::WriteOnly);
      redOn.close();

      QFile redOff(_tasksdir + "/redOff.xml");
      redOff.open(QIODevice::WriteOnly);
      redOff.close();
   }
}

AppWindow::~AppWindow()
{

}

void AppWindow::initGui()
{
   m_centralWidget = makeTestGui();
   m_sautoListWidget = makeSautoList();
   m_sautoListWidget->setVisible(m_sautoList->count() > 0);
   m_cooldownsWidget = makeCooldowns();
   QVBoxLayout *llay = new QVBoxLayout;
   llay->addWidget(m_centralWidget);
   llay->addWidget(m_sautoListWidget);
   QHBoxLayout *lay = new QHBoxLayout;
   lay->addLayout(llay);
   lay->addWidget(m_cooldownsWidget);
   QWidget *w = new QWidget(this);
   w->setLayout(lay);
   this->setCentralWidget(w);
}

QWidget* AppWindow::makeTestGui()
{
   m_green = new QLabel(this);
   m_green->setPixmap(QPixmap(":/images/off.png"));

   m_greenSwitch = new QPushButton(this);
   m_greenSwitch->setIcon(QIcon(":/images/switch-off.png"));
   m_greenSwitch->setIconSize(QSize(128, 66));
   m_greenSwitch->setFlat(true);
   m_greenSwitch->setStyleSheet("* { background-color: rgba(0,125,0,0) }");

   QVBoxLayout *greenlay = new QVBoxLayout;
   greenlay->addWidget(m_green);
   greenlay->addWidget(m_greenSwitch);

   connect(m_greenSwitch, SIGNAL(clicked()),
      this, SLOT(greenClick()));

   m_yellow = new QLabel(this);
   m_yellow->setPixmap(QPixmap(":/images/off.png"));

   m_yellowSwitch = new QPushButton(this);
   m_yellowSwitch->setIcon(QIcon(":/images/switch-off.png"));
   m_yellowSwitch->setIconSize(QSize(128, 66));
   m_yellowSwitch->setFlat(true);
   m_yellowSwitch->setStyleSheet("* { background-color: rgba(0,125,0,0) }");

   QVBoxLayout *yellowlay = new QVBoxLayout;
   yellowlay->addWidget(m_yellow);
   yellowlay->addWidget(m_yellowSwitch);

   connect(m_yellowSwitch, SIGNAL(clicked()),
      this, SLOT(yellowClick()));

   m_red = new QLabel(this);
   m_red->setPixmap(QPixmap(":/images/off.png"));

   m_redSwitch = new QPushButton(this);
   m_redSwitch->setIcon(QIcon(":/images/switch-off.png"));
   m_redSwitch->setIconSize(QSize(128, 66));
   m_redSwitch->setFlat(true);
   m_redSwitch->setStyleSheet("* { background-color: rgba(0,125,0,0) }");

   QVBoxLayout *redlay = new QVBoxLayout;
   redlay->addWidget(m_red);
   redlay->addWidget(m_redSwitch);

   connect(m_redSwitch, SIGNAL(clicked()),
      this, SLOT(redClick()));

   QFrame* line_1 = new QFrame();
   line_1->setFrameShape(QFrame::HLine);
   line_1->setFrameShadow(QFrame::Sunken);

   m_sautoButton = new QPushButton(this);
   m_sautoButton->setText("SAUTO");

   QHBoxLayout *sautoLay = new QHBoxLayout;
   sautoLay->addStretch();
   sautoLay->addWidget(m_sautoButton);
   sautoLay->addStretch();

   connect(m_sautoButton, SIGNAL(clicked()),
      this, SLOT(sautoClicked()));

   QHBoxLayout *lay = new QHBoxLayout;
   lay->addLayout(greenlay);
   lay->addLayout(yellowlay);
   lay->addLayout(redlay);
   lay->addStretch();

   QVBoxLayout *vlay = new QVBoxLayout;
   vlay->addLayout(lay);
   vlay->addWidget(line_1);
   vlay->addLayout(sautoLay);

   QWidget *w = new QWidget(this);
   w->setLayout(vlay);
   return w;
}

QWidget* AppWindow::makeSautoList()
{
   m_sautoList = new QListWidget(this);
   connect(m_sautoList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
      this, SLOT(itemDoubleClicked(QListWidgetItem *)));
   QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/xml/";
   QDir dir(path);
   dir.setFilter(QDir::NoDotAndDotDot | QDir::Files);
   dir.setSorting(QDir::Name);
   QFileInfoList flist = dir.entryInfoList();
   for (int i = 0; i < flist.size(); i++)
   {
      QFileInfo fInfo = flist.at(i);
      QListWidgetItem *item = new QListWidgetItem;
      item->setToolTip(fInfo.filePath());
      item->setText(fInfo.baseName());
      item->setData(Qt::UserRole, ++m_indexMax);
      m_sautoList->addItem(item);
   }

   m_start = new QPushButton(this);
   m_start->setIcon(QIcon(":/images/start.png"));
   m_start->setIconSize(QSize(64, 64));
   connect(m_start, SIGNAL(clicked()),
      this, SLOT(startClicked()));

   m_stop = new QPushButton(this);
   m_stop->setIcon(QIcon(":/images/stop.png"));
   m_stop->setIconSize(QSize(64, 64));

   connect(m_stop, SIGNAL(clicked()),
      this, SLOT(stopClicked()));

   QHBoxLayout *buttonLay = new QHBoxLayout;
   buttonLay->addStretch();
   buttonLay->addWidget(m_start);
   buttonLay->addWidget(m_stop);
   buttonLay->addStretch();

   QVBoxLayout *lay = new QVBoxLayout;
   lay->addWidget(m_sautoList);
   lay->addLayout(buttonLay);
   QWidget *w = new QWidget(this);
   w->setLayout(lay);
   return w;
}

QWidget* AppWindow::makeCooldowns()
{
   m_sautoCd = new SautoCooldown(this);

   connect(m_sauto, SIGNAL(stopClock_sig(int)),
      m_sautoCd, SLOT(stopClock(int)));

   connect(m_sauto, SIGNAL(pauseClock_sig(int)),
      m_sautoCd, SLOT(pauseClock(int)));

   connect(m_sauto, SIGNAL(startClock_sig(int)),
      m_sautoCd, SLOT(startClock(int)));

   connect(m_sauto, SIGNAL(clockFinished(int, const QString &)),
      m_sautoCd, SLOT(clockFinished(int, const QString &)));

   connect(m_sauto, SIGNAL(constantIntervals(int)),
      m_sautoCd, SLOT(constantIntervals(int)));

   connect(m_sauto, SIGNAL(flushAll(int)),
      m_sautoCd, SLOT(flushAll(int)));

   connect(m_sauto, SIGNAL(triggered(int)),
      m_sautoCd, SLOT(triggered(int)));

   connect(m_sauto, SIGNAL(triggered(int, const QString &)),
      m_sautoCd, SLOT(triggered(int, const QString &)));

   connect(m_sauto, SIGNAL(timeToNextSession(int, quint64, quint64, const QString &)),
      m_sautoCd, SLOT(timeToNextSession(int, quint64, quint64, const QString &)));

   connect(m_sauto, SIGNAL(timeLeft(int, quint64, quint64)),
      m_sautoCd, SLOT(timeLeft(int, quint64, quint64)));

   connect(m_sauto, SIGNAL(timeToNextTrigger(int, quint64, quint64)),
      m_sautoCd, SLOT(timeToNextTrigger(int, quint64, quint64)));

   QVBoxLayout *lay = new QVBoxLayout;
   lay->addWidget(m_sautoCd);
   QWidget *w = new QWidget(this);
   w->setLayout(lay);
   return w;
}

void AppWindow::triggered(int id, const QString &task)
{
   if (task == "greenOff")
   {
      m_green->setPixmap(QPixmap(":/images/off.png"));
      m_greenSwitch->setIcon(QIcon(":/images/switch-off.png"));
   }
   else if (task == "greenOn")
   {
      m_green->setPixmap(QPixmap(":/images/green.png"));
      m_greenSwitch->setIcon(QIcon(":/images/switch-on.png"));
   }
   else if (task == "redOff")
   {
      m_red->setPixmap(QPixmap(":/images/off.png"));
      m_redSwitch->setIcon(QIcon(":/images/switch-off.png"));
   }
   else if (task == "redOn")
   {
      m_red->setPixmap(QPixmap(":/images/red.png"));
      m_redSwitch->setIcon(QIcon(":/images/switch-on.png"));
   }
   else if (task == "yellowOff")
   {
      m_yellow->setPixmap(QPixmap(":/images/off.png"));
      m_yellowSwitch->setIcon(QIcon(":/images/switch-off.png"));
   }
   else if (task == "yellowOn")
   {
      m_yellow->setPixmap(QPixmap(":/images/yellow.png"));
      m_yellowSwitch->setIcon(QIcon(":/images/switch-on.png"));
   }
}

void AppWindow::greenClick()
{
   m_greenOn = !m_greenOn;
   if (m_greenOn)
   {
      m_green->setPixmap(QPixmap(":/images/green.png"));
      m_greenSwitch->setIcon(QIcon(":/images/switch-on.png"));
   }
   else
   {
      m_green->setPixmap(QPixmap(":/images/off.png"));
      m_greenSwitch->setIcon(QIcon(":/images/switch-off.png"));
   }
}

void AppWindow::yellowClick()
{
   m_yellowOn = !m_yellowOn;
   if (m_yellowOn)
   {
      m_yellow->setPixmap(QPixmap(":/images/yellow.png"));
      m_yellowSwitch->setIcon(QIcon(":/images/switch-on.png"));
   }
   else
   {
      m_yellow->setPixmap(QPixmap(":/images/off.png"));
      m_yellowSwitch->setIcon(QIcon(":/images/switch-off.png"));
   }
}

void AppWindow::redClick()
{
   m_redOn = !m_redOn;
   if (m_redOn)
   {
      m_red->setPixmap(QPixmap(":/images/red.png"));
      m_redSwitch->setIcon(QIcon(":/images/switch-on.png"));
   }
   else
   {
      m_red->setPixmap(QPixmap(":/images/off.png"));
      m_redSwitch->setIcon(QIcon(":/images/switch-off.png"));
   }
}

void AppWindow::sautoClicked()
{
   CScheduleDialog dialog(this);
   if (dialog.exec() == QDialog::Accepted)
   {
      QFileInfo fInfo(dialog.lastSavedFile());
      if (fInfo.exists() && m_sautoList->findItems(fInfo.baseName(), Qt::MatchExactly).size() == 0)
      {
         QListWidgetItem *item = new QListWidgetItem;
         item->setToolTip(fInfo.filePath());
         item->setText(fInfo.baseName());
         item->setData(Qt::UserRole, ++m_indexMax);
         m_sautoList->addItem(item);
      }
      m_sautoListWidget->setVisible(m_sautoList->count() > 0);
   }
}

void AppWindow::startClicked()
{
   QListWidgetItem *item = m_sautoList->currentItem();
   if (0 == item)
   {
      return;
   }
   QString usepath = item->toolTip();
   int id = item->data(Qt::UserRole).toInt();
   SautoXml xml;
   SautoModel frequency;
   INTERVAL_LIST timeIntervals;
   WEEK_DEF week;
   CALENDAR_DEF calendar;
   if (!xml.readClockFile(
         usepath,
         frequency,
         timeIntervals,
         week,
         calendar)
      )
   {
      qCritical() << "Failed at loading file " << usepath;
      return;
   }
   if (m_sauto->hasClock(id))
   {
      m_sauto->stopClock(id);
      m_sauto->removeClock(id);
   }
   if (m_sauto->addClock(id, frequency, timeIntervals, week, calendar))
   {
      m_sauto->startClock(id);
   }
}

void AppWindow::stopClicked()
{
   QListWidgetItem *item = m_sautoList->currentItem();
   if (0 == item)
   {
      return;
   }
   QString usepath = item->toolTip();
   int id = item->data(Qt::UserRole).toInt();
   if (m_sauto->hasClock(id))
   {
      m_sauto->stopClock(id);
      m_sauto->removeClock(id);
   }
}

void AppWindow::itemDoubleClicked(QListWidgetItem *item)
{
   QDesktopServices::openUrl(QUrl::fromLocalFile(item->toolTip()));
}

SautoCooldown::SautoCooldown(QWidget *parent)
   :QWidget(parent),
   m_lay(0)
{
   initGui();
}

SautoCooldown::~SautoCooldown()
{

}

void SautoCooldown::initGui()
{
   m_lay = new QVBoxLayout;
   QVBoxLayout *lay = new QVBoxLayout;
   lay->addLayout(m_lay);
   lay->addStretch();
   this->setLayout(lay);
}

void SautoCooldown::addCd(int id)
{
   if (m_cds.contains(id))
   {
      return;
   }

   SautoSingleCooldown *cd = new SautoSingleCooldown(this);
   m_lay->addWidget(cd);
   m_cds[id] = cd;
   update();
}

void SautoCooldown::removeCd(int id)
{
   if (!m_cds.contains(id))
   {
      return;
   }
   m_lay->removeWidget(m_cds[id]);
   m_cds[id]->deleteLater();
   m_cds.remove(id);
   update();
}

void SautoCooldown::stopClock(int id)
{
   addCd(id);
   m_cds[id]->stopClock();
}

void SautoCooldown::pauseClock(int id)
{
   addCd(id);
   m_cds[id]->pauseClock();
}

void SautoCooldown::startClock(int id)
{
   addCd(id);
   m_cds[id]->startClock();
}

void SautoCooldown::clockFinished(int id, const QString &endReport)
{
   removeCd(id);
}

void SautoCooldown::constantIntervals(int id)
{
   addCd(id);
   m_cds[id]->constantIntervals();
}

void SautoCooldown::flushAll(int id)
{
   addCd(id);
   m_cds[id]->flushAll();
}

void SautoCooldown::triggered(int id)
{
   addCd(id);
   m_cds[id]->triggered();
}

void SautoCooldown::triggered(int clockId, const QString &taskID)
{
   addCd(clockId);
   m_cds[clockId]->triggered(taskID);
}

void SautoCooldown::timeToNextSession(int id, quint64 msecsLeft, quint64 msecsStarted, const QString &msg)
{
   addCd(id);
   m_cds[id]->timeToNextSession(msecsLeft, msecsStarted, msg);
}

void SautoCooldown::timeLeft(int id, quint64 msecsLeft, quint64 msecsStarted)
{
   addCd(id);
   m_cds[id]->timeLeft(msecsLeft, msecsStarted);
}

void SautoCooldown::timeToNextTrigger(int id, quint64 msecsLeft, quint64 msecsStarted)
{
   addCd(id);
   m_cds[id]->timeToNextTrigger(msecsLeft, msecsStarted);
}

SautoSingleCooldown::SautoSingleCooldown(QWidget *parent)
   :QWidget(parent),
   m_nextInterval(0),
   m_nextTrigger(0),
   m_nextIntervalLabel(0),
   m_nextTriggerLabel(0),
   m_message(0),
   m_hasInterval(false)
{
   initGui();
}

SautoSingleCooldown::~SautoSingleCooldown()
{

}

void SautoSingleCooldown::initGui()
{
   m_nextInterval = new QProgressBar(this);
   m_nextInterval->setFormat("%v ms");
   m_nextTrigger = new QProgressBar(this);
   m_nextTrigger->setFormat("%v ms");
   m_nextIntervalLabel = new QLabel("Next Interval", this);
   m_nextTriggerLabel = new QLabel("Next Trigger", this);

   QGridLayout *grid = new QGridLayout;
   grid->addWidget(m_nextIntervalLabel, 0, 0);
   grid->addWidget(m_nextInterval, 0, 1);
   grid->addWidget(m_nextTriggerLabel, 1, 0);
   grid->addWidget(m_nextTrigger, 1, 1);

   m_message = new QLabel(this);

   QVBoxLayout *llay = new QVBoxLayout;
   llay->addLayout(grid);
   llay->addWidget(m_message);

   this->setLayout(llay);

   m_nextIntervalLabel->setVisible(false);
   m_nextInterval->setVisible(false);
}

void SautoSingleCooldown::stopClock()
{
   m_message->setText("Stopped at " + QDateTime::currentDateTime().toString("yyyy.MM.dd, hh:mm:ss"));
}

void SautoSingleCooldown::pauseClock()
{
   m_message->setText("Paused at " + QDateTime::currentDateTime().toString("yyyy.MM.dd, hh:mm:ss"));
}

void SautoSingleCooldown::startClock()
{
   m_message->setText("Started at " + QDateTime::currentDateTime().toString("yyyy.MM.dd, hh:mm:ss"));
}

void SautoSingleCooldown::clockFinished(const QString &endReport)
{
   m_message->setText("Finished at " + QDateTime::currentDateTime().toString("yyyy.MM.dd, hh:mm:ss") + "\n" + endReport);
}

void SautoSingleCooldown::constantIntervals()
{

}

void SautoSingleCooldown::flushAll()
{
   m_message->setText("Flushed at " + QDateTime::currentDateTime().toString("yyyy.MM.dd, hh:mm:ss"));
   m_nextInterval->reset();
   m_nextTrigger->reset();
}

void SautoSingleCooldown::triggered()
{
   m_message->setText("Triggered at " + QDateTime::currentDateTime().toString("yyyy.MM.dd, hh:mm:ss"));
}

void SautoSingleCooldown::triggered(const QString &taskID)
{
   m_message->setText("Triggered at " + QDateTime::currentDateTime().toString("yyyy.MM.dd, hh:mm:ss") + "\n" + taskID);
}

void SautoSingleCooldown::timeToNextSession(quint64 msecsLeft, quint64 msecsStarted, const QString &msg)
{
   if (!m_hasInterval)
   {
      m_hasInterval = true;
      m_nextIntervalLabel->setVisible(true);
      m_nextInterval->setVisible(true);
   }
   m_nextInterval->setRange(0, msecsStarted);
   m_nextInterval->setValue(msecsLeft);
}

void SautoSingleCooldown::timeLeft(quint64 msecsLeft, quint64 msecsStarted)
{
   if (m_hasInterval)
   {
      m_nextInterval->setRange(0, msecsStarted);
      m_nextInterval->setValue(msecsLeft);
   }
}

void SautoSingleCooldown::timeToNextTrigger(quint64 msecsLeft, quint64 msecsStarted)
{
   m_nextTrigger->setRange(0, msecsStarted);
   m_nextTrigger->setValue(msecsLeft);
}


