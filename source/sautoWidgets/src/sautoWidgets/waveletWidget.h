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
//  \file      waveletWidget.h
//
//  \brief     Definition of a widget to define a wavelet
//
//  \author    Stian Broen
//
//  \date      28.12.2012
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

#ifndef _WAVELET_WIDGET_H
#define _WAVELET_WIDGET_H

// Qt includes
#include <QWidget>
#include <QDialog>
#include <QPolygonF>

// solution includes
#include <sautoModel/sautoDefs.h>

// Qt forward declarations
class QDial;
class QLabel;
class QGridLayout;
class QVBoxLayout;
class QPaintEvent;
class QPixmap;
class QResizeEvent;
class QToolButton;
class QPushButton;
class QGroupBox;
class QCheckBox;
class QAbstractButton;
class QButtonGroup;
class QTimeEdit;
class QComboBox;

namespace sauto {
   class CWaveView : public QWidget
   {
      Q_OBJECT

   public:
      explicit CWaveView(QWidget *parent = 0);
      ~CWaveView();
      void setData(const SautoModel &data);

   signals:
      void updateParentGui();

   protected:
      void paintEvent(QPaintEvent *);
      void resizeEvent(QResizeEvent *);

   private:
      void recalcWindow();
      void redraw();
      void drawFrame();
      void drawClockGrid();
      void drawGrid();
      void drawWave();
      void drawStaticIntervals();
      void drawSingleShot();
      void drawDerivates();
      void drawSquareAroundPoints(const QPolygonF &points);
      void averagePointFValues(QPolygonF &points);

   public slots:
      void receiveFrequency_seconds(int val);
      void receiveFrequency_minutes(int val);
      void receiveFrequency_hours(int val);
      void receivePhase(int val);
      void peakStateChanged(bool val);
      void valleyStateChanged(bool val);
      void risingStateChanged(bool val);
      void sinkingStateChanged(bool val);
      void changeMode(EIntervalType mode);

   private:
      EIntervalType m_type;
      int m_period;
      int m_period_s;
      int m_period_m;
      int m_period_h;
      qreal m_phase;
      qreal m_waveLimitSeconds;
      bool m_togglePeaks;
      bool m_toggleValleys;
      bool m_toggleRising;
      bool m_toggleSinking;
      QPixmap *m_frame;
      QPolygonF m_wave;
      QPolygonF m_peaks;
      QPolygonF m_valleys;
      QPolygonF m_risings;
      QPolygonF m_sinkings;
      QRectF m_viewRect;
      QSizeF m_viewSize;
   };


   class CWaveletWidget : public QDialog
   {
      Q_OBJECT

   public:
      explicit CWaveletWidget(QWidget *parent = 0, bool allowCustomDuration = false);
      ~CWaveletWidget();
      void setData(const SautoModel &data);
      SautoModel getData();
      void updateWidgetContent();

   signals:
      void dataSignal(const SautoModel &data);
      void closeMe();
      void changeMode(EIntervalType mode);
      void isSingleInterval(bool val);
      void cancelled();

   private slots:
      void enableCustomDuration(int state);
      void customDurationChanged(const QTime &time);
      void typeButtonClicked(QAbstractButton *button);
      void OKClicked();
      void cancelClicked();
      void updateWidgets();

   private:
      void initGui();
      bool evaluateTaskFile(const QString &fileName);
      void updateData();
      void connectWave();
      void disconnectWave();

   private:
      bool m_customDurationAllowed;
      QTimeEdit *m_durationEdit;
      QCheckBox *m_enableDuration;
      QButtonGroup *m_typeGroup;
      EIntervalType m_type;
      QPushButton *m_OK;
      QPushButton *m_cancel;
      CWaveView *m_wave;
      QDial *m_freq_seconds;
      QDial *m_freq_minutes;
      QDial *m_freq_hours;
      QDial *m_phase;
      QComboBox *m_onPeakTaskBox;
      QComboBox *m_onValleyTaskBox;
      QComboBox *m_onRisingTaskBox;
      QComboBox *m_onSinkingTaskBox;
      QCheckBox *m_peakBox;
      QCheckBox *m_valleyBox;
      QCheckBox *m_risingBox;
      QCheckBox *m_sinkingBox;
      QPushButton *m_checkType_wave;
      QPushButton *m_checkType_static;
      QPushButton *m_checkType_single;
      SautoModel m_data;
   };

   QPolygonF makeSineInFrame(
      const QSize &frame,
      const qreal secWindow,
      const qreal secPeriod,
      const qreal phase,
      const qreal ampl_shrinkage = 100.0
      );

   void fillComboBox(
      QComboBox *box, 
      const QString &path, 
      const QString &suffix = "", 
      const QString &emptyFiller = "", 
      bool folders = false, 
      bool doClear = true
      );
}

#endif