#pragma once

#include "CoordinateSystem.hpp"
#include "Perimeter.hpp"
#include "Route.hpp"

#include <QComboBox>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QWidget>

#include <vector>

class VisualizationWidget : public QWidget
{
    // TODO: отрисовка по шагам с сохранением pointer
    // TODO: Отрисовка из Json: выбор json для отрисовки ->request.json ->result.json // menu/file/open

    // TODO: Отрисовка должна начинаться по команде




public:
    VisualizationWidget(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        timer.start();
        connect(&updateTimer, &QTimer::timeout, this, [this]() {
            const double deltaTime = timer.restart() / 1000.0;
            for (auto& route : routes)
                route.update(deltaTime);
            update();
        });
        updateTimer.start(16); // ~60 FPS

        speedButton = new QPushButton("Turbo x15", this);
        speedButton->setCheckable(true);
        speedButton->setGeometry(QRect(10, 10, 100, 30));
        connect(speedButton, &QPushButton::toggled, [this](bool checked) {
            for (auto& route : routes)
            {
                route.setSpeedMultiplier(checked ? 15.0 : 1.0);
            }
        });

        speedCombo = new QComboBox(this);
        speedCombo->addItem("1x", 1.0);
        speedCombo->addItem("2x", 2.0);
        speedCombo->addItem("5x", 5.0);
        speedCombo->addItem("10x", 10.0);
        speedCombo->addItem("15x", 15.0);
        speedCombo->setCurrentIndex(0);

        // Позиционируем элементы управления
        speedCombo->setGeometry(QRect(10, 10, 100, 30));

        connect(speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
            double multiplier = speedCombo->currentData().toDouble();
            for (auto& route : routes)
            {
                route.setSpeedMultiplier(multiplier);
            }
        });

        setMouseTracking(true);

        // Demo setup
        setup();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        double maxHeight = perimeter.getMaxY() - perimeter.getMinY();
        double maxWidth = perimeter.getMaxX() - perimeter.getMinX();

        CoordinateSystem cs(rect(), maxWidth, maxHeight);

        drawGrid(painter, cs); // Отрисовка сетки
        perimeter.draw(painter, cs.getTransform());

        // Отрисовка маршрутов

        // TODO: расчет currentRoutes
        // берем текущий сегмент -> создаем сегмент с началом в этом сегменте и концом в текущем и заменяем последний в current

        // TODO: Установить паузу по достижению определенного момента
        // TODO: пауза должна остановить запуск этой функции
        if (false)
        {
            paused = !paused;
        }

        for (size_t i = 0; i != routes.size(); ++i)
        {
            // Создаем новый сегмент

            // Берем индекс текущего сегмента
            auto index = routes[i].getCurrentIndex();

            Segment segment{routes[i].getCurrentSegment().start /*взять сегмент индекса?*/, routes[i].getCurrentPosition(), routes[i].getCurrentSpeed()};

            currentRoutes[i].setSegment(index, segment);

            // std::cout << "[" << i << "]" << ":" << routes[i].getCurrentPosition().x() << ", " << routes[i].getCurrentPosition().y() << "[segment]" << segment.start.x() << ", " << segment.start.y() << ";";
        }

        // std::cout << std::endl;

        // TODO: Отрисовка currentRoutes если установлено значение
        if (showLines)
        {
            for (const auto& route : currentRoutes)
            {
                painter.setPen(QPen(route.getColor(), 2));
                for (const auto& seg : route.getSegments())
                {
                    QPointF p1 = cs.getTransform().map(seg.start);
                    QPointF p2 = cs.getTransform().map(seg.end);
                    painter.drawLine(p1, p2);
                }
            }
        }

        // if (showLines)
        // {
        //     for (const auto& route : routes)
        //     {
        //         painter.setPen(QPen(route.getColor(), 2));
        //         for (const auto& seg : route.getSegments())
        //         {
        //             QPointF p1 = cs.getTransform().map(seg.start);
        //             QPointF p2 = cs.getTransform().map(seg.end);
        //             painter.drawLine(p1, p2);
        //         }
        //     }
        // }

        // TODO: добавить на painter и удалять на каждом новом frame
        // Отрисовка двигущегося круга
        for (const auto& route : routes)
        {
            const QPointF pos = route.getCurrentPosition();

            const QPointF screenPos = cs.getTransform().map(pos); // Центр

            painter.setBrush(route.getColor());

            //
            qreal dx = 18;
            qreal dy = 18;
            painter.drawEllipse(screenPos, dx, dy);

            // Отрисовка данных текущей скорости
            painter.setPen(Qt::black);
            double multiplier = speedCombo->currentData().toDouble();

            painter.drawText(screenPos + QPointF(-15, -15),
                             QString::number(route.getCurrentSpeed() / multiplier, 'f', 1) + "x" +
                                 QString::number(multiplier, 'f', 0) + " = " +
                                 QString::number(route.getCurrentSpeed(), 'f', 1) + " px/s");
        }

        // Отрисовка информации крайних точек
        if (hoveredPoint.isValid)
        {
            painter.setPen(Qt::black);
            painter.setBrush(QColor(255, 255, 225));

            QRect tooltip_rect(hoveredPoint.screenPos + QPoint(10, 10), QSize(100, 50));
            painter.drawRect(tooltip_rect);

            painter.drawText(tooltip_rect, Qt::AlignCenter, QString("X: %1\nY: %2\n%3 px/s").arg(hoveredPoint.logicalPos.x(), 0, 'f', 1).arg(hoveredPoint.logicalPos.y(), 0, 'f', 1).arg(hoveredPoint.speed, 0, 'f', 1));
        }
    }

    void resizeEvent(QResizeEvent* event) override
    {
        speedButton->move(120, 10); // Фиксированная позиция

        speedCombo->move(10, 10);
        QWidget::resizeEvent(event);
    }

    void keyPressEvent(QKeyEvent* event) override
    {
        switch (event->key())
        {
        case Qt::Key_P:
            paused = !paused;
            break;
        case Qt::Key_S:
            setup(); // Полная переинициализация маршрутов
            // currentRoutes.clear();// TODO: к виду с одним сегментом
            update();
            break;
        case Qt::Key_L:
            showLines = !showLines;
            break;
        }
        update();
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        const QPointF mousePos = event->pos();

        double maxHeight = perimeter.getMaxY() - perimeter.getMinY();
        double maxWidth = perimeter.getMaxX() - perimeter.getMinX();

        CoordinateSystem cs(rect(), maxWidth, maxHeight);

        const QPointF logicalPos = cs.toLogical(mousePos); //

        hoveredPoint.isValid = false;

        const double pickRadius = 5.0;

        for (const auto& route : routes)
        {
            for (const auto& seg : route.getSegments())
            {
                for (const auto& point : {seg.start, seg.end})
                {
                    QPointF screenPoint = cs.getTransform().map(point);
                    if (QVector2D(screenPoint - mousePos).length() < pickRadius)
                    {
                        hoveredPoint = {
                            true,
                            screenPoint.toPoint(),
                            point,
                            seg.currentSpeed};
                        update();
                        return;
                    }
                }
            }
        }

        QWidget::mouseMoveEvent(event);
        update();
    }

private:
    struct HoveredPoint
    {
        bool isValid = false;
        QPoint screenPos;
        QPointF logicalPos;
        double speed;
    };

    // TODO: Обаратываются только положительные значения координат
    void drawGrid(QPainter& painter, const CoordinateSystem& cs)
    {
        painter.setPen(QColor(220, 220, 220));
        QFont font = painter.font();
        font.setPointSize(8);
        painter.setFont(font);

        // TODO: Определение шага
        const int gridSteps = 10;
        const QRect contentRect = cs.getContentRect();

        // Вертикальные линии (ось X)
        auto step = (perimeter.getMaxX() - perimeter.getMinX()) / gridSteps;

        double current = perimeter.getMinX();

        for (int i = 0; i <= gridSteps; ++i)
        {
            QPointF p1 = cs.getTransform().map(QPointF(current, perimeter.getMinY()));
            QPointF p2 = cs.getTransform().map(QPointF(current, perimeter.getMaxY()));
            painter.drawLine(p1, p2);

            // Подписи оси X (снизу)
            if (i > 0 && i < gridSteps)
            {
                painter.drawText(p1.x() - 15, contentRect.bottom() + 25, QString::number(current, 'f', 0));
            }
            current += step;
        }

        // Горизонтальные линии (ось Y)

        step = (perimeter.getMaxY() - perimeter.getMinY()) / gridSteps;

        current = perimeter.getMinY();

        for (int i = 0; i <= gridSteps; ++i)
        {
            QPointF p1 = cs.getTransform().map(QPointF(perimeter.getMinX(), current));
            QPointF p2 = cs.getTransform().map(QPointF(perimeter.getMaxX(), current));
            painter.drawLine(p1, p2);

            // Подписи оси Y (слева)
            if (i > 0 && i < gridSteps)
            {
                painter.drawText(contentRect.left() - 50, p1.y() + 5, QString::number(current, 'f', 0));
            }

            current += step;
        }

        // Подписи осей
        QPointF origin = cs.getTransform().map(QPointF(0, 0));

        painter.drawText(contentRect.right() - 30, origin.y() - 10, "X");
        painter.drawText(origin.x() + 10, contentRect.top() + 20, "Y");
    }

    void setup()
    {
        routes.clear();

        double multiplier = speedCombo->currentData().toDouble();

        Route route1;
        route1.addSegment({50, 50}, {300, 300}, 150 * multiplier);
        route1.addSegment({300, 300}, {100, 300}, 100 * multiplier);
        routes.push_back(route1);

        Route route2;
        route2.addSegment({300, 50}, {50, 200}, 80 * multiplier);
        route2.addSegment({50, 200}, {200, 350}, 120 * multiplier);
        routes.push_back(route2);

        // TODO: инициализация currentRoutes, добавить в конструктор после инициализации routes
        initCurrentRoutes();

        perimeter.addPolyline({{0, 0}, {1000, 0}, {350, 350}, {0, 350}, {0, 0}});
    }

    void initCurrentRoutes()
    {
        currentRoutes = std::vector<Route>(routes.size());
        //currentRoutes.reserve(routes.size());

        // for(const auto& route:routes){
        //     currentRoutes.push_back(Route(route.getSegments().size(), route.getColor()));
        // }

        for (size_t i = 0; i != routes.size(); ++i)
        {
            for (size_t j = 0; j != routes[i].getSegments().size(); ++j)
            {
                currentRoutes[i].addSegment(QPointF{}, QPointF{}, 0);
            }
        }
    }

public:
    void setRoutes(const std::vector<Route> r)
    {
        routes = r;

        initCurrentRoutes();
    }

    void setPerimeter(const Perimeter& p)
    {
        perimeter = p;
    }

private:
    QComboBox* speedCombo;
    QPushButton* speedButton;

    QTimer updateTimer;
    QElapsedTimer timer;

    std::vector<Route> currentRoutes;

    std::vector<Route> routes;
    Perimeter perimeter;
    HoveredPoint hoveredPoint;

    bool paused = false;
    bool showLines = true;
};
