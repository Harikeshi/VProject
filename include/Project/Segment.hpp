#pragma once

#include <QPointF>

#include <cmath>

struct Segment
{
    QPointF start;
    QPointF end;

    double baseSpeed;
    double currentSpeed; // Текущая скорость с учетом множителя

    Segment(QPointF s = QPointF{}, QPointF e = QPointF{}, double b = 0)
        : start(s), end(e), baseSpeed(b), currentSpeed(b)
    {
    }

    double length() const
    {
        return std::hypot(end.x() - start.x(), end.y() - start.y());
    }
};
