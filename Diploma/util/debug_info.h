#ifndef DIPLOMA_DEBUG_INFO_H
#define DIPLOMA_DEBUG_INFO_H

#include <iostream>


#pragma region Assert message

#ifdef NDEBUG

    #define assert_message(condition, message) do { } while(false)

#else

    #define assert_message(condition, message) \
    do \
    { \
        if (!(condition)) \
        { \
            std::cerr << "Assetion '" #condition "', failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while(false)

#endif

#pragma endregion

#endif