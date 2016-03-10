#include "MainApplication.h"
#include <glibmm-2.4/glibmm.h>

int
main (int argc, char *argv[])
{
    auto application = MainApplication::create();

    const int status = application->run(argc, argv);
    return status;
}