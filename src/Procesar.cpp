#include "Procesar.h"
#include <itkExtractImageFilter.h>
#include <itkImageRegionConstIterator.h>

Procesar::Procesar(const std::string &path) {
    // Definimos ReaderType para leer volúmenes 3D (float)
    using ReaderType = itk::ImageFileReader<ImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(path);

    try {
        reader->Update();
    }
    catch (itk::ExceptionObject &err) {
        throw std::runtime_error(err.GetDescription());
    }

    image = reader->GetOutput();
    numSlices = image->GetLargestPossibleRegion().GetSize()[2];
}

int Procesar::obtenerNumSlices() const {
    return numSlices;
}

cv::Mat Procesar::obtenerSlice(int index) {
    // Definimos el filtro de extracción (3D → 2D)
    using ExtractFilterType = itk::ExtractImageFilter<ImageType, itk::Image<float, 2>>;
    auto extractFilter = ExtractFilterType::New();

    // Regiones del volumen original
    auto region = image->GetLargestPossibleRegion();
    auto size   = region.GetSize();
    auto start  = region.GetIndex();

    // Para extraer un solo slice cambiamos size[2]=0 y ajustamos el índice Z
    size[2] = 0;
    start[2] = index;

    itk::Image<float, 3>::RegionType desiredRegion;
    desiredRegion.SetSize(size);
    desiredRegion.SetIndex(start);

    extractFilter->SetExtractionRegion(desiredRegion);
    extractFilter->SetInput(image);

    try {
        extractFilter->Update();
    }
    catch (itk::ExceptionObject &err) {
        throw std::runtime_error(err.GetDescription());
    }

    // Obtenemos el slice 2D
    using Image2DType = itk::Image<float, 2>;
    auto slice2D = extractFilter->GetOutput();

    // Creamos un cv::Mat de tipo CV_32F con el tamaño del slice
    int cols = static_cast<int>(slice2D->GetLargestPossibleRegion().GetSize()[0]);
    int rows = static_cast<int>(slice2D->GetLargestPossibleRegion().GetSize()[1]);
    cv::Mat mat(rows, cols, CV_32F);

    // Iterador para copiar cada píxel de ITK a cv::Mat
    itk::ImageRegionConstIterator<Image2DType> it(slice2D, slice2D->GetRequestedRegion());
    it.GoToBegin();
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            mat.at<float>(y, x) = it.Get();
            ++it;
        }
    }

    // Normalizamos a 8 bits (0–255) para poder visualizarlo
    double minVal, maxVal;
    cv::minMaxLoc(mat, &minVal, &maxVal);
    cv::Mat mat8;
    mat.convertTo(mat8, CV_8U,
                  255.0 / ( (maxVal - minVal) != 0 ? (maxVal - minVal) : 1 ),
                  -minVal * 255.0 / ( (maxVal - minVal) != 0 ? (maxVal - minVal) : 1 ));

    return mat8;
}
