#ifndef BLITHEASSERT_H
#define BLITHEASSERT_H

#include <iostream>

#ifndef NDEBUG
    // The do-while helps convert the compound statements into a single statement. So,
    // for eg, there are no errors with using the ASSERT in an if-else block.
    // See https://stackoverflow.com/a/1067238
    #define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
    #define ASSERT(condition, message) do { } while (false)
#endif

#endif // BLITHEASSERT_H