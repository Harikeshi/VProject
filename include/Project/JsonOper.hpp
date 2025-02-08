#pragma once

#include "Perimeter.hpp"
#include "Route.hpp"

#include <nlohmann/json.hpp>

class Json
{
public:
    static std::vector<Route> parseToRoutes(const nlohmann::json& json)
    {
        std::vector<Route> result;

        if (!json.contains("routes"))
        {
            throw std::runtime_error("Объект не содердит \"routes\".");
        }

        if (!json["routes"].is_array())
        {
            throw std::runtime_error("\"routes\" не является массивом.");
        }

        auto routes = json["routes"];

        for (const auto& route : routes)
        {
            if (!route.contains("points"))
            {
                throw std::runtime_error("Объект не содердит \"points\".");
            }

            if (!route["points"].is_array())
            {
                throw std::runtime_error("\"points\" не является массивом.");
            }
            if (!route.contains("velocities"))
            {
                throw std::runtime_error("Объект не содердит \"velocities\".");
            }

            if (!route["velocities"].is_array())
            {
                throw std::runtime_error("\"velocities\" не является массивом.");
            }

            if (route["points"].size() != route["velocities"].size())
            {
                throw std::runtime_error("Длина \"points\" не совпадает с длиной \"velocities\".");
            }

            Route path;

            for (size_t i = 0; i != route["points"].size() - 1; ++i)
            {
                if (!route["points"][i].is_array() || !route["points"][i + 1].is_array())
                {
                    throw std::runtime_error("Данные для точки не являются массивом.");
                }
                if (!(route["points"][i].size() == 2) || !(route["points"][i + 1].size() == 2))
                {
                    throw std::runtime_error("Длина массива точки != 2.");
                }

                Segment segment{QPointF{route["points"][i][0].get<double>(), route["points"][i][1].get<double>()},
                                QPointF{route["points"][i + 1][0].get<double>(), route["points"][i + 1][1].get<double>()},
                                route["velocities"][i + 1]};

                path.addSegment(segment);
            }

            result.push_back(path);
        }

        return result;
    }

    static Perimeter parseToPolygon(const nlohmann::json& json)
    {
        Perimeter result;

        if (!json.contains("search_region"))
        {
            throw std::runtime_error("Объект не содержит \"search_region\".");
        }

        if (!json["search_region"].contains("borders"))
        {
            throw std::runtime_error("Объект \"search_region\" не содержит \"borders\".");
        }

        if (!json["search_region"]["borders"].is_array())
        {
            throw std::runtime_error("\"points\" не является массивом.");
        }

        for (const auto& border : json["search_region"]["borders"])
        {
            if (!border.is_array())
            {
                throw std::runtime_error("Объект внутри \"borders\" не является массивом.");
            }

            std::vector<QPointF> polyline;

            for (const auto& point : border)
            {
                if (!point.is_array())
                {
                    throw std::runtime_error("Данные для точки не являются массивом.");
                }
                if (!(point.size() == 2))
                {
                    throw std::runtime_error("Длина массива точки != 2.");
                }
                if (!point[0].is_number() || !point[1].is_number())
                {
                    throw std::runtime_error("Данные в массиве точки не являются числом.");
                }

                polyline.push_back(QPointF{point[0].get<double>(), point[1].get<double>()});
            }

            result.addPolyline(polyline);
        }

        return result;
    }
};
