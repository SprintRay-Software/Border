#pragma once
#ifndef IMAGETRAVERSE_H
#define IMAGETRAVERSE_H

#include <QColor>
#include <QHash>
#include <QImage>
#include <QMap>  // TODO: QHash?
#include <QPair>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QVector>
#include <vector> 

#include "borderfinderConstants.h"

using namespace std;

const QPoint NOT_IN_PICTURE = QPoint(-1, -1);

class BorderFinder {
public:
	/**
	 * Constructor to take in file path to image and turn into QImage for later
	 * processing
	 *
	 * @param pathToImage Path to image (png and stl tested)
	 */
	BorderFinder(const QString &pathToImage);

	/**
	 * Write the borderpoints for the current Bordermaker to a text file in the
	 * format 'x y\n'.
	 *
	 * @param filePath Path to write points to
	 * @return true if succesful, false otherwise
	 */
	bool writeBorderPointsToFile(QString filePath, int x_off, int y_off);

	/**
	 * Draw the borderpoints for the current Bordermaker to an image file.
	 *
	 * @param filePath Path to write image to (path must end in supported QImage
	 * 				   format, ex .png)
	 * @return true if succesful, false otherwise
	 */
	bool drawBorderPointsToImage(QString imagePath);

private:
	// The image to read and modify
	QImage img;

	// set to hold borders
	QSet<QPoint> isBorder;

	QHash<int, QVector<int>> rowEntranceExit;

	// The points
	QVector<QPoint> borderPoints;

	// track each shape
	QVector<QVector<QPoint>> shapePoints;

	// track the approximate center of each shape
	// TODO: edge cases (extreme left outlier, etc)
	QVector<QPoint> topLeftCorners;

	// Enum to track directions
	enum class EDGE_DIR { LEFT, RIGHT, UP, DOWN, ERR };

	// map to invert the enums
	QMap<EDGE_DIR, EDGE_DIR> invertDir =
		QMap<EDGE_DIR, EDGE_DIR>({ {EDGE_DIR::LEFT, EDGE_DIR::RIGHT},
								  {EDGE_DIR::RIGHT, EDGE_DIR::LEFT},
								  {EDGE_DIR::UP, EDGE_DIR::DOWN},
								  {EDGE_DIR::DOWN, EDGE_DIR::UP} });

	/**
	 * Returns the borderpoints for the current image.
	 *
	 * @return borderpoints for the current image
	 */
	QVector<QPoint> getBorderPoints();

	/**
	 * From a border point of a discovered shape, move over the internals of the
	 * discovered shape until an empty pixel is discovered in the row, or move to
	 * next row.
	 *
	 * @param currPoint A border point of a shape in shapePoints
	 * @return The next empty pixel
	 */
	QPoint skipOverOldShape(QPoint currPoint);

	/**
	 * Convert all gray pixels to white, so the image only has black and white
	 * pixels. Only useful for outputting to image!
	 *
	 * @param img The image to convert to black and white
	 * @return the image in a black and white format
	 */
	QImage grayScaleToBlackAndWhite(QImage img);

	/**
	 * Traverse top to bottom, left to right scanning for the first filled pixel
	 *
	 * @param start The point to start scanning at
	 * @return First point of new shape
	 */
	QPoint findStartPointOfNewShape(const QPoint &start);

	/**
	 * Takes a point, direction of last edge, and returns the next edge to go to
	 *
	 * @param point Cooresponds to top left of pixel, where the next edge should
	 * start
	 * @param lastDir The direction of the last found edge relative to point
	 * @return Direction for the next edge and the border pixel
	 */
	QPair<EDGE_DIR, QPoint> findNextEdgeFromPoint(const QPoint &point,
		EDGE_DIR lastDir);

	/**
	 * Special case for finding next edge when there are two filled pixels
	 * diagonal from each other with the other two being empty.
	 *
	 * @param DOWNRIGHT The pixel to the downright of the point in question
	 * @param lastDir The direction of the last found edge relative to point
	 * @return Direction for the next edge and the border pixel
	 */
	QPair<EDGE_DIR, QPoint> findNextEdgeDiagonal(const QPoint &DOWNRIGHT,
		EDGE_DIR lastDir);

	/**
	 * Given the topmost leftmost filled pixel of a shape, finds and returns the
	 * border edges
	 *
	 * @param startPoint the topmost leftmost filled pixel of a shape
	 * @return List of all points for the border around this shape
	 */
	QVector<QPoint> outlineShape(const QPoint &startPoint);

	/**
	 * An implementation of Bresenham's line algorithm to find the points between
	 * two points, with a modification to prevent diagonal moves.
	 *
	 * @param point1 the first point
	 * @param point2 the second point
	 * @return the points between those two to make a line
	 * @see <a
	 * href="https://stackoverflow.com/questions/8936183/bresenham-lines-w-o-diagonal-movement">Source</a>
	 * @see <a href="http://members.chello.at/~easyfilter/Bresenham.pdf">Paper
	 * (Section 1.7)</a>
	 */
	QVector<QPoint> getLineBetweenTwoPoints(QPoint point1, QPoint point2);

	/**
	 * Helper function, given two points check if their color is equal
	 *
	 * @param point1 First point
	 * @param point2 Second point
	 * @return true if colors are equal, false otherwise
	 */
	bool isPointColorEqual(const QPoint &point1, const QPoint &point2);

	/**
	 * Helper function to determine if a pixel is within the image boundary.
	 *
	 * @param p The point to check for out of bounds
	 * @return true if out of bounds, false otherwise
	 */
	bool isPointOutOfBounds(const QPoint &point);

	/**
	 * Return if the given pixel is filled.
	 *
	 * @param point The point to determine if it is filled or not
	 * @return true is filled
	 */
	bool isPixelFilled(const QPoint &point);

	void drawLine(QImage &img, QVector<QPoint> points);

	QImage connectIslands();

	double PerpendicularDistance(const QPoint &pt, const QPoint lineStart, const QPoint lineEnd);

	void RamerDouglasPeucker(const vector<QPoint> &pointList, double epsilon, vector<QPoint> &out);
};

#endif  // IMAGETRAVERSE_H


