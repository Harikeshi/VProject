#pragma once

#include <QRect>
#include <QTransform>

class CoordinateSystem
{
public:
    CoordinateSystem(QRect area, const double& maxWidth, const double& maxHeight)
    {
        calculateTransform(area, maxWidth, maxHeight);
    }

    void updateArea(QRect area, const double& maxWidth, const double& maxHeight)
    {
        calculateTransform(area, maxWidth, maxHeight);
    }
    QTransform getTransform() const
    {
        return transform;
    }
    QPointF toLogical(QPointF screen_pos) const
    {
        return transform.inverted().map(screen_pos);
    }

    QRect getContentRect() const
    {
        return contentRect;
    }

private:
    void calculateTransform(QRect area, const double& maxWidth, const double& maxHeight)
    {
        // Отступы
        const int left = 100;   // Увеличиваем отступ слева
        const int bottom = 100; // Увеличиваем отступ снизу
        const int right = 20;
        const int top = 20;

        contentRect = QRect(
            left,
            top,
            area.width() - left - right,
            area.height() - top - bottom);

        transform = QTransform()
                         .translate(left, area.height() - bottom)
                         .scale(1, -1)
                         .scale(contentRect.width() / maxWidth, contentRect.height() / maxHeight)
                         .translate(-20, -20);
    }

    QTransform transform;
    QRect contentRect;
};
