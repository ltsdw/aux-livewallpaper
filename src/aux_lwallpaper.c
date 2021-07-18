#include "util.h"


int main(int arg, const char* const argv[])
{
    char flag[3];
    const Path config_path = getConfigPath();

    daemonize();

    if (argv[1])
        strncpy(flag, argv[1], 2);

    if (!strncmp(flag, "-d", 2))
    {
        cleanAndExit();
        exit(EXIT_FAILURE);
    } else if (!strncmp(flag, "-s", 2))
    {
        if (!checkProcess("xwinwrap"))
        {
            initXWinwrap(config_path);
            while (true)
            {
                if (checkFile(config_path, "mpv.log"))
                {
                    terminateXWinwrap();
                    sleep(2);
                    initXWinwrap(config_path);
                }
                sleep(20);
            }
        } else
            exit(EXIT_FAILURE);
    } else
        exit(EXIT_FAILURE);

    return EXIT_SUCCESS;
}
