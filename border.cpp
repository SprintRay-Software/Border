#include <OpenMesh\Core\IO\MeshIO.hh>
#include <OpenMesh\Core\Mesh\PolyMesh_ArrayKernelT.hh>
#include "Border.h"
#include "borderfinder.h"
#include "borderfinderConstants.h"

typedef OpenMesh::PolyMesh_ArrayKernelT<> MyMesh;
MyMesh myMesh;

void Border::startFind(QString input_path)
{
	//input_path = input_path.remove(0, 8);
	BorderFinder sqr(input_path);
	QFileInfo fileInfo(input_path);
	QString fileName = fileInfo.baseName();
	sqr.drawBorderPointsToImage(QCoreApplication::applicationDirPath() + OUTPUT_IMG + fileName + ".png");
	sqr.writeBorderPointsToFile(QCoreApplication::applicationDirPath() + OUTPUT_FILE + fileName + ".txt", x_off, y_off);
	//sqr.drawBorderPointsToImage(OUTPUT_IMG + fileName + ".png");
	//sqr.writeBorderPointsToFile(OUTPUT_FILE + fileName + ".txt", x_off, y_off);
}

void Border::cloneModelAndProcess(string input_path)
{
	if (!OpenMesh::IO::read_mesh(myMesh, input_path))
	{
		std::cout << "Failed to read mesh!" << std::endl;
		return;
	}
	x_off = 0, y_off = 0;
	CalcOff(x_off, y_off);
	QDir dir(QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH);
	QString qinput_path = QString::fromStdString(input_path);
	QFileInfo fileInfo(qinput_path);
	QString fileName = fileInfo.baseName();
	bool exist = dir.exists(fileName);
	if (!exist)
	{
		dir.mkdir(fileName);
	}
	QString generate_path = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::CLONE_STL_PATH + "/" + fileName + "/" + fileName + ".stl";
	if (!OpenMesh::IO::write_mesh(myMesh, generate_path.toStdString(), OpenMesh::IO::Options::Binary))
	{
		std::cerr << "Cannot write mesh to file ' .stl ' " << std::endl;
		return;
	}
	std::cout << "startFind:" << generate_path.toStdString() << std::endl;
	startFind(generate_path);

	QString path = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::CLONE_STL_PATH + "/" + fileName;
	dir = QDir(path);
	std::cout << "dir:addr" << path.toStdString() << std::endl;
	foreach(QFileInfo mfi, dir.entryInfoList())
	{
		if (mfi.isFile() && (mfi.suffix() == "stl")) 
		{
			dir.remove(mfi.fileName());
		}
	}
	//QString temp_path = QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::CLONE_STL_PATH + "/" + fileName + ".stl";
	//startFind(temp_path);
}

void Border::CalcOff(int &x_off, int &y_off)
{
	//double min_x = myMesh.point[0].data()[0], max_x = myMesh.point[0].data()[0],
	//	min_y = myMesh.point[0].data()[1], max_y = myMesh.point[0].data()[1];
	double min_x = myMesh.point(myMesh.vertices_begin()).data()[0], max_x = myMesh.point(myMesh.vertices_begin()).data()[0],
		min_y = myMesh.point(myMesh.vertices_begin()).data()[1], max_y = myMesh.point(myMesh.vertices_begin()).data()[1];
	for (auto vec_it = myMesh.vertices_begin(); vec_it != myMesh.vertices_end(); vec_it++)
	{
		if (myMesh.point(vec_it).data()[0] <= min_x)
		{
			min_x = myMesh.point(vec_it).data()[0];
		}
		if (myMesh.point(vec_it).data()[0] >= max_x)
		{
			max_x = myMesh.point(vec_it).data()[0];
		}
		if (myMesh.point(vec_it).data()[1] <= min_y)
		{
			min_y = myMesh.point(vec_it).data()[1];
		}
		if (myMesh.point(vec_it).data()[1] >= max_y)
		{
			max_y = myMesh.point(vec_it).data()[1];
		}
	}
	std::cout << "min_x:" << min_x << "max_x:" << max_x << std::endl;
	std::cout << "min_y:" << min_y << "max_y:" << max_y << std::endl;
	if (min_x < 0)
	{
		x_off = -((int)min_x - 1);
	}
	if (max_x > 182.4)
	{
		x_off = -(int)(max_x - 182.4 + 1);
	}
	if (min_y < 0)
	{
		y_off = -((int)min_y - 1);
	}
	if (max_y > 102.6)
	{
		y_off = -(int)(max_y - 102.6 + 1);
	}
	if (x_off != 0 || y_off != 0)
	{
		for (auto vec_it = myMesh.vertices_begin(); vec_it != myMesh.vertices_end(); vec_it++)
		{
			myMesh.point(vec_it).data()[0] = myMesh.point(vec_it).data()[0] + x_off;
			myMesh.point(vec_it).data()[1] = myMesh.point(vec_it).data()[1] + y_off;
		}
		std::cout << "x_off:" << x_off << "y_off:" << y_off << std::endl;
	}
	return;
}