/// Logger.cpp
/// Creator: Pádraig Basquel

#include "Logger.h"
#include <iostream>

void Logger::log(int level, std::string logstring) 
{
    if ( level < _level) 
        _logger(level, logstring);
}