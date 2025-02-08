#pragma once

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QWidget>

#include "JsonOper.hpp"
#include "VisualizationWidget.hpp"
#include <nlohmann/json.hpp>

// TODO: Переделать на QJson

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent)
    {
        QMenu* fileMenu = menuBar()->addMenu("Файл");
        QAction* openRequest = new QAction("Загрузить Request.json", this);
        QAction* openResult = new QAction("Загрузить Result.json", this);
        fileMenu->addAction(openRequest);
        fileMenu->addAction(openResult);

        visualizationWidget = new VisualizationWidget;
        setCentralWidget(visualizationWidget);

        connect(openRequest, &QAction::triggered, this, &MainWindow::initPerimeter);
        connect(openResult, &QAction::triggered, this, &MainWindow::initRoutes);
    }

    // TODO: информация о загрузке файлов
private slots:
    std::string openJsonFile() // TODO:
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Открыть JSON файл", "", "JSON Files (*.json)");

        std::string str;

        if (!fileName.isEmpty())
        {
            QFile file(fileName);

            // Проверка
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл");
                return {};
            }

            QTextStream in(&file);
            str = file.readAll().toStdString();

            file.close();
        }

        return str;
    }

    nlohmann::json stringToJson(const std::string& str)
    {
        nlohmann::json json;
        try
        {
            json = nlohmann::json::parse(str);
        }
        catch (...)
        {
            throw std::runtime_error("Невозможно обработать Json.");
        }

        return json;
    }

    void initRoutes() // TODO:
    {
        nlohmann::json json;
        try
        {
            std::string str = openJsonFile();
            json = stringToJson(str);
        }
        catch (std::runtime_error& ex)
        {
            QMessageBox::warning(this, "Ошибка", ex.what());

            return;
        }

        try
        {
            std::vector<Route> routes = Json::parseToRoutes(json);

            visualizationWidget->setRoutes(routes);
        }
        catch (std::runtime_error& ex)
        {
            // TODO: Обработка ошибки

            QMessageBox::warning(this, "Ошибка", ex.what());

            return;
        }

        routesLoaded = true;

        QMessageBox::information(this, "JSON файл", "JSON файл успешно загружен.");
    }

    void initPerimeter() // TODO:
    {
        nlohmann::json json;
        try
        {
            std::string str = openJsonFile();
            json = stringToJson(str);
        }
        catch (std::runtime_error& ex)
        {
            QMessageBox::warning(this, "Ошибка", ex.what());

            return;
        }

        try
        {
            Perimeter perimeter = Json::parseToPolygon(json);

            visualizationWidget->setPerimeter(perimeter);
        }
        catch (std::runtime_error& ex)
        {
            QMessageBox::warning(this, "Ошибка", ex.what());
            return;
        }

        perimeterLoaded = true; // TODO:

        QMessageBox::information(this, "JSON файл", "JSON файл успешно загружен.");
    }

private:
    bool perimeterLoaded = false;
    bool routesLoaded = false;

    // Должны хранить данные кораблей

    VisualizationWidget* visualizationWidget;
};
