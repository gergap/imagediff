// SPDX-License-Identifier: GPL-2.0
#pragma once
#include <QImage>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <QWidget>

#include "ImageDiff.h"

class ImageCompareWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageCompareWidget(QWidget *parent = nullptr);

    bool loadLeft(const QString &path);
    bool loadRight(const QString &path);

    void setThreshold(int t)
    {
        m_threshold = t;
        recomputeDiff();
    }
    void setSensitivity(double s)
    {
        m_sensitivity = s;
        recomputeDiff();
    }

    QImage diffImage() const { return m_diffResult.diffImage; }

signals:
    void diffUpdated(const DiffResult &r);

private slots:
    void recomputeDiff();

private:
    QImage m_left, m_right;
    QLabel *m_leftFilenameLabel;
    QLabel *m_rightFilenameLabel;
    DiffResult m_diffResult;
    int m_threshold = 10;
    double m_sensitivity = 1.0;

    QSplitter *m_splitter;
    QLabel *m_leftLabel;
    QLabel *m_rightLabel;
    QLabel *m_diffLabel;  // shows diff (toggleable)
    QScrollArea *m_leftScroll;
    QScrollArea *m_rightScroll;
    QScrollArea *m_diffScroll;

    void updateViews();
    QLabel *makeImageLabel();
};
