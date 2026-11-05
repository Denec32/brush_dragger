#include "canvaswidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

#include <cmath>

namespace {
constexpr double kPenWidth = 3.0;          // world units, so ink scales with zoom
// angleDelta is 120 units per wheel notch, so one notch zooms ~1.2x.
constexpr double kZoomPerWheelUnit = 1.0015;
} // namespace

CanvasWidget::CanvasWidget(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_OpaquePaintEvent);  // we paint every pixel ourselves
    setCursor(Qt::CrossCursor);
}

void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
    p.setRenderHint(QPainter::Antialiasing);

    p.translate(m_viewport.toScreen({0, 0}));
    p.scale(m_viewport.zoom(), m_viewport.zoom());
    p.setPen(QPen(Qt::black, kPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    for (const QPolygonF &stroke : m_strokes) {
        if (stroke.size() == 1)
            p.drawPoint(stroke.front());   // a click: round cap makes it a dot
        else
            p.drawPolyline(stroke);
    }
}

void CanvasWidget::mousePressEvent(QMouseEvent *e) {
    if (m_drag != Drag::None)
        return;   // ignore a second button mid-drag
    if (e->button() == Qt::LeftButton) {
        m_drag = Drag::Drawing;
        m_strokes.emplace_back(QList{m_viewport.toWorld(e->position())});
        update();
    } else if (e->button() == Qt::MiddleButton) {
        m_drag = Drag::Panning;
        m_lastPanPos = e->position();
        setCursor(Qt::ClosedHandCursor);
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *e) {
    switch (m_drag) {
    case Drag::Drawing:
        m_strokes.back() << m_viewport.toWorld(e->position());
        break;
    case Drag::Panning:
        m_viewport.panBy(e->position() - m_lastPanPos);
        m_lastPanPos = e->position();
        break;
    case Drag::None:
        return;
    }
    update();
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent *e) {
    const bool endsDrag =
        (m_drag == Drag::Drawing && e->button() == Qt::LeftButton) ||
        (m_drag == Drag::Panning && e->button() == Qt::MiddleButton);
    if (!endsDrag)
        return;
    if (m_drag == Drag::Panning)
        setCursor(Qt::CrossCursor);
    m_drag = Drag::None;
}

void CanvasWidget::wheelEvent(QWheelEvent *e) {
    m_viewport.zoomAt(e->position(), std::pow(kZoomPerWheelUnit, e->angleDelta().y()));
    update();
}
