// SPDX-License-Identifier: GPL-2.0
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>

#include "ImageCompareWidget.h"
#include "MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("ImageDiff Reviewer Tool");
    parser.addHelpOption();

    QCommandLineOption optReview("review", "Review mode (GUI waits for user exit code)");
    QCommandLineOption optLeft("left", "Left image", "file");
    QCommandLineOption optRight("right", "Right image", "file");

    parser.addOption(optReview);
    parser.addOption(optLeft);
    parser.addOption(optRight);

    parser.process(app);

    MainWindow w;
    ImageCompareWidget* cmp = w.findChild<ImageCompareWidget*>();

    // --- REVIEW MODE ------------------------------------------------------
    if (parser.isSet(optReview)) {
        QString left = parser.value(optLeft);
        QString right = parser.value(optRight);

        if (left.isEmpty() || right.isEmpty()) {
            qCritical("In review mode, --left and --right are required.");
            return 4;
        }
        qDebug() << "Review mode enabled. Starting with images:" << left << "and" << right;

        cmp->loadLeft(left);
        cmp->loadRight(right);

        // MainWindow anzeigen
        w.show();

        // App läuft – MainWindow muss ein Signal liefern, wenn ein Exit-Button gedrückt wurde
        int r = app.exec();

        // Das Window setzt z.B. exit code via QApplication::exit(code)
        return r;
    }

    // --- NORMAL STARTUP (two positional args allowed) ----------------------
    if (parser.positionalArguments().size() >= 2) {
        const QStringList args = parser.positionalArguments();
        cmp->loadLeft(args[0]);
        cmp->loadRight(args[1]);
    }

    w.show();
    return app.exec();
}
