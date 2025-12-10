// SPDX-License-Identifier: GPL-2.0
#pragma once
#include <QColor>
#include <QImage>

struct DiffResult {
    QImage diffImage;  // visual diff (highlighted)
    QImage diffMask;   // grayscale mask with difference magnitude (0..255)
    int differingPixels = 0;
    double maxDiff = 0.0;
};

class ImageDiff
{
public:
    // threshold: 0..255, any per-channel abs diff > threshold is counted as difference
    // sensitivity: multiply per-channel difference by this factor before thresholding (useful to amplify small changes)
    static DiffResult compute(const QImage &a, const QImage &b, int threshold = 10, double sensitivity = 1.0);
};
