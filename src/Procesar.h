#ifndef PROCESAR_H
#define PROCESAR_H

#include <string>
#include <opencv2/opencv.hpp>
#include <itkImage.h>
#include <itkImageFileReader.h>

class Procesar {
public:
    explicit Procesar(const std::string &path);
    cv::Mat obtenerSlice(int index);
    int obtenerNumSlices() const;

private:
    using ImageType = itk::Image<float, 3>;
    ImageType::Pointer image;
    int numSlices;
};

#endif // PROCESAR_H
