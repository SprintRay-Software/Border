#include "borderfinder.h"
#include "slicerutils.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <QPainter>

inline uint qHash(const QPoint &key) { return (key.x() << 16) + key.y(); }

BorderFinder::BorderFinder(const QString &pathToImage) : isBorder(), rowEntranceExit(), borderPoints() {
	QFileInfo inputFile(pathToImage);

	if (!inputFile.exists()) {
		qFatal(BORDERFINDERCONSTANTS::FILE_NOT_FOUND.toLatin1().constData(), pathToImage.toLatin1().constData());
	}

	QFileInfo fileInfo(pathToImage);
	QString fileName = fileInfo.baseName();

	if (inputFile.suffix() == "stl" || inputFile.suffix() == "obj") {
		SlicerUtils::Slice(pathToImage);

		qDebug() << "QImage = " << QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH + "/" + fileName + "/AutoOrientPolygons_0.png";
		QFile image(QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH + "/" + fileName + "/AutoOrientPolygons_0.png");
		if (image.exists())
		{
			qDebug() << "image.exists()";
			this->img = QImage(QCoreApplication::applicationDirPath() + BORDERFINDERCONSTANTS::SQUASHED_IMG_PATH + "/" + fileName + "/AutoOrientPolygons_0.png");
			//this->img = QImage("D:/build-rayware-Desktop_Qt_5_11_2_MSVC2015_64bit-Release/release/Borderfinder/teeth1/AutoOrientPolygons_0.png");
			//this->img = QImage("C:/Program Files (x86)/RayWare/Borderfinder/teeth1/AutoOrientPolygons_0.png");
		}
	}
	else {
		this->img = QImage(pathToImage);
	}

	if (this->img.isNull()) {
		qDebug() << "img.isNull()";
		//qFatal(BORDERFINDERCONSTANTS::FILE_TYPE_INCORRECT.toLatin1().constData(), pathToImage.toLatin1().constData());
	}
}

bool BorderFinder::drawBorderPointsToImage(QString imagePath) {
	if (img.isNull())
	{
		qDebug() << "drawBorderPointsToImage.isNull()";
		return false;
	}
	QImage borderedImg(img);

	borderedImg = grayScaleToBlackAndWhite(borderedImg);

	QVector<QPoint> qvec_point = getBorderPoints();
	vector<QPoint> qvec_point1;
	vector<QPoint> pointListOut;
	for (QPoint point : qvec_point)
	{
		qvec_point1.push_back(point);
	}

	double xmin = qvec_point[0].x(), xmax = qvec_point[0].x(), ymin = qvec_point[0].y(), ymax = qvec_point[0].y();
	for (QPoint point : qvec_point)
	{
		if (point.x() <= xmin)
		{
			xmin = point.x();
		}
		if (point.y() <= ymin)
		{
			ymin = point.y();
		}
		if (point.x() >= xmax)
		{
			xmax = point.x();
		}
		if (point.y() >= ymax)
		{
			ymax = point.y();
		}
	}
	double w_coor = (xmax + xmin) / 2.0;
	double h_coor = (ymax + ymin) / 2.0;

	double y_offset = 1080 - 2 * h_coor;

	RamerDouglasPeucker(qvec_point1, 5.0, pointListOut);

	QPainter p(&borderedImg);
	p.setPen(QColor(255, 0, 0));
	int length = pointListOut.size();
	for (int i = 0; i < length; i++)
	{
		p.drawLine(pointListOut[i%length].x(), pointListOut[i%length].y()+ y_offset, pointListOut[(i + 1) % length].x(), pointListOut[(i + 1) % length].y()+ y_offset);
	}

	//for (QPoint pixel : pointListOut) {
	//	// don't try to change an offscreen pixel
	//	if (!isPointOutOfBounds(pixel)) {
	//		borderedImg.setPixelColor(pixel, BORDERFINDERCONSTANTS::BORDER_PIXEL);
	//		// hardcoded for teeth1.stl
	//	}
	//}

	// will return false if error
	return borderedImg.save(imagePath);
}

bool BorderFinder::writeBorderPointsToFile(QString filePath, int x_off, int y_off) {
	QFile file(filePath);
	if (file.open(QFile::Append)) {
		QTextStream out(&file);
		QVector<QPoint> qvec_point = getBorderPoints();
		if (qvec_point.size() <= 0)
		{
			return false;
		}
		for (QPoint point : qvec_point)
		{
			point.setX(point.x() + x_off);
			point.setY(point.y() + y_off);
		}
		vector<QPoint> qvec_point1;
		vector<QPoint> pointListOut;
		for (QPoint point : qvec_point)
		{
			qvec_point1.push_back(point);
		}

		double xmin= qvec_point[0].x(), xmax = qvec_point[0].x(), ymin = qvec_point[0].y(), ymax = qvec_point[0].y();
		for (QPoint point : qvec_point)
		{
			if (point.x() <= xmin)
			{
				xmin = point.x();
			}
			if (point.y() <= ymin)
			{
				ymin = point.y();
			}
			if (point.x() >= xmax)
			{
				xmax = point.x();
			}
			if (point.y() >= ymax)
			{
				ymax = point.y();
			}
		}
		double w_coor = (xmax + xmin) / 2.0;
		double h_coor = (ymax + ymin) / 2.0;

		double y_offset = 1080 - 2 * h_coor;


		RamerDouglasPeucker(qvec_point1, 5.0, pointListOut);
		out << pointListOut.size();
		for (QPoint point : pointListOut) {
			out << ' ' << '[' << point.x() << ',' << (point.y() + y_offset) << ']';
		}
		out << endl;
		file.close();
		return true;
	}
	return false;
}

QImage BorderFinder::grayScaleToBlackAndWhite(QImage img) {
	QImage blackAndWhite(img);
	for (int y = 0; y < blackAndWhite.height(); ++y) {
		for (int x = 0; x < blackAndWhite.width(); ++x) {
			QColor color = blackAndWhite.pixelColor(x, y);
			//if (color != BORDERFINDERCONSTANTS::EMPTY_PIXEL && color != BORDERFINDERCONSTANTS::FILLED_PIXEL) {
				blackAndWhite.setPixelColor(x, y, BORDERFINDERCONSTANTS::FILLED_PIXEL);
			//}
		}
	}
	return blackAndWhite;
}

QVector<QPoint> BorderFinder::getBorderPoints() {
	if (this->borderPoints.empty()) {
		if (img.isNull())
		{
			qDebug() << "getBorderPoints.isNull()";
			return this->borderPoints;
		}
		for (QPoint currPoint = findStartPointOfNewShape(QPoint(0, 0));
			currPoint != NOT_IN_PICTURE;
			currPoint = findStartPointOfNewShape(currPoint)) {
			this->shapePoints.append(outlineShape(currPoint));
		}

		if (this->shapePoints.size() == 1) {
			this->borderPoints = this->shapePoints[0];
		}
		else {
			this->img = connectIslands();
			//this->img.save("/home/aidan/Sprintray/combined.png");
			QPoint startPt = findStartPointOfNewShape(QPoint(0, 0));
			this->isBorder.clear();
			this->borderPoints = outlineShape(startPt);
		}
	}
	return this->borderPoints;
}

QPoint BorderFinder::findStartPointOfNewShape(const QPoint &start) {
	// scan top to bottom, left to right
	QPoint currPoint(start);
	// TODO: i need the inner x loop to initalize to start, but whenever the loop
	// restarts it needs to go back to 0. I never reset Y so that isn't urgent
	// but shouldn't be set to 0 either.
	for (currPoint.setY(0);
		currPoint.y() < img.height() && currPoint != NOT_IN_PICTURE;
		currPoint.setY(currPoint.y() + 1)) {
		for (currPoint.setX(0);
			currPoint.x() < img.width() && currPoint != NOT_IN_PICTURE;
			currPoint.setX(currPoint.x() + 1)) {
			/* TODO: implement island logic
			 * If not visited, this is a new area
			 * If visited, access data structure to find out how to "skip" over
			 * the shape, find exit point for the row
			 */
			if (this->isBorder.contains(currPoint)) {
				// DIAGONAL PIECES COULD BE ADDED TWICE. NEED TO LOOK INTO THIS
				// idea: focus back on the edge part. change isBorder to hold pairs
				// what we'd want to check is if the top edge is already part of a
				// border
				currPoint = skipOverOldShape(currPoint);
				if (currPoint == NOT_IN_PICTURE) {
					return NOT_IN_PICTURE;
				}
			}

			if (img.pixelColor(currPoint) != BORDERFINDERCONSTANTS::EMPTY_PIXEL) {
				return currPoint;
			}
		}
	}

	return NOT_IN_PICTURE;
}

QPoint BorderFinder::skipOverOldShape(QPoint currPoint) {
	int x = currPoint.x(), y = currPoint.y();
	// single pixel case, not against image border
	if (x + 1 < img.width() && img.pixelColor(x + 1, y) == BORDERFINDERCONSTANTS::EMPTY_PIXEL) {
		return QPoint(x + 1, y);
	}

	QVector<int> &rowVector = this->rowEntranceExit[y];
	// qFind is depreciated
	int *exit = std::find(rowVector.begin(), rowVector.end(), x) + 1;

	if (exit != rowVector.end() && *exit + 1 < img.width()) {
		return QPoint(*exit + 1, y);
	}

	// our exit is at the border of the row (for single pixel case too).
	// Move to next row

	if (y + 1 < img.height()) {
		QPoint nextRow = QPoint(0, y + 1);
		return (isPixelFilled(nextRow)) ? skipOverOldShape(nextRow) : nextRow;
	}
	else {
		return NOT_IN_PICTURE;
	}
}

QVector<QPoint> BorderFinder::outlineShape(const QPoint &startPoint) {
	/* We treat coordinates as the top left of the pixel. This vector adds the
	 * top edge of our startPoint to our shape. This edge exists because we
	 * parsed the image top to bottom, left to right, and this is the first
	 * filled pixel we found: if there was a pixel above it that was filled,
	 * that would have been our topLeft pixel.
	 */
	QVector<QPoint> edgePoints = { startPoint,
								  QPoint(startPoint.x() + 1, startPoint.y()) };

	QVector<QPoint> borderPixels = { startPoint };

	this->isBorder.insert(startPoint);
	this->rowEntranceExit[startPoint.y()].append(startPoint.x());
	this->topLeftCorners.append(startPoint);

	// We start searching from the last point, so the edge we just traveled is
	// a left edge from that edge's perspective
	EDGE_DIR lastDir = EDGE_DIR::LEFT;

	// start at 1 to start searching the vector at the last point
	for (int i = 1; i < edgePoints.size(); ++i) {
		uint x = edgePoints[i].x(), y = edgePoints[i].y();

		QPair<EDGE_DIR, QPoint> currEdge =
			findNextEdgeFromPoint(edgePoints[i], lastDir);

		// add the correct point to the vector according to the edge dir
		QPoint newEdgePoint;
		switch (currEdge.first) {
		case EDGE_DIR::UP:
			newEdgePoint = QPoint(x, y - 1);
			break;
		case EDGE_DIR::RIGHT:
			newEdgePoint = QPoint(x + 1, y);
			break;
		case EDGE_DIR::DOWN:
			newEdgePoint = QPoint(x, y + 1);
			break;
		case EDGE_DIR::LEFT:
			newEdgePoint = QPoint(x - 1, y);
			break;
		case EDGE_DIR::ERR:
			// TODO: this case should never happen, but I want to be
			// absolutely sure of that before I remove it
			qFatal(BORDERFINDERCONSTANTS::NEXT_EDGE_NOT_FOUND.toLatin1().constData(), x, y);
			return edgePoints;
		}

		// once we "travel" along an edge to our next point, the direction
		// is inverted. Traveling along a left edge makes it a right edge at
		// the new point
		lastDir = invertDir[currEdge.first];

		edgePoints.push_back(newEdgePoint);

		// if the shape we just added is our original point, we finished loop
		if (edgePoints[i + 1] == startPoint) {
			// this will leave the vector at the same size and end the loop
			edgePoints.pop_back();
		}
		else {
			// TODO: separate into their own funcs
			if (currEdge.first == EDGE_DIR::DOWN || currEdge.first == EDGE_DIR::UP) {
				// only add border pixels that are borders on the row
				this->rowEntranceExit[currEdge.second.y()].append(currEdge.second.x());
			}
			// add new border pixel
			if (currEdge.second != borderPixels.last()) {
				borderPixels.append(currEdge.second);
				this->isBorder.insert(currEdge.second);
			}
		}
	}

	for (QVector<int> &vector : this->rowEntranceExit) {
		// qSort is depreciated, using std::sort as recommended
		std::sort(vector.begin(), vector.end());
	}

	return edgePoints;
}

QPair<BorderFinder::EDGE_DIR, QPoint> BorderFinder::findNextEdgeFromPoint(
	const QPoint &point, BorderFinder::EDGE_DIR lastDir) {
	uint x = point.x(), y = point.y();

	QPoint UPLEFT(x - 1, y - 1);
	QPoint UPRIGHT(x, y - 1);
	QPoint DOWNLEFT(x - 1, y);
	QPoint DOWNRIGHT(x, y);

	QPair<EDGE_DIR, QPoint> nextDir;

	if (isPointColorEqual(UPLEFT, DOWNRIGHT) &&
		isPointColorEqual(UPRIGHT, DOWNLEFT) &&
		!isPointColorEqual(UPLEFT, UPRIGHT)) {
		return findNextEdgeDiagonal(DOWNRIGHT, lastDir);
	}

	// mismatching colors means we want an edge in that direction, make sure
	// it's a new edge
	if (!isPointColorEqual(UPLEFT, UPRIGHT) && lastDir != EDGE_DIR::UP) {
		nextDir = QPair<EDGE_DIR, QPoint>(
			EDGE_DIR::UP, isPixelFilled(UPLEFT) ? UPLEFT : UPRIGHT);
	}
	else if (!isPointColorEqual(UPRIGHT, DOWNRIGHT) &&
		lastDir != EDGE_DIR::RIGHT) {
		nextDir = QPair<EDGE_DIR, QPoint>(
			EDGE_DIR::RIGHT, isPixelFilled(UPRIGHT) ? UPRIGHT : DOWNRIGHT);
	}
	else if (!isPointColorEqual(DOWNRIGHT, DOWNLEFT) &&
		lastDir != EDGE_DIR::DOWN) {
		nextDir = QPair<EDGE_DIR, QPoint>(
			EDGE_DIR::DOWN, isPixelFilled(DOWNRIGHT) ? DOWNRIGHT : DOWNLEFT);
	}
	else if (!isPointColorEqual(DOWNLEFT, UPLEFT) &&
		lastDir != EDGE_DIR::LEFT) {
		nextDir = QPair<EDGE_DIR, QPoint>(
			EDGE_DIR::LEFT, isPixelFilled(DOWNLEFT) ? DOWNLEFT : UPLEFT);
	}
	else {
		// TODO: this case should never happen, but want to be confident before
		// I remove it
		nextDir = QPair<EDGE_DIR, QPoint>(EDGE_DIR::ERR, NOT_IN_PICTURE);
	}

	return nextDir;
}

QPair<BorderFinder::EDGE_DIR, QPoint> BorderFinder::findNextEdgeDiagonal(
	const QPoint &DOWNRIGHT, BorderFinder::EDGE_DIR lastDir) {
	// We know it's a diagonal case: simply stick on the pixel you started from
	// since we know it's diagonal, one pixel color tells us the rest
	// TODO: clarify this!

	uint x = DOWNRIGHT.x(), y = DOWNRIGHT.y();

	QPoint UPLEFT(x - 1, y - 1);
	QPoint UPRIGHT(x, y - 1);
	QPoint DOWNLEFT(x - 1, y);

	switch (lastDir) {
	case EDGE_DIR::UP:
		if (isPixelFilled(UPLEFT)) {
			return QPair<EDGE_DIR, QPoint>(EDGE_DIR::LEFT, UPLEFT);
		}
		else {
			return QPair<EDGE_DIR, QPoint>(EDGE_DIR::RIGHT, UPRIGHT);
		}
	case EDGE_DIR::RIGHT:
		if (isPixelFilled(DOWNRIGHT)) {
			return QPair<EDGE_DIR, QPoint>(EDGE_DIR::DOWN, DOWNRIGHT);
		}
		else {
			return QPair<EDGE_DIR, QPoint>(EDGE_DIR::UP, UPRIGHT);
		}
	case EDGE_DIR::DOWN:
		if (isPixelFilled(DOWNRIGHT)) {
			return QPair<EDGE_DIR, QPoint>(EDGE_DIR::RIGHT, DOWNRIGHT);
		}
		else {
			return QPair<EDGE_DIR, QPoint>(EDGE_DIR::LEFT, DOWNLEFT);
		}
	case EDGE_DIR::LEFT:
		if (isPixelFilled(UPLEFT)) {
			return QPair<EDGE_DIR, QPoint>(EDGE_DIR::UP, UPLEFT);
		}
		else {
			return QPair<EDGE_DIR, QPoint>(EDGE_DIR::DOWN, DOWNLEFT);
		}
	default:
		return QPair<EDGE_DIR, QPoint>(EDGE_DIR::ERR, NOT_IN_PICTURE);
	}
}

bool BorderFinder::isPixelFilled(const QPoint &point) {
	if (isPointOutOfBounds(point)) {
		return false;
	}
	else {
		return img.pixelColor(point) != BORDERFINDERCONSTANTS::EMPTY_PIXEL;
	}
}

bool BorderFinder::isPointColorEqual(const QPoint &point1,
	const QPoint &point2) {
	// Offscreen pixels are treated as EMPTY_PIXEL so that they clash with
	// a FILLED_PIXEL and thereby create an edge
	if (isPointOutOfBounds(point1) && isPointOutOfBounds(point2)) {
		return true;
	}
	else if (isPointOutOfBounds(point1)) {
		return img.pixelColor(point2) == BORDERFINDERCONSTANTS::EMPTY_PIXEL;
	}
	else if (isPointOutOfBounds(point2)) {
		return img.pixelColor(point1) == BORDERFINDERCONSTANTS::EMPTY_PIXEL;
	}
	else {
		// two states: either both EMPTY_PIXEL or both not
		return isPixelFilled(point1) == isPixelFilled(point2);
	}
}

bool BorderFinder::isPointOutOfBounds(const QPoint &point) {
	return point.x() < 0 || point.x() >= img.width() || point.y() < 0 ||
		point.y() >= img.height();
}

QVector<QPoint> BorderFinder::getLineBetweenTwoPoints(QPoint point1,
	QPoint point2) {
	// from http://members.chello.at/~easyfilter/Bresenham.pdf 1.7 and
	// https://stackoverflow.com/questions/8936183/bresenham-lines-w-o-diagonal-movement
	QVector<QPoint> points;
	int x0 = point1.x(), x1 = point2.x(), y0 = point1.y(), y1 = point2.y();
	int changeX = qAbs(x1 - x0);
	int dirX = x0 < x1 ? 1 : -1;
	int changeY = -qAbs(y1 - y0);
	int dirY = y0 < y1 ? 1 : -1;
	int err = changeX + changeY;

	points.append(point1);

	while (x0 != x1 || y0 != y1) {
		if (err * 2 - changeY > changeX - err * 2) {
			err += changeY;
			x0 += dirX;
		}
		else {
			err += changeX;
			y0 += dirY;
		}
		points.append(QPoint(x0, y0));
	}

	return points;
}

QImage BorderFinder::connectIslands() {
	QImage connected(this->img);
	for (int i = 0; i < this->topLeftCorners.size(); ++i) {
		for (int j = i; j < this->topLeftCorners.size(); ++j) {
			QVector<QPoint> line =
				getLineBetweenTwoPoints(topLeftCorners[i], topLeftCorners[j]);
			drawLine(connected, line);
		}
	}
	return connected;
}

void BorderFinder::drawLine(QImage &img, QVector<QPoint> points) {
	for (QPoint point : points) {
		img.setPixelColor(point, BORDERFINDERCONSTANTS::FILLED_PIXEL);
	}
}

double BorderFinder::PerpendicularDistance(const QPoint &pt, const QPoint lineStart, const QPoint lineEnd)
{
	double dx = lineEnd.x() - lineStart.x();
	double dy = lineEnd.y() - lineStart.y();

	//Normalise
	double mag = pow(pow(dx, 2.0) + pow(dy, 2.0), 0.5);
	if (mag > 0.0)
	{
		dx /= mag; dy /= mag;
	}

	double pvx = pt.x() - lineStart.x();
	double pvy = pt.y() - lineStart.y();

	//Get dot product (project pv onto normalized direction)
	double pvdot = dx * pvx + dy * pvy;

	//Scale line direction vector
	double dsx = pvdot * dx;
	double dsy = pvdot * dy;

	//Subtract this from pv
	double ax = pvx - dsx;
	double ay = pvy - dsy;

	return pow(pow(ax, 2.0) + pow(ay, 2.0), 0.5);
}

void BorderFinder::RamerDouglasPeucker(const vector<QPoint> &pointList, double epsilon, vector<QPoint> &out)
{
	if (pointList.size() < 2)
	{
		qDebug() << "Not enough points to simplify";
		return;
	}

	// Find the point with the maximum distance from line between start and end
	double dmax = 0.0;
	size_t index = 0;
	size_t end = pointList.size() - 1;
	for (size_t i = 1; i < end; i++)
	{
		double d = PerpendicularDistance(pointList[i], pointList[0], pointList[end]);
		if (d > dmax)
		{
			index = i;
			dmax = d;
		}
	}

	// If max distance is greater than epsilon, recursively simplify
	if (dmax > epsilon)
	{
		// Recursive call
		vector<QPoint> recResults1;
		vector<QPoint> recResults2;
		vector<QPoint> firstLine(pointList.begin(), pointList.begin() + index + 1);
		vector<QPoint> lastLine(pointList.begin() + index, pointList.end());
		RamerDouglasPeucker(firstLine, epsilon, recResults1);
		RamerDouglasPeucker(lastLine, epsilon, recResults2);

		// Build the result list
		out.assign(recResults1.begin(), recResults1.end() - 1);
		out.insert(out.end(), recResults2.begin(), recResults2.end());
		if (out.size() < 2)
		{
			qDebug() << "Problem assembling output";
			return;
		}
	}
	else
	{
		//Just return start and end points
		out.clear();
		out.push_back(pointList[0]);
		out.push_back(pointList[end]);
	}
}