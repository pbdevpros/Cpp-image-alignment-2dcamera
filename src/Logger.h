/// Logger.h
/// Creator: Pádraig Basquel

#ifndef LOGGER
#define LOGGER

#include <string>
#include <limits>
#include <fstream>

/**
 * Generic logging class.
 * Log level and a log enable can be set by the user.
 * User must define the actual logging function for the class, either at initialization or later.
 * User must deine the logging level.
 */

class Logger
{
  public:
    Logger();
    Logger(bool isEnable) : _isEnable(isEnable) {}
    Logger(bool isEnable, void (*userLogger)(int, std::string)) : _isEnable(isEnable), _logger(userLogger) {}

    void log(int, std::string);
    void setLogger(void (*userLogger)(int, std::string )) { _logger = userLogger ; }
    void enableLogging(bool isEnable) { _isEnable = isEnable ; }
    void setLogLevel(int level) { _level = level;  }

  protected:
    std::ofstream _logstream;
    void (*_logger)(int, std::string);
    bool _isEnable;
    int _level = std::numeric_limits<int>::max();
    
};

#endif