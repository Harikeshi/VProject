#pragma once

#include "Segment.hpp"

#include <QColor>

class Route
{
public:
    Route()
    {
        static int hue = 0;
        color = QColor::fromHsl(hue, 255, 128);
        hue = (hue + 45) % 360;
    }

    Route(const int size, const QColor& c): segments(size), color{c}{}
    void reset()
    {
        currentSegment = 0;
        progress = 0;
    }

    Segment getCurrentSegment() const
    {
        return segments[currentSegment];
    }

    void setLastSegment(const Segment& segment)
    {
        if (!segments.empty())
            segments.back() = segment;
    }

    void setSegment(const size_t index, const Segment& segment)
    {
        segments[index] = segment;
    }

    void addSegment(QPointF start, QPointF end, double speed)
    {
        segments.push_back({start, end, speed});
    }

    void addSegment(const Segment& segment)
    {
        segments.push_back(segment);
    }

    void update(double deltaTime)
    {
        if (currentSegment >= segments.size())
            return;

        auto& seg = segments[currentSegment];
        double maxTravel = seg.currentSpeed * deltaTime;
        double remaining = seg.length() * (1.0 - progress);

        if (maxTravel >= remaining)
        {
            progress = 1.0;
            currentSegment++;
            if (currentSegment < segments.size())
                progress = 0;
        }
        else
        {
            progress += maxTravel / seg.length();
        }
    }

    QPointF getCurrentPosition() const
    {
        if (segments.empty())
            return {};

        if (currentSegment >= segments.size())
            return segments.back().end;

        const auto& seg = segments[currentSegment];

        return seg.start + (seg.end - seg.start) * progress;
    }

    double getCurrentSpeed() const
    {
        if (currentSegment >= segments.size())
            return 0;

        return segments[currentSegment].currentSpeed;
    }

    QColor getColor() const
    {
        return color;
    }

    void setColor(const QColor& c) {
        color = c;
    }
    const std::vector<Segment>& getSegments() const
    {
        return segments;
    }

    void setSpeedMultiplier(double multiplier)
    {
        for (auto& seg : segments)
        {
            seg.currentSpeed = seg.baseSpeed * multiplier;
        }
    }

    // private:
    //     struct Segment {
    //         QPointF start;
    //         QPointF end;
    //         double base_speed;
    //         double speed;
    //     };

    size_t getCurrentIndex() const
    {
        return currentSegment;
    }

private:
    std::vector<Segment> segments;
    QColor color;

    size_t currentSegment = 0; // Index
    double progress = 0;
};
