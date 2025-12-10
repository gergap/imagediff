// SPDX-License-Identifier: GPL-2.0
#include "ImageCompareWidget.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QImageReader>
#include <QPainter>
#include <QPixmap>
#include <QScrollBar>
#include <QVBoxLayout>

static QImage loadImageAuto(const QString &path)
{
    QImageReader r(path);
    r.setAutoTransform(true);
    QImage img = r.read();
    return img;
}

ImageCompareWidget::ImageCompareWidget(QWidget *parent) : QWidget(parent)
{
    m_leftFilenameLabel = new QLabel("<left>");
    m_rightFilenameLabel = new QLabel("<right>");

    m_leftFilenameLabel->setStyleSheet("font-weight: bold;");
    m_rightFilenameLabel->setStyleSheet("font-weight: bold;");

    m_leftLabel = makeImageLabel();
    m_rightLabel = makeImageLabel();
    m_diffLabel = makeImageLabel();

    m_leftScroll = new QScrollArea;
    m_rightScroll = new QScrollArea;
    m_diffScroll = new QScrollArea;

    m_leftScroll->setWidget(m_leftLabel);
    m_rightScroll->setWidget(m_rightLabel);
    m_diffScroll->setWidget(m_diffLabel);

    // Left column = filename + scrollarea
    QWidget *leftBox = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftBox);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->addWidget(m_leftFilenameLabel);
    leftLayout->addWidget(m_leftScroll);

    // Right column = filename + scrollarea
    QWidget *rightBox = new QWidget;
    QVBoxLayout *rightLayout = new QVBoxLayout(rightBox);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->addWidget(m_rightFilenameLabel);
    rightLayout->addWidget(m_rightScroll);

    // Top splitter: left vs right
    m_splitter = new QSplitter(Qt::Horizontal);
    m_splitter->addWidget(leftBox);
    m_splitter->addWidget(rightBox);

    // Bottom: diff
    QSplitter *s = new QSplitter(Qt::Vertical);
    s->addWidget(m_splitter);
    s->addWidget(m_diffScroll);

    QVBoxLayout *v = new QVBoxLayout();
    v->addWidget(s);
    setLayout(v);

    // Keep scrolling in sync
    connect(m_leftScroll->verticalScrollBar(), &QScrollBar::valueChanged, m_rightScroll->verticalScrollBar(),
            &QScrollBar::setValue);
    connect(m_rightScroll->verticalScrollBar(), &QScrollBar::valueChanged, m_leftScroll->verticalScrollBar(),
            &QScrollBar::setValue);

    connect(m_leftScroll->horizontalScrollBar(), &QScrollBar::valueChanged, m_rightScroll->horizontalScrollBar(),
            &QScrollBar::setValue);
    connect(m_rightScroll->horizontalScrollBar(), &QScrollBar::valueChanged, m_leftScroll->horizontalScrollBar(),
            &QScrollBar::setValue);
}

QLabel *ImageCompareWidget::makeImageLabel()
{
    QLabel *l = new QLabel;
    l->setBackgroundRole(QPalette::Base);
    l->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    l->setScaledContents(true);
    l->setAlignment(Qt::AlignCenter);
    return l;
}

static QImage makeErrorImage(const QString &text)
{
    const int W = 400;
    const int H = 200;

    QImage img(W, H, QImage::Format_ARGB32);
    img.fill(Qt::white);

    QPainter p(&img);
    p.fillRect(0, 0, W, H, QColor(255, 200, 200));  // light red background
    p.setPen(Qt::red);
    p.setFont(QFont("Sans", 20, QFont::Bold));
    p.drawText(img.rect(), Qt::AlignCenter, text);
    return img;
}

bool ImageCompareWidget::loadLeft(const QString &path)
{
    m_leftFilenameLabel->setText(path);
    QImage img = loadImageAuto(path);

    if (img.isNull()) {
        m_left = makeErrorImage("Could not load:\n" + path);
        updateViews();
        return false;  // Do not recompute diff
    }

    m_left = img;
    updateViews();
    recomputeDiff();
    return true;
}

bool ImageCompareWidget::loadRight(const QString &path)
{
    m_rightFilenameLabel->setText(path);
    QImage img = loadImageAuto(path);

    if (img.isNull()) {
        m_right = makeErrorImage("Could not load:\n" + path);
        updateViews();
        return false;  // Do not recompute diff
    }

    m_right = img;
    updateViews();
    recomputeDiff();
    return true;
}

void ImageCompareWidget::updateViews()
{
    if (!m_left.isNull()) {
        m_leftLabel->setPixmap(QPixmap::fromImage(m_left));
        m_leftLabel->adjustSize();
    } else {
        m_leftLabel->clear();
    }
    if (!m_right.isNull()) {
        m_rightLabel->setPixmap(QPixmap::fromImage(m_right));
        m_rightLabel->adjustSize();
    } else {
        m_rightLabel->clear();
    }
    if (!m_diffResult.diffImage.isNull()) {
        m_diffLabel->setPixmap(QPixmap::fromImage(m_diffResult.diffImage));
        m_diffLabel->adjustSize();
    }
}

void ImageCompareWidget::recomputeDiff()
{
    if (m_left.isNull() || m_right.isNull()) return;
    m_diffResult = ImageDiff::compute(m_left, m_right, m_threshold, m_sensitivity);
    updateViews();
    emit diffUpdated(m_diffResult);
}
