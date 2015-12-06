#include "rowblob.h"

RowBlob::RowBlob()
{

}

RowBlob::~RowBlob()
{

}

RowBlob::RowBlob(vector<Point> _contour)
{
	contour = _contour;
	mu = moments(contour, false);
	mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
}

vector<Point> RowBlob::getContour()
{
	return contour;
}

void RowBlob::setContour(vector<Point> _contour)
{
	contour = _contour;
	mu = moments(contour, false);
	mc = Point2f(mu.m10 / mu.m00, mu.m01 / mu.m00);
}

Moments RowBlob::getMoments()
{
	return mu;
}

Point2f RowBlob::getmassCenter()
{
	return mc;
}
