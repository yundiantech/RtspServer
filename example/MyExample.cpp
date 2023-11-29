#include <iostream>

#include "net/RtspStreamManager.h"

int main(int argc, char* argv[])
{
    RtspStreamManager *stream_manager = new RtspStreamManager();

    stream_manager->start_server();

    while (1)
    {
        usleep(1000000);
    }

    return 0;
}