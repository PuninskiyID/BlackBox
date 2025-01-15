#include "rotated_label.h"
#include <QPainter>
#include <QFontMetrics>
#include <QtMath>

RotatedLabel::RotatedLabel(const QString &text, QWidget *parent)
    : QWidget(parent), m_text(text), m_rotationAngle(0) {}

void RotatedLabel::setText(const QString &text) {
    m_text = text;
    update();
}

void RotatedLabel::setRotationAngle(qreal angle) {
    m_rotationAngle = angle;
    update();
}

void RotatedLabel::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    // Translate to center of the widget
    painter.translate(width() / 2.0, height() / 2.0);
    painter.rotate(m_rotationAngle);
    painter.translate(-width() / 2.0, -height() / 2.0);

    // Draw text centered within the widget's rectangle
    QFont font = painter.font();
    font.setPointSize(10); // Adjust font size if needed
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, m_text);
}



QSize RotatedLabel::sizeHint() const {
    QFontMetrics metrics(font());
    int textWidth = metrics.horizontalAdvance(m_text);
    int textHeight = metrics.height();

    // Calculate bounding rectangle for rotated text
    qreal angleRadians = qDegreesToRadians(m_rotationAngle);
    int rotatedWidth = static_cast<int>(qAbs(textWidth * qCos(angleRadians)) + qAbs(textHeight * qSin(angleRadians)));
    int rotatedHeight = static_cast<int>(qAbs(textWidth * qSin(angleRadians)) + qAbs(textHeight * qCos(angleRadians)));

    return QSize(rotatedWidth, rotatedHeight);
}

QSize RotatedLabel::minimumSizeHint() const {
    QFontMetrics metrics(font());
    int textWidth = metrics.horizontalAdvance(m_text);
    int textHeight = metrics.height();

    // Provide a smaller bounding rectangle for minimal constraints
    return QSize(textWidth, textHeight);
}

