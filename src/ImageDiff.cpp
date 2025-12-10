// SPDX-License-Identifier: GPL-2.0
#include "ImageDiff.h"

#include <algorithm>
#include <cmath>

static inline int pixelDiffMagnitude(const QColor &ca, const QColor &cb)
{
    int dr = ca.red() - cb.red();
    int dg = ca.green() - cb.green();
    int db = ca.blue() - cb.blue();
    // use L2 norm approximation -> magnitude in [0..~441], we map later to 0..255
    double mag = std::sqrt(double(dr * dr + dg * dg + db * db));
    return int(mag + 0.5);
}

DiffResult ImageDiff::compute(const QImage &a_in, const QImage &b_in, int threshold, double sensitivity)
{
    DiffResult res;
    if (a_in.isNull() || b_in.isNull()) return res;

    // Choose canvas size = max(width,height)
    int w = std::max(a_in.width(), b_in.width());
    int h = std::max(a_in.height(), b_in.height());

    QImage ai =
        a_in.convertToFormat(QImage::Format_RGBA8888).scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QImage bi =
        b_in.convertToFormat(QImage::Format_RGBA8888).scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    res.diffImage = QImage(w, h, QImage::Format_RGBA8888);
    res.diffMask = QImage(w, h, QImage::Format_Grayscale8);
    res.differingPixels = 0;
    res.maxDiff = 0.0;

    for (int y = 0; y < h; ++y) {
        const uint8_t *aLine = ai.constScanLine(y);
        const uint8_t *bLine = bi.constScanLine(y);
        uint8_t *dLine = res.diffImage.scanLine(y);
        uint8_t *mLine = res.diffMask.scanLine(y);
        for (int x = 0; x < w; ++x) {
            QColor ca = ai.pixelColor(x, y);
            QColor cb = bi.pixelColor(x, y);

            int mag = pixelDiffMagnitude(ca, cb);
            double magScaled = mag * sensitivity;
            if (magScaled > res.maxDiff) res.maxDiff = magScaled;

            int m8 = int(std::min(255.0, magScaled / std::sqrt(3.0 * 255.0 * 255.0) * 255.0));  // normalized-ish
            // decide if different
            bool diff = (magScaled > threshold);

            // Build diff image: if diff -> red highlight, else blended greyscale of original
            QColor out;
            if (diff) {
                // red overlay with alpha proportional to difference
                int alpha = std::min(220, 80 + int(magScaled));  // visibility
                out = QColor(255, 0, 0, alpha);
            } else {
                // subtle translucent copy of a
                out = ca;
            }

            // set pixel in diffImage
            res.diffImage.setPixelColor(x, y, out);
            // set mask (difference magnitude)
            mLine[x] = static_cast<uint8_t>(m8);

            if (diff) ++res.differingPixels;
        }
    }

    return res;
}
