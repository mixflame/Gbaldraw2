#ifndef SCRIBBLEAREA_H
#define SCRIBBLEAREA_H

#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

class ScribbleArea : public QWidget {
    Q_OBJECT

public:
    ScribbleArea(QWidget *parent = 0);
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);

    bool isModified() const { return modified; }
    QColor penColor() const {return myPenColor;}
    int penWidth() const {return myPenWidth;}

    // store all points if needed
    QJsonArray points;

    QHash<QString, int> nameHash;
    QStringList layerOrder;
    QJsonObject layers;

    QString username; // our own username

public slots:
    void clearImage();
    void print();
    void addClick(int x, int y, bool dragging, int r, int g, int b, int width, QString clickName);
    void redraw();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawLineTo(const QPoint &fromPoint, const QPoint &endPoint, QColor penColor, int penWidth);
    void resizeImage(QImage *image, const QSize &newSize);
    bool modified;
    bool scribbling;
    QColor myPenColor;
    int myPenWidth;
    QImage image;
    QPoint lastPoint;


};

#endif // SCRIBBLEAREA_H
