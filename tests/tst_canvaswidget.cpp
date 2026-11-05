#include <QTest>
#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>

#include "canvaswidget.h"

namespace {

// Sends input the way the windowing system would, so the widget is driven
// exactly as a user drives it.
void mouse(QWidget &w, QEvent::Type type, Qt::MouseButton button,
           Qt::MouseButtons held, QPointF pos) {
    QMouseEvent ev(type, pos, w.mapToGlobal(pos), button, held, Qt::NoModifier);
    QApplication::sendEvent(&w, &ev);
}

void drag(QWidget &w, Qt::MouseButton button, QPointF from, QPointF to) {
    mouse(w, QEvent::MouseButtonPress, button, button, from);
    constexpr int steps = 10;
    for (int i = 1; i <= steps; ++i)
        mouse(w, QEvent::MouseMove, Qt::NoButton, button,
              from + (to - from) * (double(i) / steps));
    mouse(w, QEvent::MouseButtonRelease, button, Qt::NoButton, to);
}

void click(QWidget &w, QPointF at) {
    mouse(w, QEvent::MouseButtonPress, Qt::LeftButton, Qt::LeftButton, at);
    mouse(w, QEvent::MouseButtonRelease, Qt::LeftButton, Qt::NoButton, at);
}

void wheelUp(QWidget &w, QPointF at, int notches) {
    QWheelEvent ev(at, w.mapToGlobal(at), QPoint(), QPoint(0, 120 * notches),
                   Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(&w, &ev);
}

bool isInk(const QWidget &w, QPoint at) {
    const QImage img = const_cast<QWidget &>(w).grab().toImage();
    return qGray(img.pixel(at)) < 128;
}

} // namespace

class TstCanvasWidget : public QObject {
    Q_OBJECT

private slots:
    void blankCanvasIsAllWhite() {
        CanvasWidget w;
        w.resize(120, 80);
        const QImage img = w.grab().toImage();
        for (int y = 0; y < img.height(); ++y)
            for (int x = 0; x < img.width(); ++x)
                QCOMPARE(img.pixel(x, y), qRgb(255, 255, 255));
    }

    void leftDragDrawsBlackLine() {
        CanvasWidget w;
        w.resize(200, 200);
        drag(w, Qt::LeftButton, {20, 50}, {180, 50});
        QVERIFY(isInk(w, {100, 50}));
        QVERIFY(!isInk(w, {100, 100}));
    }

    void clickLeavesDot() {
        CanvasWidget w;
        w.resize(200, 200);
        click(w, {60, 70});
        QVERIFY(isInk(w, {60, 70}));
    }

    void separateDragsAreSeparateStrokes() {
        CanvasWidget w;
        w.resize(200, 200);
        drag(w, Qt::LeftButton, {20, 40}, {60, 40});
        drag(w, Qt::LeftButton, {140, 160}, {180, 160});
        QVERIFY(isInk(w, {40, 40}));
        QVERIFY(isInk(w, {160, 160}));
        // No ink joining the end of one stroke to the start of the next.
        QVERIFY(!isInk(w, {100, 100}));
    }

    void middleDragPansInk() {
        CanvasWidget w;
        w.resize(200, 200);
        drag(w, Qt::LeftButton, {20, 50}, {180, 50});
        drag(w, Qt::MiddleButton, {100, 100}, {100, 140});
        QVERIFY(!isInk(w, {100, 50}));
        QVERIFY(isInk(w, {100, 90}));
    }

    void middleDragDoesNotDraw() {
        CanvasWidget w;
        w.resize(200, 200);
        drag(w, Qt::MiddleButton, {20, 100}, {180, 100});
        QVERIFY(!isInk(w, {100, 100}));
    }

    void canvasExtendsBeyondTheWindow() {
        CanvasWidget w;
        w.resize(200, 200);
        // Pan far away, draw there, pan back: ink drawn off the original
        // view is kept and comes back into view.
        drag(w, Qt::MiddleButton, {190, 100}, {10, 100});
        drag(w, Qt::MiddleButton, {190, 100}, {10, 100});
        drag(w, Qt::LeftButton, {80, 100}, {120, 100});
        QVERIFY(isInk(w, {100, 100}));
        drag(w, Qt::MiddleButton, {10, 100}, {190, 100});
        drag(w, Qt::MiddleButton, {10, 100}, {190, 100});
        QVERIFY(!isInk(w, {100, 100}));
        drag(w, Qt::MiddleButton, {190, 100}, {10, 100});
        drag(w, Qt::MiddleButton, {190, 100}, {10, 100});
        QVERIFY(isInk(w, {100, 100}));
    }

    void wheelZoomsAroundCursor() {
        CanvasWidget w;
        w.resize(300, 300);
        click(w, {100, 100});   // dot under the cursor
        click(w, {150, 150});   // dot away from it
        wheelUp(w, {100, 100}, 4);
        // The point under the cursor stays put; everything else moves away.
        QVERIFY(isInk(w, {100, 100}));
        QVERIFY(!isInk(w, {150, 150}));
    }

    void drawingAfterZoomLandsUnderCursor() {
        CanvasWidget w;
        w.resize(300, 300);
        wheelUp(w, {50, 50}, 3);
        drag(w, Qt::MiddleButton, {200, 200}, {170, 230});
        click(w, {222, 111});
        QVERIFY(isInk(w, {222, 111}));
    }
};

QTEST_MAIN(TstCanvasWidget)
#include "tst_canvaswidget.moc"
