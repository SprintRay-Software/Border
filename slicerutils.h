#pragma once
#ifndef SLICER_UTILS_H
#define SLICER_UTILS_H

#include <QString>
#include <QImage>
#include <QMatrix4x4>

#include "borderfinderConstants.h"

namespace SlicerUtils {

	QString BuildTransformationMatrixString(const QMatrix4x4 &mat);

    QStringList getArgumentsForFile(const QString &layersDir, double scale);

    void SliceChi(const QString &layersDir, double scale);

    void Slice(const QString &layersDir, double scale);

	QString getExecutablePath();

};

#endif // SLICER_UTILS_H


