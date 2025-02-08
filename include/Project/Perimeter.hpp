#pragma once

#include <QPainter>
#include <QPointF>
#include <QTransform>

#include <vector>

class Perimeter
{
public:
    void addPolyline(const std::vector<QPointF>& points)
    {
        for (const auto& point : points)
        {
            if (point.x() > maxX)
            {
                maxX = point.x();
            }
            if (point.y() > maxY)
            {
                maxY = point.y();
            }
            if (point.x() < minX)
            {
                minX = point.x();
            }
            if (point.y() < minY)
            {
                minY = point.y();
            }
        }

        polylines.push_back(points);
    }

    void draw(QPainter& painter, const QTransform& transform) const
    {
        painter.setPen(Qt::blue);
        for (const auto& polyline : polylines)
        {
            for (size_t i = 1; i < polyline.size(); ++i)
            {
                QPointF p1 = transform.map(polyline[i - 1]);
                QPointF p2 = transform.map(polyline[i]);
                painter.drawLine(p1, p2);
            }
        }
    }

    double getMinX()
    {
        return minX;
    }

    double getMinY()
    {
        return minY;
    }

    double getMaxX()
    {
        return maxX;
    }

    double getMaxY()
    {
        return maxY;
    }

private:
    double minX, minY, maxX, maxY;

    std::vector<std::vector<QPointF>> polylines;
};
