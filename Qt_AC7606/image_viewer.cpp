#include "image_viewer.h"

Image_viewer::Image_viewer(QString s_file_name) : QImage(s_file_name)
{
    p_gray_image = std::make_shared<QImage>(Image_viewer::convertToFormat(QImage::Format_Grayscale8));
}

Image_viewer::~Image_viewer()
{

}

QByteArray Image_viewer::Gray_Image_to_Array()
{
    QByteArray a_block_data;
    QDataStream ds_out(&a_block_data, QIODevice::WriteOnly);
    ds_out.setVersion(QDataStream::Qt_5_11);

    ds_out << qint64(0);
    ds_out << qint8(49);
    ds_out.writeBytes((char*)this->p_gray_image->bits(), this->p_gray_image->width() * this->p_gray_image->height());
    ds_out.device()->seek(0);
    ds_out << (qint64)(a_block_data.size() - sizeof(qint64));

    return a_block_data;
}
