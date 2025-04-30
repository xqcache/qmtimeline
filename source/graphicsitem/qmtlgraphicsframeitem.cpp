#include "qmtlgraphicsframeitem.h"
#include "itemmodel/qmtlframeitemmodel.h"
#include "qmtlgraphicsmodel.h"
#include "qmtlgraphicsscene.h"
#include <QPainter>

struct QmTLGraphicsFrameItemPrivate { };

QmTLGraphicsFrameItem::QmTLGraphicsFrameItem(QmTLGraphicsScene& scene, QmTLItemID item_id)
    : QmTLGraphicsItem(scene, item_id)
    , d_(new QmTLGraphicsFrameItemPrivate)
{
}

QmTLGraphicsFrameItem::~QmTLGraphicsFrameItem() noexcept
{
    delete d_;
}

QRectF QmTLGraphicsFrameItem::boundingRect() const
{
    if (!graphModel()) [[unlikely]] {
        return {};
    }
    auto* item_model = graphModel()->itemModel<QmTLFrameItemModel>(item_id_);
    if (!item_model) [[unlikely]] {
        return {};
    }
    auto delay = item_model->data<QmTLFrameItemData>().delay();
    qreal tick_pixels = graphScene().axisTickPixels();
    return QRectF(-tick_pixels / 2.0, 0, delay.has_value() ? graphScene().mapToAxis(*delay) + tick_pixels : tick_pixels,
        graphScene().itemHeight(item_id_));
}

void QmTLGraphicsFrameItem::fitInAxis()
{
    if (!graphModel()) [[unlikely]] {
        return;
    }
    auto* item_model = static_cast<QmTLFrameItemModel*>(graphModel()->itemModel(item_id_));
    if (!item_model) [[unlikely]] {
        return;
    }
    setX(graphScene().mapToAxisX(item_model->data().timeKey()));
}

void QmTLGraphicsFrameItem::drawDelay(QPainter* painter)
{
    if (!graphModel()) [[unlikely]] {
        return;
    }
    auto* item_model = graphModel()->itemModel<QmTLFrameItemModel>(item_id_);
    if (!item_model) [[unlikely]] {
        return;
    }

    auto& item_data = item_model->data<QmTLFrameItemData>();
    const auto& delay = item_data.delay();
    if (!delay) {
        return;
    }
    qreal tick_pixels = graphScene().axisTickPixels();
    qreal delay_pixels = graphScene().mapToAxis(*delay);
    qreal spacing_begin = boundingRect().left() + tick_pixels;
    qreal spacing_end = delay_pixels - tick_pixels / 2.0;
    qreal center_y = boundingRect().height() / 2.0;

    QRectF boundary(boundingRect().left(), 0, tick_pixels, boundingRect().height());
    boundary.moveLeft(delay_pixels - tick_pixels / 2.0);

    {
        // 绘制delay frame矩形
        painter->drawRoundedRect(boundary, 2, 2);
    }

    painter->setPen("#006064");
    qreal triangle_edge = qMax(center_y * 0.15, 5.0);
    {
        // 绘制start frame右侧小三角
        painter->save();
        painter->setPen(Qt::white);
        QPainterPath path;
        path.moveTo(spacing_begin + triangle_edge, center_y);
        path.lineTo(spacing_begin, center_y - triangle_edge);
        path.lineTo(spacing_begin, center_y + triangle_edge);
        path.lineTo(spacing_begin + triangle_edge, center_y);
        painter->drawPath(path);
        painter->restore();
    }

    QString label = "Delay";
    QRectF label_rect = painter->fontMetrics().boundingRect(label);
    label_rect.setWidth(label_rect.width() + 10);
    qreal label_max_width = delay_pixels - triangle_edge * 2 - tick_pixels;
    {
        // 绘制Label
        if (label_rect.width() < label_max_width) {
            label_rect.moveTop((boundary.height() - label_rect.height()) / 2.0);
            label_rect.moveLeft(spacing_begin + triangle_edge + (label_max_width - label_rect.width()) / 2.0);
            painter->drawText(label_rect, Qt::AlignCenter, label);
        }
    }

    {
        // 绘制start和delay frame中间的虚线
        painter->save();
        QPen pen = painter->pen();
        pen.setStyle(Qt::DashLine);
        pen.setCapStyle(Qt::RoundCap);
        painter->setPen(pen);
        if (label_rect.width() < label_max_width) {
            painter->drawLine(spacing_begin + triangle_edge, center_y, label_rect.left(), center_y);
            painter->drawLine(label_rect.right(), center_y, spacing_end, center_y);
        } else {
            painter->drawLine(spacing_begin + triangle_edge, center_y, spacing_end, center_y);
        }
        painter->restore();
    }

    {
        // 绘制delay frame左侧的小三角
        painter->save();
        painter->setPen(Qt::white);
        QPainterPath path;
        path.moveTo(spacing_end - triangle_edge, center_y);
        path.lineTo(spacing_end, center_y - triangle_edge);
        path.lineTo(spacing_end, center_y + triangle_edge);
        path.lineTo(spacing_end - triangle_edge, center_y);
        painter->drawPath(path);
        painter->restore();
    }
}

void QmTLGraphicsFrameItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->setPen(Qt::white);
    painter->setBrush(QColor("#006064"));

    QRectF bounding_rect = boundingRect();
    QRectF boundary(bounding_rect.left(), 0, graphScene().axisTickPixels(), bounding_rect.height());
    painter->drawRoundedRect(boundary, 2, 2);

    drawDelay(painter);
}