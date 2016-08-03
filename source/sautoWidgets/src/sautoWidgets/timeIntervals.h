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
//  \file      timeIntervals.h
//
//  \brief     Definition of widget to specify a time interval
//
//  \author    Stian Broen
//
//  \date      27.06.2012
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

#ifndef _TIME_INTERVALS_H
#define _TIME_INTERVALS_H

// Qt includes
#include <QWidget>
#include <QPolygon>
#include <QPainter>
#include <QDialog>

// local includes
#include "waveletWidget.h"

// Qt forward declarations
class QTimeEdit;
class QPaintEvent;
class QMouseEvent;
class QPushButton;
class QRadioButton;
class QMenu;

namespace sauto {
   class TimeInterval
   {
   public:
      TimeInterval()
      {
         m_useFreq = false;
         m_hasFocus = false;
         m_index = 0;
         m_area = QRectF(0, 0, 0, 0);
         m_position_start = 0;
         m_position_stop = 0;
      }

      SautoModel  m_waveData;
      bool m_useFreq;
      bool m_hasFocus;
      int m_index;
      QRectF m_area;
      qreal m_position_start;
      qreal m_position_stop;
   };

   class CTimeLineView : public QWidget
   {
      Q_OBJECT

   public:
      explicit CTimeLineView(QWidget *parent = 0);
      ~CTimeLineView();
      INTERVAL_LIST makeTimeIntervals();
      void setData(const INTERVAL_LIST &intervals);
      void clearAll();
      void updateWidgetContent();

   protected:
      void paintEvent(QPaintEvent *);
      void mousePressEvent(QMouseEvent *);
      void mouseMoveEvent(QMouseEvent *);
      void mouseReleaseEvent(QMouseEvent *);

   public slots:
      void addSingleShot(const QTime &time);
      void addInterval(quint64 start, quint64 duration, EIntervalType type);
      void addInterval(quint64 start, quint64 duration, quint64 period);
      void addInterval(const SautoModel &data);
      void removeFocusTime();
      void changeStartTime(int newVal);
      void changeStopTime(int newVal);
      void changeSingleshot(int newVal);
      void receiveIntervalType(EIntervalType type);

   private slots:
      void receivedCloseFreqSig();
      void receivedFreq(const SautoModel &data);

   signals:
      void timeChangeSig(qreal start, qreal stop);
      void setMode(EIntervalType type);

   private: 
      void drawFrame();
      void drawInterval(const TimeInterval &interval, QPainter &painter);
      void drawSingleShot(const TimeInterval &interval, QPainter &painter);
      QRectF getBar(const TimeInterval &interval);
      QPolygonF getPoly(const QPointF &pt, qreal halfDist = 5.0);
      void resizeBar(const QPointF &pos);
      void calcTime();
      qreal timeToPos(const QTime &time) const;

   private: 
      bool mouseIsPressed;
      bool isResizingBar;
      bool isMovingBar;
      bool barMoveInitOK;
      EIntervalType m_intervalType;
      QMenu *freqMenu;
      CWaveletWidget *freqWidget;
      QPair<int, bool> resizeIndex_trueStart;
      QRectF viewRect;
      QSizeF viewSize;
      qreal secsPerPixel;
      qreal m_lastPos;
      QPixmap *m_frame;
      QHash<int, TimeInterval> m_intervals;
   };


   class CTimeIntervalWidget : public QDialog
   {
      Q_OBJECT

   public:
      explicit CTimeIntervalWidget(QWidget *parent = 0);
      ~CTimeIntervalWidget();
      void init();
      void setData(const INTERVAL_LIST &intervals);
      void cancel();
      void updateWidgetContent();

   public slots:
      void setDayOfWeek(DAYS dayOfWeek);

   private slots:
      void addClicked();
      void removeClicked();
      void OKClicked();
      void cancelClicked();
      void startEditChanged(const QTime &time);
      void stopEditChanged (const QTime &time);
      void timeChange(qreal start, qreal stop);
      void radioButtonClicked(int id);
      void setMode(EIntervalType type);

   signals:
      void addSingleShot(const QTime &time);
      void addInterval(quint64 start, quint64 duration, EIntervalType type);
      void removeFocusTime();
      void changeStartTime  (int newVal );
      void changeStopTime   (int newVal );
      void changeSingleshot (int newVal );
      void closeMe();
      void timeIntervals(const INTERVAL_LIST &intervals);
      void timeIntervals(const INTERVAL_LIST &intervals, DAYS weekDayID);
      void setIntervalType(EIntervalType type);
      void cancelled();

   private: 
      void initGUI();
      void estimateNewInterval(QTime &start, QTime &stop);
      void estimateSingleshot(QTime &time);

   private: 
      EIntervalType m_intervalType;
      DAYS m_weekDay;
      INTERVAL_LIST m_data;
      QPushButton *m_addButton;
      QPushButton *m_removeButton;
      QPushButton *m_okButton;
      QPushButton *m_cancelButton;
      QRadioButton *m_makeIntervalRadio;
      QRadioButton *m_makeSingleshotRadio;
      QTimeEdit *m_startEdit;
      QTimeEdit *m_stopEdit;
      QTime m_singleShotTime;
      QTime m_start;
      QTime m_stop ;
      CTimeLineView *m_timeLine;
      QHash<int, TimeInterval> m_arrows;
   };
}
#endif