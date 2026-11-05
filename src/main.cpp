#include <QApplication>

#include "canvaswidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CanvasWidget canvas;
    canvas.setWindowTitle("brush_dragger");
    canvas.resize(1280, 800);
    canvas.show();
    return QApplication::exec();
}
