#pragma once

#include <QPointF>

#include <algorithm>

// Maps between world coordinates (the unbounded canvas, where strokes live)
// and screen coordinates (widget pixels):  screen = world * zoom + pan.
// zoom is always within [kMinZoom, kMaxZoom], so the mapping is invertible.
class Viewport {
public:
    static constexpr double kMinZoom = 0.05;
    static constexpr double kMaxZoom = 50.0;

    double zoom() const { return m_zoom; }

    QPointF toScreen(QPointF world) const { return world * m_zoom + m_pan; }
    QPointF toWorld(QPointF screen) const { return (screen - m_pan) / m_zoom; }

    void panBy(QPointF screenDelta) { m_pan += screenDelta; }

    // Scales by factor while keeping the world point under `anchor` in place.
    void zoomAt(QPointF anchor, double factor) {
        const QPointF world = toWorld(anchor);
        m_zoom = std::clamp(m_zoom * factor, kMinZoom, kMaxZoom);
        m_pan = anchor - world * m_zoom;
    }

private:
    double m_zoom = 1.0;
    QPointF m_pan;
};
