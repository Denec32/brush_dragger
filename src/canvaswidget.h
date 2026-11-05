#pragma once

#include <QPolygonF>
#include <QWidget>

#include <vector>

#include "viewport.h"

// An infinite white canvas with a black pen.
//   left drag   - draw a stroke
//   middle drag - pan
//   wheel       - zoom around the cursor
// Strokes are stored in world coordinates, so they stay put under pan/zoom.
class CanvasWidget : public QWidget {
    Q_OBJECT

public:
    explicit CanvasWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;

private:
    // What the current drag is doing; at most one at a time.
    enum class Drag { None, Drawing, Panning };

    Viewport m_viewport;
    std::vector<QPolygonF> m_strokes;   // world coordinates; back() is live while Drawing
    Drag m_drag = Drag::None;
    QPointF m_lastPanPos;               // screen position of the previous pan step
};
