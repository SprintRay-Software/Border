#pragma once

#include <QStandardPaths>
#include <qdir.h>
#include <QCoreApplication>
#include <iostream> 
#include "border_global.h"

using namespace std;

// UPDATE WHERE FILES SHOULD GO
const QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
// the input png, stl or obj file
const QString INPUT_PATH = QCoreApplication::applicationDirPath() + "/Borderfinder/lib/slicer/teeth3.stl";
// where the points should go
const QString OUTPUT_FILE = QCoreApplication::applicationDirPath() + "/Borderfinder/";
// where the image should go
const QString OUTPUT_IMG = QCoreApplication::applicationDirPath() + "/Borderfinder/";

class BORDERSHARED_EXPORT Border
{
public:
    void startFind(QString inputPath);
    void cloneModelAndProcess(string inputPath);
    void calcOff(int &offX, int &offY);

public:
    int offX;
    int offY;
};
