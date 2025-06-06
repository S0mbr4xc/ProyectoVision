#ifndef PROCESAR_H
#define PROCESAR_H

#include <string>
#include <opencv2/opencv.hpp>
#include <itkImage.h>
#include <itkImageFileReader.h>

using ImageType = itk::Image<float, 3>;

class Procesar {
public:
    // Constructor: abre un archivo .nii y carga el volumen
    Procesar(const std::string &path);

    // Devuelve la cantidad de slices (dimensión Z) del volumen
    int obtenerNumSlices() const;

    // Extrae el slice número 'index' y lo convierte a cv::Mat en 8 bits
    cv::Mat obtenerSlice(int index);

private:
    typename ImageType::Pointer image; // puntero al volumen 3D en ITK
    int numSlices;                     // número de cortes (Z)
};

#endif // PROCESAR_H
