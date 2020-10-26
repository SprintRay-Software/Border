#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "border.h"
#include "BorderFinder.h"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MyMesh;
MyMesh myMesh;

void Border::startFind(QString inputPath)
{
	//input_path = input_path.remove(0, 8);
    BorderFinder sqr(inputPath);
    QFileInfo fileInfo(inputPath);
	QString fileName = fileInfo.baseName();
	sqr.drawBorderPointsToImage(QCoreApplication::applicationDirPath() + OUTPUT_IMG + fileName + ".png");
    sqr.writeBorderPointsToFile(QCoreApplication::applicationDirPath() + OUTPUT_FILE + fileName + ".txt", offX, offY);
	//sqr.drawBorderPointsToImage(OUTPUT_IMG + fileName + ".png");
	//sqr.writeBorderPointsToFile(OUTPUT_FILE + fileName + ".txt", x_off, y_off);
}

void Border::cloneModelAndProcess(string inputPath)
{
    if (!OpenMesh::IO::read_mesh(myMesh, inputPath))
	{
		std::cout << "Failed to read mesh!" << std::endl;
		return;
	}
    offX = 0, offY = 0;
    calcOff(offX, offY);
	QDir dir(QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH);
    QString qinputPath = QString::fromStdString(inputPath);
    QFileInfo fileInfo(qinputPath);
	QString fileName = fileInfo.baseName();
	bool exist = dir.exists(fileName);
	if (!exist)
	{
		dir.mkdir(fileName);
	}
    QString generatePath = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::CLONE_STL_PATH + "/" + fileName + "/" + fileName + ".stl";
    if (!OpenMesh::IO::write_mesh(myMesh, generatePath.toStdString(), OpenMesh::IO::Options::Binary))
	{
		std::cerr << "Cannot write mesh to file ' .stl ' " << std::endl;
		return;
	}
    startFind(generatePath);

	QString path = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::CLONE_STL_PATH + "/" + fileName;
	dir = QDir(path);
	foreach(QFileInfo mfi, dir.entryInfoList())
	{
		if (mfi.isFile() && (mfi.suffix() == "stl")) 
		{
			dir.remove(mfi.fileName());
		}
	}
}

void Border::calcOff(int &offX, int &offY)
{
    double minX = myMesh.point(myMesh.vertices_begin()).data()[0], maxX = myMesh.point(myMesh.vertices_begin()).data()[0],
        minY = myMesh.point(myMesh.vertices_begin()).data()[1], maxY = myMesh.point(myMesh.vertices_begin()).data()[1];
	for (auto vec_it = myMesh.vertices_begin(); vec_it != myMesh.vertices_end(); vec_it++)
	{
        if (myMesh.point(vec_it).data()[0] <= minX)
		{
            minX = myMesh.point(vec_it).data()[0];
		}
        if (myMesh.point(vec_it).data()[0] >= maxX)
		{
            maxX = myMesh.point(vec_it).data()[0];
		}
        if (myMesh.point(vec_it).data()[1] <= minY)
		{
            minY = myMesh.point(vec_it).data()[1];
		}
        if (myMesh.point(vec_it).data()[1] >= maxY)
		{
            maxY = myMesh.point(vec_it).data()[1];
		}
	}
    if (minX < 0)
	{
        offX = -((int)minX - 1);
	}
    if (maxX > 182.4)
	{
        offX = -(int)(maxX - 182.4 + 1);
	}
    if (minY < 0)
	{
        offY = -((int)minY - 1);
	}
    if (maxY > 102.6)
	{
        offY = -(int)(maxY - 102.6 + 1);
	}
    if (offX != 0 || offY != 0)
	{
		for (auto vec_it = myMesh.vertices_begin(); vec_it != myMesh.vertices_end(); vec_it++)
		{
            myMesh.point(vec_it).data()[0] = myMesh.point(vec_it).data()[0] + offX;
            myMesh.point(vec_it).data()[1] = myMesh.point(vec_it).data()[1] + offY;
		}
	}
	return;
}
