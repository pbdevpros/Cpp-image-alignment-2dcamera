#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <vector>
#include <json/json.h>
#include "Logger.h"
#include "ImageData.h"
#include "ProcessQueue.h"

#define kUSER_CONFIG_PATH       "../config.json"
#define kLOG_PATH_KEY           "log_path"
#define kIMAGE_PATH_KEY         "image_path"
#define kIMAGE_EXTENSION_KEY    "image_extension"
#define kOUTPUT_PATH            "output_path"
#define kPROCESS_STEPS          (4) 

void AlignerLog(int level, std::string logstring);
std::vector<std::string>  getFilenames(std::string path, std::string extension);
Json::Value getUserConfig(std::string path);
void ProcessImages(std::vector<std::shared_ptr<ProcessQueue>> que, ImageData data, std::string outputFolder);
void Usage(std::string function);

int main(int argc, char* argv[]) {

    // usage
    bool CONTINUOUS = false;
    bool ONCE = false;
    if ( argc == 2 && ( strcmp(argv[1], "-l") == 0  || strcmp(argv[1], "--linear") == 0 ) ) {
        ONCE = true;
    } else if ( argc == 2 &&  ( strcmp(argv[1], "-c") == 0  || strcmp(argv[1], "--continuous") == 0 ) ) {
        CONTINUOUS = true;
    } else { // help
        Usage(argv[0]);
        return 0;   
    }

    // read user configuration
    Json::Value userConfig = getUserConfig(kUSER_CONFIG_PATH);
    std::string logPath  = userConfig.get(kLOG_PATH_KEY, "UTF-8" ).asString();
    std::string imagesPath = userConfig.get(kIMAGE_PATH_KEY, "UTF-8" ).asString();
    std::string outputPath = userConfig.get(kOUTPUT_PATH, "UTF-8").asString();
    std::string extension = userConfig.get(kIMAGE_EXTENSION_KEY, "UTF-8").asString();
    std::vector<std::string> imagesNames = getFilenames(imagesPath, extension);

    // initialize log path
    std::ofstream ifs(logPath);
    if (!ifs.is_open()) {
        ifs.open(logPath);
    }
    Logger imageLog(true, AlignerLog);

    // read in images, overlay a contour over the object in the image and get the co-ordinates of the corners of the image.
    bool success = false;
    if (CONTINUOUS) {
       
        std::vector<std::string> completeImages = imagesNames;
        int newFiles = imagesNames.size();
        while ( newFiles != 0 ) {

            newFiles = 0;
            for ( auto &imageName : imagesNames ) {
                ImageData data(imagesPath + imageName);
                data.setLogger(imageLog);
                success = data.readImage();
                success = data.findContours();
                success = data.findCorners();
                success = data.saveImage(outputPath);
                if (success == false) {
                    throw std::runtime_error ("Error, processing image failed.");
                }
            }

            // wait - make sure all files desired are in folder
            AlignerLog(INFO, "Waiting for files in input folder...");
            for ( int i = 0; i < 5; ++i )
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            // scan the input folder again - if there are no new images, exit
            std::vector<std::string> newFileNames = getFilenames(imagesPath, extension);
            std::vector<std::string>::iterator nameIter;
            imagesNames = newFileNames;
            
            for ( const auto &name : newFileNames ) {
                nameIter = std::find(completeImages.begin(), completeImages.end(), name);
                if ( nameIter != completeImages.end() ) {
                    imagesNames.erase(std::remove(imagesNames.begin(), imagesNames.end(), name), imagesNames.end());
                } else { // new image
                    newFiles++;
                    completeImages.push_back(name); // this image can now be considered complete, it will be input in the next loop.
                }
            }

            AlignerLog(INFO, "A loop through the input folder is complete, there are " + std::to_string(newFiles) + " new files.");
        }

        return 0;
    }

    if ( ONCE ) {
        std::vector<std::thread> threads;
        std::vector<std::shared_ptr<ProcessQueue>> que;

        for ( int i = 0 ; i < kPROCESS_STEPS ; ++i ) {
            std::shared_ptr<ProcessQueue> p_step = std::make_shared<ProcessQueue> ();
            que.emplace_back(p_step);
        }

        if ( que.size() != kPROCESS_STEPS ) {
            throw std::runtime_error ("Error! Creating que failed, size of que is incorrect");
        }


        for ( auto &imageName : imagesNames ) {
            // start a thread for each image, processing each image in each thread
            ImageData newImage(imagesPath + imageName);
            newImage.setLogger(imageLog);
            threads.emplace_back(std::thread (ProcessImages, que, std::move(newImage), outputPath));
        }

        if ( threads.size() != imagesNames.size() ) {
            throw std::runtime_error ("Error! Number of threads created does not match the number of images being processed.");
        }

        for ( auto &imageThread : threads ) 
            imageThread.join();

        for ( int i = 0 ; i < que.size() ; ++i ) {
            que[i].reset();
        }

        AlignerLog(INFO, "Image threads have been processed. Finishing...");
    } 
    
    // close log stream
    imageLog.log(INFO, "Program is complete! Exiting...");
    if (ifs.is_open()) {
        ifs.close();
    }

    return 0;
}

/**
 * Print usage
 */
void Usage(std::string function)
{
    std::cout << function << "\nAlign a display square with the top and bottom edges of a camera." << std::endl << std::endl;
    std::cout << "linear \t\t[-l] [--linear] run through each image file in a folder, calculating the degree of the edges of the display to the edges of the camera." << std::endl;
    std::cout << "continuous \t[-c] [--continous] run in a continous loop, reading files from input folder at 5 second intervals" << std::endl << std::endl;
}

/**
 * Logging function.
 */
void AlignerLog(int level, std::string logstring)
{
    std::cout << __TIME__ << " LINE: " <<__LINE__ << " <" << level << "> " << logstring << std::endl;
}

/**
 * Read in the filenames from given folder.
 */
std::vector<std::string> getFilenames(std::string path, std::string extension) {
  std::vector<std::string> filenames;
  DIR* directory = opendir(path.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    std::string filename(file->d_name);
    if (filename.find(extension) != std::string::npos) {
        filenames.push_back(filename);
        AlignerLog(INFO, filename);
    }
  }
  closedir(directory);
  return filenames;
}

/**
 *  Reads in config details from a JSON file.
 */ 
Json::Value getUserConfig(std::string path)
{
    Json::Value root;
    std::ifstream config_doc(path, std::ifstream::binary);
    config_doc >> root;
    return root;
}

/**
 * Synchronize the processing of images. Ensures only a set number of processes are active at any one time,
 * to prevent possible corruption of image data.
 */
void ProcessImages(std::vector<std::shared_ptr<ProcessQueue>> que, ImageData data, std::string outputFolder)
{
    if ( que.size() > kPROCESS_STEPS) 
        throw std::runtime_error("Error! The number of steps in the queue is larger than 4"); 

    bool success = false;
    for ( int i = 0 ; i < que.size(); ++i ){
        que[i]->Wait();
        switch (i) {
            case 0: success = data.readImage();
                    break;
            case 1: success = data.findContours();
                    break;
            case 2: success = data.findCorners();
                    if (success) success = data.findAlignment();
                    else break;
                    break;
            case 3: success = data.saveImage(outputFolder);
                    break;
        }
        if (!success)
            throw std::runtime_error ("Error! Processing " + data.getFilename() + " failed.");
        que[i]->NotifyCompletion();

    }
}