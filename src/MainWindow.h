// SPDX-License-Identifier: GPL-2.0
#pragma once
#include <QMainWindow>

class ImageCompareWidget;
class QLabel;
class QSlider;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

    void enableReview();
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
    QPushButton *m_btnAccept;
    QPushButton *m_btnReject;
    QPushButton *m_btnSkip;
    QPushButton *m_btnAbort;

    void createActions();
};
