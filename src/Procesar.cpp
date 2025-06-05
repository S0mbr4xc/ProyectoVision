#include "Procesar.h"
#include <itkExtractImageFilter.h>
#include <itkImageRegionConstIterator.h>

Procesar::Procesar(const std::string &path) {
    using ReaderType = itk::ImageFileReader<ImageType>;
    auto reader = ReaderType::New();
    reader->SetFileName(path);
    reader->Update();
    image = reader->GetOutput();
    numSlices = image->GetLargestPossibleRegion().GetSize()[2];
}

int Procesar::obtenerNumSlices() const {
    return numSlices;
}

cv::Mat Procesar::obtenerSlice(int index) {
    using ExtractFilterType = itk::ExtractImageFilter<ImageType, itk::Image<float, 2>>;
    auto extractFilter = ExtractFilterType::New();
    auto region = image->GetLargestPossibleRegion();
    auto size   = region.GetSize(); size[2] = 0;
    auto start  = region.GetIndex();    start[2] = index;
    itk::Image<float, 3>::RegionType desiredRegion; desiredRegion.SetSize(size); desiredRegion.SetIndex(start);

    extractFilter->SetExtractionRegion(desiredRegion);
    extractFilter->SetInput(image);
    extractFilter->Update();

    using Image2DType = itk::Image<float, 2>;
    auto slice2D = extractFilter->GetOutput();
    int cols = slice2D->GetLargestPossibleRegion().GetSize()[0];
    int rows = slice2D->GetLargestPossibleRegion().GetSize()[1];
    cv::Mat mat(rows, cols, CV_32F);
    itk::ImageRegionConstIterator<Image2DType> it(slice2D, slice2D->GetRequestedRegion());
    it.GoToBegin();
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x, ++it)
            mat.at<float>(y, x) = it.Get();
    double minVal, maxVal;
    cv::minMaxLoc(mat, &minVal, &maxVal);
    cv::Mat mat8;
    mat.convertTo(mat8, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
    return mat8;
}
