// SPDX-License-Identifier: GPL-2.0
#include "MainWindow.h"

#include <QAction>
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QStatusBar>
#include <QToolBar>

#include "ImageCompareWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // --- Central widget: wrap your ImageCompareWidget so we can add the button bar below it
    QWidget *central = new QWidget(this);
    QVBoxLayout *centralLayout = new QVBoxLayout;
    central->setLayout(centralLayout);

    m_cmp = new ImageCompareWidget;
    centralLayout->addWidget(m_cmp);

    // --- Button bar for review exit-codes (Accept/Reject/Skip/Abort)
    auto *btnAccept = new QPushButton("Accept");
    auto *btnReject = new QPushButton("Reject");
    auto *btnSkip = new QPushButton("Skip");
    auto *btnAbort = new QPushButton("Abort");

    btnAccept->setStyleSheet("background:#4caf50; color:white;");
    btnReject->setStyleSheet("background:#f44336; color:white;");
    btnSkip->setStyleSheet("background:#9e9e9e; color:black;");
    btnAbort->setStyleSheet("background:#ff9800; color:black;");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnAccept);
    buttonLayout->addWidget(btnReject);
    buttonLayout->addWidget(btnSkip);
    buttonLayout->addWidget(btnAbort);
    buttonLayout->addStretch();

    centralLayout->addLayout(buttonLayout);

    setCentralWidget(central);

    // --- Menus / Actions (unverändert)
    createActions();

    // --- Status bar
    m_status = new QLabel;
    statusBar()->addWidget(m_status, 1);

    // Control dock (Threshold / Sensitivity) - unverändert
    QWidget *controls = new QWidget;
    QFormLayout *fl = new QFormLayout;

    m_thresholdSlider = new QSlider(Qt::Horizontal);
    m_thresholdSlider->setRange(0, 255);
    m_thresholdSlider->setValue(10);
    connect(m_thresholdSlider, &QSlider::valueChanged, this, &MainWindow::onThresholdChanged);
    fl->addRow("Threshold", m_thresholdSlider);

    m_sensitivitySlider = new QSlider(Qt::Horizontal);
    m_sensitivitySlider->setRange(1, 500);  // map to 0.01..5.00
    m_sensitivitySlider->setValue(100);
    connect(m_sensitivitySlider, &QSlider::valueChanged, this, &MainWindow::onSensitivityChanged);
    fl->addRow("Sensitivity (%)", m_sensitivitySlider);

    controls->setLayout(fl);
    QDockWidget *dock = new QDockWidget("Controls", this);
    dock->setWidget(controls);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    // --- Connect diff status update to statusbar label (unverändert)
    connect(m_cmp, &ImageCompareWidget::diffUpdated, this, [this](const DiffResult &r) {
        m_status->setText(
            QString("Differing pixels: %1, maxDiff: %2").arg(r.differingPixels).arg(r.maxDiff, 0, 'f', 1));
    });

    // --- Connect review buttons to exit-code slots
    connect(btnAccept, &QPushButton::clicked, this, &MainWindow::onAccept);
    connect(btnReject, &QPushButton::clicked, this, &MainWindow::onReject);
    connect(btnSkip, &QPushButton::clicked, this, &MainWindow::onSkip);
    connect(btnAbort, &QPushButton::clicked, this, &MainWindow::onAbort);

    // --- Shortcuts for review actions
    new QShortcut(QKeySequence(Qt::Key_Return), this, SLOT(onAccept()));
    new QShortcut(QKeySequence(Qt::Key_Enter), this, SLOT(onAccept()));
    new QShortcut(QKeySequence(Qt::Key_A), this, SLOT(onAccept()));
    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(onAbort()));
    new QShortcut(QKeySequence(Qt::Key_R), this, SLOT(onReject()));
    new QShortcut(QKeySequence(Qt::Key_S), this, SLOT(onSkip()));

    setWindowTitle("ImageDiff");
    resize(1200, 800);
}

void MainWindow::createActions()
{
    QMenu *file = menuBar()->addMenu("&File");

    QAction *openL = new QAction("Open &Left...", this);
    connect(openL, &QAction::triggered, this, &MainWindow::openLeft);
    file->addAction(openL);

    QAction *openR = new QAction("Open &Right...", this);
    connect(openR, &QAction::triggered, this, &MainWindow::openRight);
    file->addAction(openR);

    QAction *saveD = new QAction("&Save Diff...", this);
    connect(saveD, &QAction::triggered, this, &MainWindow::saveDiff);
    file->addAction(saveD);

    file->addSeparator();
    QAction *quit = new QAction("&Quit", this);
    connect(quit, &QAction::triggered, this, &QWidget::close);
    file->addAction(quit);
}

void MainWindow::openLeft()
{
    QString fn = QFileDialog::getOpenFileName(this, "Open left image", QString(), "Images (*.png *.jpg *.bmp)");
    if (fn.isEmpty()) return;
    if (!m_cmp->loadLeft(fn)) statusBar()->showMessage("Failed to load left image", 3000);
}

void MainWindow::openRight()
{
    QString fn = QFileDialog::getOpenFileName(this, "Open right image", QString(), "Images (*.png *.jpg *.bmp)");
    if (fn.isEmpty()) return;
    if (!m_cmp->loadRight(fn)) statusBar()->showMessage("Failed to load right image", 3000);
}

void MainWindow::saveDiff()
{
    QString fn = QFileDialog::getSaveFileName(this, "Save diff image", QString(), "PNG Image (*.png)");
    if (fn.isEmpty()) return;
    QImage diff = m_cmp->diffImage();
    if (diff.isNull()) {
        statusBar()->showMessage("No diff to save", 3000);
        return;
    }
    if (!diff.save(fn))
        statusBar()->showMessage("Failed to save diff", 3000);
    else
        statusBar()->showMessage("Diff saved", 2000);
}

void MainWindow::onThresholdChanged(int v)
{
    m_cmp->setThreshold(v);
    statusBar()->showMessage(QString("Threshold %1").arg(v), 1000);
}

void MainWindow::onSensitivityChanged(int v)
{
    double sens = v / 100.0;
    m_cmp->setSensitivity(sens);
    statusBar()->showMessage(QString("Sensitivity %1%").arg(v), 1000);
}

// ---- Exit Codes ----
void MainWindow::onAccept()
{
    QApplication::exit(0);
}
void MainWindow::onReject()
{
    QApplication::exit(1);
}
void MainWindow::onSkip()
{
    QApplication::exit(2);
}
void MainWindow::onAbort()
{
    QApplication::exit(3);
}
