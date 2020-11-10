#include "slicerutils.h"

#include <iostream>
#include <QProcess>
#include <QImage>
#include <QString>
#include <fstream>
#include <QTextCodec>
#include <string>
using namespace std;


QString SlicerUtils::BuildTransformationMatrixString(const QMatrix4x4 &mat) {
	QString matrix_string;
	for (int e = 0; e < 16; e++) {
		if (e != 0) matrix_string += ",";
		matrix_string += QString::number(mat.data()[e]);
	}
	return matrix_string;
}

void SlicerUtils::Slice(const QString &stlFile, double scale) {
    return SliceChi(stlFile, scale);
}

void SlicerUtils::SliceChi(const QString &stlFile, double scale) {
	QProcess process;
	process.setProcessChannelMode(QProcess::MergedChannels);
    QTextCodec *code = QTextCodec::codecForName("GBK");
    string str = code->fromUnicode(getExecutablePath()).data();
    QString strDir = QString(QString::fromLocal8Bit(str.c_str()));
    process.start(strDir, getArgumentsForFile(stlFile, scale));

	while (process.waitForReadyRead()) {
		std::string line = process.readAll().toStdString();
		std::cout << line << std::endl;

		// removed code regarding application output
	}
}

QStringList SlicerUtils::getArgumentsForFile(const QString &layersDir, double scale) {

	QStringList arguments;
    QTextCodec *code = QTextCodec::codecForName("GBK");
	arguments << "-slice";
	arguments << "-p";
    QString para = "1920,1080,";
    double w = 1920 * scale;
    double h = 1080 * scale;
    para = para + QString::number(w) + "," + QString::number(h) + ",0.05,1";
    arguments << para;
	arguments << "-a";
	arguments << "1,1,1000,100,1,1";

	arguments << "-m";

    string str = code->fromUnicode(layersDir).data();
    QString strDir = QString(QString::fromLocal8Bit(str.c_str()));
    arguments << strDir;

	arguments << BORDERFINDERCONSTANTS::DEFAULT_TRANSFORMATION_MATRIX;

	// define output directory (NOTE: THIS FOLDER MUST EXIST IN THE BUILD DIRECTORY!)
	arguments << "-f";
	QFileInfo fileInfo(layersDir);
	QString fileName = fileInfo.baseName();
	QDir dir(QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH);
	bool exist = dir.exists(fileName);
	if (!exist)
	{
		dir.mkdir(fileName);
	}
	QString generate_path = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH + "/" + fileName;
    qDebug() << generate_path.toLocal8Bit();

    str = code->fromUnicode(generate_path).data();
    strDir = QString(QString::fromLocal8Bit(str.c_str()));
    arguments << strDir;
	// removed support code for now
    return arguments;
}

QString SlicerUtils::getExecutablePath() {
    return QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SLICER_PATH;
}

