#include "slicerutils.h"

#include <iostream>
#include <QProcess>
#include <QImage>
#include <QString>
#include <fstream>


QString SlicerUtils::BuildTransformationMatrixString(const QMatrix4x4 &mat) {
	QString matrix_string;
	for (int e = 0; e < 16; e++) {
		if (e != 0) matrix_string += ",";
		matrix_string += QString::number(mat.data()[e]);
	}
	return matrix_string;
}

void SlicerUtils::Slice(const QString &stlFile) {
	return SliceChi(stlFile);
}

void SlicerUtils::SliceChi(const QString &stlFile) {
	qDebug() << getArgumentsForFile(stlFile);
	qDebug() << "SLICER_PATH:"<< getExecutablePath();
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);
	process.start(getExecutablePath(), getArgumentsForFile(stlFile));

	while (process.waitForReadyRead()) {
		std::string line = process.readAll().toStdString();
		std::cout << line << std::endl;

		// removed code regarding application output
	}
}

QStringList SlicerUtils::getArgumentsForFile(const QString &layersDir) {

	QStringList arguments;

	arguments << "-slice";
	arguments << "-p";
	arguments << "1920,1080,182.4,102.6,0.05,1";
	arguments << "-a";
	arguments << "1,1,1000,100,1,1";

	arguments << "-m";
	arguments << layersDir;

	arguments << BORDERFINDERCONSTANTS::DEFAULT_TRANSFORMATION_MATRIX;

	// define output directory (NOTE: THIS FOLDER MUST EXIST IN THE BUILD DIRECTORY!)
	arguments << "-f";
	//arguments << "outImg";
	QFileInfo fileInfo(layersDir);
	QString fileName = fileInfo.baseName();
	QDir dir(QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH);
	qDebug() << "BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH = " << dir.absolutePath();
	bool exist = dir.exists(fileName);
	qDebug() << "fileName = " << fileName;
	if (!exist)
	{
		qDebug() << "exist = " << exist;
		dir.mkdir(fileName);
	}
	QString generate_path = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH + "/" + fileName;
	qDebug() << generate_path;
	arguments << generate_path;
	//arguments << "D:/build-rayware-Desktop_Qt_5_11_2_MSVC2015_64bit-Release/release/Borderfinder/teeth1";
	//arguments << "C:/Program Files (x86)/RayWare/Borderfinder/teeth1";
	//arguments << "D:/ÈÎÎñ/Java/22";

	//QFileInfo fileInfo(layersDir);
	//QString fileName = fileInfo.baseName();
	//arguments << BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH;

	// removed support code for now
	qDebug() << arguments;
	qDebug() << "QCoreApplication::applicationDirPath():" << QCoreApplication::applicationDirPath();

	return arguments;
}

QString SlicerUtils::getExecutablePath() {
	qDebug() << "BORDERFINDERCONSTANTS::SLICER_PATH:" << QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SLICER_PATH;
	return QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SLICER_PATH;
	//return "D:/build-rayware-Desktop_Qt_5_11_2_MSVC2015_64bit-Release/release/Borderfinder/Command Line/DLPSlicerSingleWin.exe";
}

