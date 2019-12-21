#include <QtWidgets>
#if defined (QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#endif
#endif

#include <scribblearea.h>

ScribbleArea::ScribbleArea(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    modified = false;
    scribbling = false;
    myPenWidth = 1;
    myPenColor = Qt::blue;
}

bool ScribbleArea::openImage(const QString &fileName){
    QImage loadedImage;
    if(!loadedImage.load(fileName)){
        return false;
    }
    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}

bool ScribbleArea::saveImage(const QString &fileName, const char *fileFormat){
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());
    if(visibleImage.save(fileName, fileFormat)){
        modified = false;
        return true;
    } else {
        return false;
    }
}

void ScribbleArea::setPenColor(const QColor &newColor){
    myPenColor = newColor;
}

void ScribbleArea::setPenWidth(int newWidth){
    myPenWidth = newWidth;
}

void ScribbleArea::clearImage() {
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}

void ScribbleArea::addClick(int x, int y, bool dragging, int r, int g, int b, int width, QString clickName) {
    // old way
    QJsonObject point;
    point["x"] = x;
    point["y"] = y;
    point["dragging"] = dragging;
    point["r"] = r;
    point["g"] = g;
    point["b"] = b;
    point["width"] = width;
    point["username"] = clickName;

    QString layerName;

    if(!nameHash.contains(clickName)){
        int layer = 0;
        nameHash[clickName] = layer;
        layerName = clickName + "_" + QString::number(layer);
        QJsonArray layerArray;
        layers[layerName] = layerArray;

    } else {
        if(dragging == false){
            int layer = nameHash[clickName] + 1;
            nameHash[clickName] = layer;
            layerName = clickName + "_" + QString::number(layer);
            QJsonArray layerArray;
            layers[layerName] = layerArray;
        } else {
            int layer = nameHash[clickName];
            layerName = clickName + "_" + QString::number(layer);
        }
    }

    QJsonArray tempLayers = layers[layerName].toArray();
    tempLayers.append(point);
    layers[layerName] = tempLayers;

    if(!layerOrder.contains(layerName))
          layerOrder.append(layerName);

    qDebug() << layerName;
    qDebug() << QString::number(layerOrder.size());
}

void ScribbleArea::redraw() {
    this->clearImage();
    QColor oldColor = myPenColor;
    int oldWidth = myPenWidth;
    for(int j=0; j < layerOrder.size(); j++) {
        //qDebug() << QString::number(j);
        QString layer = layerOrder[j];
        QJsonArray layerArray = layers[layer].toArray();
        for(int i = 1; i < layerArray.size(); i++) {
            QJsonObject lastObj = layerArray[i - 1].toObject();
            QJsonObject thisObj = layerArray[i].toObject();
            QPoint lastPoint(lastObj["x"].toInt(), lastObj["y"].toInt());
            QPoint endPoint(thisObj["x"].toInt(), thisObj["y"].toInt());
            if(thisObj["dragging"].toBool() && lastObj["dragging"].toBool()) {
                myPenColor = QColor(lastObj["r"].toInt(), lastObj["g"].toInt(), lastObj["b"].toInt());
                myPenWidth = lastObj["width"].toInt();
                drawLineTo(lastPoint, endPoint);
            }
        }
    }
    myPenColor = oldColor;
    myPenWidth = oldWidth;
}

void ScribbleArea::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        scribbling = true; // mouse pressed
        // add click to json array
        addClick(lastPoint.x(), lastPoint.y(), false, myPenColor.red(), myPenColor.green(), myPenColor.blue(), myPenWidth, username);

        // todo: send point over network
    }
}

void ScribbleArea::mouseMoveEvent(QMouseEvent *event){
    if((event->buttons() & Qt::LeftButton) && scribbling) {
        QPoint endPoint = event->pos();
        //drawLineTo(lastPoint, endPoint);
        lastPoint = endPoint;

        addClick(lastPoint.x(), lastPoint.y(), true, myPenColor.red(), myPenColor.green(), myPenColor.blue(), myPenWidth, username);

        redraw();
    }
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::LeftButton && scribbling) {
        QPoint endPoint = event->pos();
        //drawLineTo(lastPoint, endPoint);
        lastPoint = endPoint;
        scribbling = false;
    }
}

void ScribbleArea::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}

void ScribbleArea::resizeEvent(QResizeEvent *event){
    if(width() > image.width() || height() > image.height()){
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}

void ScribbleArea::drawLineTo(const QPoint &fromPoint, const QPoint &endPoint){
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(fromPoint, endPoint);
    modified = true;
    int rad = (myPenWidth / 2) + 2;
    update(QRect(fromPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
}

void ScribbleArea::resizeImage(QImage *image, const QSize &newSize){
    if(image->size() == newSize){
        return;
    }

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}

void ScribbleArea::print(){
#if QT_CONFIG(printdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer, this);
    if(printDialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = image.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(image.rect());
        painter.drawImage(0, 0, image);
    }
#endif
}
