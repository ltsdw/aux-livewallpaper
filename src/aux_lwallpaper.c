#include "util.h"


int main(int arg, const char* const argv[])
{
    char flag[10];

    if (argv[1])
        strncpy(flag, argv[1], 2);
    else
        help();

    if (!strncmp(flag, "-h", 2) || !strncmp(flag, "--help", 6))
        help();

    if (!strncmp(flag, "-d", 2))
    {
        cleanAndExit();
        die("something went wrong at stopping processes.\n");
    } else if (!strncmp(flag, "-s", 2))
    {
        daemonize();
        const Path config_path = getConfigPath();

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
            die("process already running.\n");
    } else
        help();

    return EXIT_SUCCESS;
}
