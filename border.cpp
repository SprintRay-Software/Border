#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "border.h"
#include "BorderFinder.h"
#include "qDebug.h"
#include "QDateTime"
#include <QTextCodec>

typedef OpenMesh::PolyMesh_ArrayKernelT<> MyMesh;
MyMesh myMesh;

void Border::startFind(QString inputPath,double scale)
{
    BorderFinder sqr(inputPath,scale);
    QFileInfo fileInfo(inputPath);
	QString fileName = fileInfo.baseName();
	sqr.drawBorderPointsToImage(QCoreApplication::applicationDirPath() + OUTPUT_IMG + fileName + ".png");
    sqr.writeBorderPointsToFile(QCoreApplication::applicationDirPath() + OUTPUT_FILE + fileName + ".txt", offX, offY);
}

bool Border::cloneModelAndProcess(string inputPath,double scale)
{
    if (!OpenMesh::IO::read_mesh(myMesh, string(QString::fromStdString(inputPath).toLocal8Bit())))
	{
		std::cout << "Failed to read mesh!" << std::endl;
        return false;
	}
    offX = 0, offY = 0;
    bool isNormal = calcOff(offX, offY, scale);
    if(!isNormal)
    {
        return isNormal;
    }
    QDateTime time3 = QDateTime::currentDateTime();;
    if(offX != 0 || offY != 0)
    {
        QTextCodec *code = QTextCodec::codecForName("GBK");
        string str = code->fromUnicode(QString::fromStdString(inputPath)).data();
        QString qinputPath = QString(QString::fromLocal8Bit(str.c_str()));
        QDir dir(QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH);
        QFileInfo fileInfo(qinputPath);
        QString fileName = fileInfo.baseName();
        bool exist = dir.exists(fileName);
        if (!exist)
        {
            dir.mkdir(fileName);
        }
        QString generatePath = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::CLONE_STL_PATH + "/" + fileName + "/" + fileName + ".stl";
        auto localPath = generatePath.toLocal8Bit();
        if (!OpenMesh::IO::write_mesh(myMesh, string(localPath), OpenMesh::IO::Options::Binary))
        {
            std::cerr << "Cannot write mesh to file ' .stl ' " << std::endl;
            return false;
        }
        startFind(generatePath, scale);
        QString path = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::CLONE_STL_PATH + "/" + fileName;
        dir = QDir(path.toLocal8Bit());
        foreach(QFileInfo mfi, dir.entryInfoList())
        {
            if (mfi.isFile() && (mfi.suffix() == "stl1"))
            {
                dir.remove(mfi.fileName().toLocal8Bit());
            }
        }
        QDateTime time4 = QDateTime::currentDateTime();;
    }
    else
    {
        startFind(QString::fromStdString(inputPath), scale);
    }
    return isNormal;
}
bool Border::calcOff(int &offX, int &offY, double scale)
{
    if(myMesh.vertices_empty())
    {

        return false;
    }
    float minX = myMesh.point(myMesh.vertices_begin()).data()[0], maxX = myMesh.point(myMesh.vertices_begin()).data()[0],
        minY = myMesh.point(myMesh.vertices_begin()).data()[1], maxY = myMesh.point(myMesh.vertices_begin()).data()[1];
	for (auto vec_it = myMesh.vertices_begin(); vec_it != myMesh.vertices_end(); vec_it++)
	{
        if (myMesh.point(*vec_it).data()[0] <= minX)
		{
            minX = myMesh.point(*vec_it).data()[0];
		}
        if (myMesh.point(*vec_it).data()[0] >= maxX)
		{
            maxX = myMesh.point(*vec_it).data()[0];
		}
        if (myMesh.point(*vec_it).data()[1] <= minY)
		{
            minY = myMesh.point(*vec_it).data()[1];
		}
        if (myMesh.point(*vec_it).data()[1] >= maxY)
		{
            maxY = myMesh.point(*vec_it).data()[1];
		}
	}
    double length = 1920 * scale;
    double width = 1080 * scale;
    if (minX < 0)
	{
        offX = -((int)minX - 15);
	}
    if (maxX > length)
	{
        offX = -(int)(maxX - length + 15);
	}
    if (minY < 0)
	{
        offY = -((int)minY - 15);
	}
    if (maxY > width)
	{
        offY = -(int)(maxY - width + 15);
	}
    std::cout << "offX = " <<offX<< std::endl;
    std::cout << "offY = " <<offY<< std::endl;
    if (offX != 0 || offY != 0)
	{
		for (auto vec_it = myMesh.vertices_begin(); vec_it != myMesh.vertices_end(); vec_it++)
		{
            myMesh.point(vec_it).data()[0] = myMesh.point(vec_it).data()[0] + offX;
            myMesh.point(vec_it).data()[1] = myMesh.point(vec_it).data()[1] + offY;
		}
	}
    return true;
}
