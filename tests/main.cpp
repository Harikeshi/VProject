#include <QApplication>

#include <Project/VisualizationWidget.hpp>
#include <Project/MainWindow.hpp>

// TODO: Не дорисовывает конец маршрута при большой скорости, из-за округления
int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.resize(800, 600);
    mainWindow.show();

    return app.exec();

}
