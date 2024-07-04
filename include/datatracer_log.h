#ifndef DATA_TRACER_LOG_H
#define DATA_TRACER_LOG_H

#include <iostream>

#define     log_err   std::cerr << "ERROR: " << __func__ << ": "
#define     log_info  std::cout << "INFO: "  << __func__ << ": "
#if DEBUG
    #define log_debug std::cout << "DEBUG: " << __func__ << ": "
#else
    #define log_debug if (0) std::cout
#endif

#endif // DATA_TRACER_LOG_H
