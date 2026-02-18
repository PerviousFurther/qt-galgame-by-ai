#include <QApplication>
#include <QMainWindow>

#include "scene.h"
#include "item.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Qt Galgame Engine");
    window.resize(1280, 720);

    Scene scene(QStringLiteral("main_scene"), &window);
    Q_UNUSED(scene)

    window.show();

    return app.exec();
}
