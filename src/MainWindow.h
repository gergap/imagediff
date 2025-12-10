// SPDX-License-Identifier: GPL-2.0
#pragma once
#include <QMainWindow>

class ImageCompareWidget;
class QLabel;
class QSlider;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void openLeft();
    void openRight();
    void saveDiff();
    void onThresholdChanged(int v);
    void onSensitivityChanged(int v);
    void onAccept();
    void onReject();
    void onSkip();
    void onAbort();

private:
    ImageCompareWidget *m_cmp;
    QLabel *m_status;
    QSlider *m_thresholdSlider;
    QSlider *m_sensitivitySlider;

    void createActions();
};
