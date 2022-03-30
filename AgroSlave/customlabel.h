#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>
#include <QMouseEvent>
#include <QDebug>


enum StateLabel {
    NONE_STATELABEL,
    PLAY_STATELABEL,
    STOP_STATELABEL,
};

class CustomLabel : public QLabel
{
    Q_OBJECT
public:
    explicit CustomLabel(QWidget* parent = nullptr);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;


    void setWindowGeometry(QRect &geometry);

    int getImgWidth() const;

    int getImgHeigth() const;

    StateLabel getState() const;
    void setState(StateLabel newState);

signals:
    void press(const QMouseEvent &event);
    void release(const QMouseEvent &event);
    void move(const QMouseEvent &event);

public slots:
    void setImage(const QImage &img);

private:
    int originalWidth, originalHeigth, imgWidth, imgHeigth;
    QRect *windowGeometry;

    StateLabel state;

};

#endif // CUSTOMLABEL_H
