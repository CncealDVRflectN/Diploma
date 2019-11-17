#ifndef DIPLOMA_PIPE_H
#define DIPLOMA_PIPE_H

#include <iostream>


enum class PipeAccessType
{
    PIPE_WRITE
};


class Pipe
{
public:
    Pipe(const std::string& pipeName, PipeAccessType accessType);

    Pipe(const char* const pipeName, PipeAccessType accessType);

    ~Pipe();


    void write(const char* const format, ...);

    void flush();

    void close();

    bool isValid();

private:
    FILE* mPipe;
};

#endif