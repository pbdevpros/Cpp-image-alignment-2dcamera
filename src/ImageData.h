/// ImageData.h
/// Creator: Pádraig Basquel

#ifndef IMAGEDATA
#define IMAGEDATA

#include "Logger.h"
#include <string>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

/**
 * Processes and saves images.
 */

class ImageData
{
  public:
    ImageData();
    ImageData(std::string filepath) : filepath_(filepath) {}
    void setFilename(std::string filepath) { filepath_ = filepath ; }  
    void setLogger(Logger &log) { log_= &log ; }

    std::string getFilename() { return filepath_ ; } 
    std::vector<std::vector<float>> getCorners();
    double getVerticalAlignment() { return v_alignment_ ; }
    double getHorizontalAlignment() { return h_alignment_ ; }

    bool readImage();
    bool readImage(std::string filepath);
    bool saveImage(std::string filepath);
    bool findContours();
    bool findCorners();
    bool findAlignment();
    bool displayImage(bool isOutputImage);

  private:
    cv::Mat image_;                     // input (original) image
    cv::Mat op_image_;                  // output image
    std::vector<cv::Point> contours_;   // co-ordinates of points along a contour of an object in the image. Null until findContours is used.
    std::vector<cv::Point2f> corners_;  // co-ordinates of the corners of an object in the image. The order is 0: top left, 1: top right, 2: bottom left, 3: bottom right.
    double h_alignment_, v_alignment_;  // horizontal and vertical alignment.
    std::string filepath_;
    Logger* log_;
    
};

#endif
