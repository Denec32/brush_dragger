#include <QTest>

#include "viewport.h"

class TstViewport : public QObject {
    Q_OBJECT

private slots:
    void defaultViewportIsIdentity() {
        const Viewport vp;
        QCOMPARE(vp.toScreen({12.5, -7}), QPointF(12.5, -7));
        QCOMPARE(vp.toWorld({12.5, -7}), QPointF(12.5, -7));
    }

    void panShiftsWhereWorldAppears() {
        Viewport vp;
        vp.panBy({30, -10});
        QCOMPARE(vp.toScreen({0, 0}), QPointF(30, -10));
        QCOMPARE(vp.toWorld({30, -10}), QPointF(0, 0));
    }

    void zoomAtKeepsAnchorFixed() {
        Viewport vp;
        vp.panBy({40, 25});
        const QPointF anchor(100, 80);
        const QPointF worldUnderAnchor = vp.toWorld(anchor);

        vp.zoomAt(anchor, 2.0);

        QCOMPARE(vp.zoom(), 2.0);
        QCOMPARE(vp.toScreen(worldUnderAnchor), anchor);
    }

    void toWorldInvertsToScreen() {
        Viewport vp;
        vp.panBy({-13, 7});
        vp.zoomAt({50, 50}, 3.0);
        const QPointF world(123.25, -456.5);
        QCOMPARE(vp.toWorld(vp.toScreen(world)), world);
    }

    void zoomIsClampedToSaneRange() {
        Viewport vp;
        vp.zoomAt({0, 0}, 1e-9);
        QCOMPARE(vp.zoom(), Viewport::kMinZoom);
        vp.zoomAt({0, 0}, 1e12);
        QCOMPARE(vp.zoom(), Viewport::kMaxZoom);
    }
};

QTEST_APPLESS_MAIN(TstViewport)
#include "tst_viewport.moc"
