#ifndef DIPLOMA_PIPE_H
#define DIPLOMA_PIPE_H

#include <iostream>


enum PipeAccessType
{
    PIPE_WRITE
};


class Pipe
{
public:
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