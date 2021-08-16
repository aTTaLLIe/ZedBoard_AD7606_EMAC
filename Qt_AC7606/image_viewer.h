#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H

#include <QImage>
#include <memory>
#include "qcustomplot.h"

class Image_viewer : public QImage
{
public:
    Image_viewer(QString);
    ~Image_viewer();

    std::shared_ptr<QImage> p_gray_image;

    QByteArray Gray_Image_to_Array();
};

#endif // IMAGE_VIEWER_H
