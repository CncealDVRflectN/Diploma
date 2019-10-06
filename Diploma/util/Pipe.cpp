#include "Pipe.h"
#include "debug_info.h"
#include <cstdarg>


static const char * const ACCESS_TYPE_WRITE = "w";


#pragma region Constructors and destructors

Pipe::Pipe(const char* const pipeName, PipeAccessType accessType)
{
    const char* accessTypeStr = nullptr;

    switch (accessType)
    {
    case PIPE_WRITE:
        accessTypeStr = ACCESS_TYPE_WRITE;
        break;
    default:
        break;
    }

    assert_message(accessTypeStr != nullptr, "Unknown pipe access type");

#ifdef WIN32
    mPipe = _popen(pipeName, accessTypeStr);
#else
    mPipe = popen(pipeName, accessTypeStr);
#endif

    assert_message(mPipe != nullptr, "Cannot open pipe");
}


Pipe::~Pipe()
{
    if (mPipe != nullptr)
    {
        close();
    }
}

#pragma endregion


#pragma region Member methods

void Pipe::write(const char* const format, ...)
{
    assert_message(mPipe != nullptr, "Cannot write data to pipe, because it's already closed");

    va_list args;
    va_start(args, format);
    vfprintf(mPipe, format, args);
    va_end(args);
}


void Pipe::flush()
{
    assert_message(mPipe != nullptr, "Cannot flush pipe, because it's already closed");

    fflush(mPipe);
}


void Pipe::close()
{
    assert_message(mPipe != nullptr, "Cannot close pipe, because it's already closed");

#ifdef WIN32
    _pclose(mPipe);
#else
    pclose(mPipe);
#endif

    mPipe = nullptr;
}


bool Pipe::isValid()
{
    return (mPipe != nullptr);
}

#pragma endregion
