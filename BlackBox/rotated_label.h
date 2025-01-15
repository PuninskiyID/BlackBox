#ifndef ROTATEDLABEL_H
#define ROTATEDLABEL_H

#include <QWidget>
#include <QString>

class RotatedLabel : public QWidget {
    Q_OBJECT

public:
    explicit RotatedLabel(const QString &text = "", QWidget *parent = nullptr);
    void setText(const QString &text);
    void setRotationAngle(qreal angle);
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
    qreal m_rotationAngle; // Angle in degrees
};

#endif // ROTATEDLABEL_H
