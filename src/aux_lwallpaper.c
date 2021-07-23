#include "util.h"


int main(int arg, const char* const argv[])
{
    char flag[10];

    if (argv[1])
        strncpy(flag, argv[1], 9);
    else
        help();

    if (!strncmp(flag, "-h", 2) || !strncmp(flag, "--help", 6))
        help();

    if (!strncmp(flag, "-v", 2) || !strncmp(flag, "--version", 9))
        die("version: %s", VERSION);

    if (!strncmp(flag, "-d", 2))
    {
        cleanAndExit();
        die("something went wrong at stopping processes.");
    } else if (!strncmp(flag, "-s", 2))
    {
        if (!isXwinwrapRunning())
        {
            daemonize();
            const Path config_path = getConfigPath();

            initXWinwrap(config_path);

            while (true)
            {
                if (checkFile(config_path, "mpv.log"))
                {
                    terminateXWinwrap();
                    sleep(2);
                    initXWinwrap(config_path);
                }

                if (!isMpvRunning())
                {
                    terminateXWinwrap();
                    sleep(2);
                    initXWinwrap(config_path);
                }

                sleep(20);
            }
        } else
            die("aux_lwallpaper already running.");
    } else
        help();

    return EXIT_SUCCESS;
}
