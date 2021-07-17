#include "util.h"


int main(int arg, const char* const argv[])
{
    char flag[3];
        
    struct XWinwrap* xwinwrap;
    xwinwrap = (XWinwrap*) malloc(sizeof(XWinwrap));

    strcpy(xwinwrap->config_path, getConfigPath());

    daemonize(xwinwrap);

    if (argv[1])
        strncpy(flag, argv[1], 2);

    if (!strncmp(flag, "-d", 2))
    {
        cleanAndExit(xwinwrap);
        exit(EXIT_FAILURE);
    } else if (!strncmp(flag, "-s", 2))
    {
    
        if (!checkProcess("xwinwrap"))
        {
            initXWinwrap(xwinwrap);
            while (true)
            {
                if (checkFile(xwinwrap->config_path, "mpv.log"))
                {
                    terminateXWinwrap(xwinwrap);
                    sleep(2);
                    initXWinwrap(xwinwrap);
                }
                sleep(20);
            }
        } else
            exit(EXIT_FAILURE);
    } else
        exit(EXIT_FAILURE);

    free(xwinwrap);

    return EXIT_SUCCESS;
}
