/// ImageData.cpp
/// Creator: Pádraig Basquel

#include "ImageData.h"
#define PI 3.14159265

ImageData::ImageData()
{
    filepath_ = "";
    v_alignment_ = 100.0; // initialize to a large degree, to alert the user this must be updated
    h_alignment_ = 100.0;
}

/// @brief Return the corners of an image. This data does not exist until findCorners is run.
std::vector<std::vector<float>> ImageData::getCorners() 
{
    if ( corners_.empty() )
    {
        log_->log(ERROR, "Cannot return corners. Corners of the object in the image file have not been found, as findCorners has not been run.");
        return std::vector<std::vector<float >> (0.00f) ;
    }

    std::vector<std::vector<float> > opCorners ;
    try {
        for ( auto const &points : corners_ )
            opCorners.emplace_back(std::vector<float> { points.x, points.y });
    } catch ( ... ) {
        throw std::runtime_error ("Error! Could not return corners found.");
    }

    return opCorners;
}

/// @brief Reads in an image and saves it.
bool ImageData::readImage()
{
    if( filepath_.empty() )
    {
        log_->log(ERROR, "Could not open or find the image. Filepath not given.");
        return false;
    }
    
    image_ = cv::imread(filepath_, cv::IMREAD_COLOR);
    return true;
}

/// @brief Read in an image.
/// @param filepath Filepath of the image to be read in.
bool ImageData::readImage(std::string filepath)
{
    image_ = cv::imread(filepath, cv::IMREAD_COLOR);
    if( image_.empty() )
    {
        log_->log(ERROR, "Could not open or find the image");
        return false;
    }
    return true;
}

/// @brief Find the contours of an object in an image.
/// @details Expects an image of an object which is illuminated in darkness. Searches for a single contour of a large area, 
///          requiring the object to take up a large portion of the image. The vector points which make up the contour 
///          found are stored in a member variable.
bool ImageData::findContours()
{
    if( image_.empty() )
    {
        log_->log(ERROR, "Cannot find the contours of the desired image. Image data is empty.");
        return false;
    }

    // convert to another color space
    cv::Mat hsv;
    cv::cvtColor(image_, hsv, cv::COLOR_BGR2HSV);

    // Creating masks to detect the upper and lower red color.
    cv::Mat mask1,mask2;
    int hsv_val = 0;                    // 0 = Red, 60 = Green, 150 = Purple, 120 = Blue
    int sensitity = 10;
    int sat_min_threshhold = 120;       // 120 = red, green
    int val_min_threshold = 120;        // 70 = red, green
    int sat_max_threshold = 255;        // 255 = red, green
    int val_max_threshold = 255;        // 255 = red, green
    cv::inRange(hsv, cv::Scalar(hsv_val, sat_min_threshhold, val_min_threshold), cv::Scalar(hsv_val - sensitity, sat_max_threshold, val_max_threshold), mask1);
    cv::inRange(hsv, cv::Scalar(hsv_val, sat_min_threshhold, val_min_threshold), cv::Scalar(hsv_val + sensitity, sat_max_threshold, val_max_threshold), mask2);

    // Generating the final mask
    mask1 = mask1 + mask2;
    log_->log(INFO, "Mask generated from target image.");

    // extract contours of the canny image
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours( mask1, contours, hierarchy, cv::RETR_TREE , cv::CHAIN_APPROX_SIMPLE);

    // find the region with the largest area - this is most likely the display
    int area = 10*100; // 1000 pixels area - NOTE: This may cause no contoured area to be generated, if the area is too large.
    int counter = 0;
    int i, val;
    op_image_ = image_.clone();
    for( i = 0; i< contours.size(); i++ )
    {
        if(cv::contourArea(contours[i]) < area) continue; // ignore contours that are too small to be a patty
        cv::drawContours( op_image_, contours, /** i indicates which contour region to draw */ i, cv::Scalar(255,255,255), 2, 8, hierarchy, 0); 
        val = i;
        counter++;
    }
    log_->log(INFO, "Count of regions with contours is: " + std::to_string(counter) );

    if ( ! ( counter == 1 ) ) {
        log_->log(ERROR, "Error. Failed to generate image with 1 contoured area.");
        throw std::runtime_error ("Could not find the image contours");
    }

    contours_ = contours[val];
    return true;
}

/// @brief Save output image. Will save original input image if no operations on the data have taken place.
/// @param filepath Path to the new folder (can be relative or absolute)
bool ImageData::saveImage(std::string folderpath)
{
    if (op_image_.empty() && !image_.empty()) {
        op_image_ = image_.clone();
        log_->log(INFO, "Copying input image to output. No operations have taken place on the input image data.");
    } else if ( image_.empty() ) {
        log_->log(ERROR, "Cannot save output image! Image must be read before being displayed");
        return false;
    }

    std::string output_path ;
    
    auto idxSlash = filepath_.rfind("/");
    if (idxSlash == std::string::npos) {
        filepath_.insert(0, "/");
        idxSlash = 0;
    }

    auto idx = filepath_.rfind(".");
    if ( idx != std::string::npos) // append before the file extension 
        output_path = filepath_.substr(idxSlash+1, (idx-(idxSlash+1))) + "_output" + filepath_.substr(idx, (filepath_.length()-idx));
    else
        output_path = filepath_.substr(idxSlash+1, (filepath_.length())) + "_output";
    output_path = folderpath + output_path;
    
    log_->log(INFO, "Saving to: " + output_path);
    cv::imwrite(output_path, op_image_);
    return true;
}

/// @brief Finds 4 corners of an image
/// @param corners Vector of the co-ordinate points of the corners in the image. 
bool ImageData::findCorners()
{
    if( op_image_.empty() )
    {
        log_->log(ERROR, "Cannot find the corners of the contours, no contours calculated for the input image.");
        return false;
    }

    bool success = false;
    

    // define constants filter
    int maxCorners = 4;
    double qualityLevel = 0.001;
    double minDistance = 200; // screen must take up a large portion of the image
    int blockSize = 10;
    bool useHarrisDetector = false;
    double k = 0.04;
    cv::RNG rng(12345);

    // apply corner detection
    cv::Mat gray;
    cv::cvtColor(op_image_, gray, cv::COLOR_BGR2GRAY); /** Image must be grey scale */
    try {
        cv::goodFeaturesToTrack(gray,
                                corners_,
                                maxCorners,
                                qualityLevel,
                                minDistance,
                                cv::Mat(),
                                blockSize,
                                useHarrisDetector,
                                k );
    } catch ( ... ) {
        std::string msg = "Error! Failed to find the corners of the image.";
        log_->log(ERROR, msg);
        throw std::runtime_error (msg);
    }
    
    // draw corners detected
    log_->log(INFO, "Corners have been detected. Number of corners detected: " + std::to_string(corners_.size()));
    int r = 50;
    try {
        for( int i = 0; i < corners_.size(); i++ )
        { 
            cv::circle( op_image_, corners_[i], r, cv::Scalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255)) ); 
            log_->log(INFO, "For corner[" + std::to_string(i) + "], this is x,y : x " + std::to_string(corners_[i].x) + ", y " + std::to_string(corners_[i].y));
        }
    }  catch ( ... ) {
        std::string msg = "Error! Failed to draw corners onto the output image.";
        log_->log(ERROR, msg);
        throw std::runtime_error (msg);
    }

    success = true;


    return success;
}

/// @brief Calculates the angle of alignment, between a side of the object and the edge of the camera lens
bool ImageData::findAlignment()
{
    if( corners_.empty() )
    {
        log_->log(ERROR, "Cannot get the alignment, corners of the image have not been found. Run findCorners() before calling.");
        return false;
    }

    // calculate horizontal alignment using the top of the object
    int topLeft = 0;
    int topRight = 1;
    float adj = std::sqrt(std::pow(corners_[topRight].x - corners_[topLeft].x, 2));
    float hyp = std::sqrt(std::pow(corners_[topRight].x - corners_[topLeft].x, 2) + std::pow(corners_[topRight].y - corners_[topLeft].y, 2));
    h_alignment_ = std::acos(adj/hyp) * 180.0 / PI;
    log_->log(INFO, "Calculated the horizontal angle \t||| " + std::to_string(h_alignment_) + " (degrees)");

    // calculate vertical alignment using the right of the object
    int bottomLeft = 2;
    float v_adj = std::sqrt(std::pow(corners_[topLeft].y - corners_[bottomLeft].y, 2));
    float v_hyp = std::sqrt(std::pow(corners_[topLeft].x - corners_[bottomLeft].x, 2) + std::pow(corners_[topLeft].y - corners_[bottomLeft].y, 2));
    v_alignment_ = std::acos(v_adj/v_hyp) * 180.0 / PI;
    log_->log(INFO, "Calculated the vertical angle \t||| " + std::to_string(v_alignment_) + " (degrees)");    

    return true;
}

/// @brief Displays an image.
/// @param isOutputImage If true, the image being operated on inside the class will be displayed.
bool ImageData::displayImage(bool isOutputImage)
{
    if( image_.empty() )
    {
        log_->log(ERROR, "Cannot display image! Image must be read before being displayed");
        return false;
    }
    bool success = false;

    cv::Mat displayImg ;
    if (isOutputImage) displayImg = op_image_;
    else displayImg = image_;

    try {
        cv::namedWindow("ImageData::displayImage", cv::WINDOW_AUTOSIZE ); // Create a window for display.
        cv::imshow( "Display Image ", image_ );                // Show our image inside it.
        cv::waitKey(0); // Wait for a keystroke in the window
    } catch ( ... ) {
        std::string msg = "Error! Failed to display the target image";
        log_->log(ERROR, msg);
        throw std::runtime_error (msg);
    }

    success = true;
    return success;
}