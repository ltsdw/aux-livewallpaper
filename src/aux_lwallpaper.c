#include "util.h"


int main(int arg, char* const argv[])
{
    char* flag = argv[1];

    if (!argv[1]) help();

    if (!strncmp(flag, "-h", 2) || !strncmp(flag, "--help", 6)) help();

    if (!strncmp(flag, "-v", 2) || !strncmp(flag, "--version", 9)) die("version: %s", VERSION);

    if (!strncmp(flag, "-d", 2))
    {
        terminateAndExit();

        die("something went wrong at stopping processes.");
    } else if (!strncmp(flag, "-s", 2))
    {
        // create necessary directories if they don't exist
        setup();

        if (!isAuxLwallpaperRunning())
        {
            doChecks();

            const bool should_compose = shouldCompose();

            daemonize();

            writePid(getpid(), "aux_lwallpaper");

            const Cmd compositor_name = getCompositorName();

            // unecessary deallocate, it will be used until the program ends
            char* config_path;

            getConfigPath(&config_path);

            if (!config_path) die("couldn't get the configuration path.");
            else
            {
                while (true)
                {
                    if (should_compose && !isCompositorRunning() && !isWineserverRunning()) initCompositor();

                    if (!isXwinwrapRunning() && !isWineserverRunning()) initXWinwrap(config_path);

                    if (checkFile(config_path, "mpv.log"))
                    {
                        pkill("xwinwrap", SIGKILL);

                        sleep(1);

                        initXWinwrap(config_path);
                    }

                    if (isWineserverRunning() && isXwinwrapRunning())
                    {
                        pkill("xwinwrap", SIGKILL);

                        if (should_compose && isCompositorRunning()) pkill(compositor_name, SIGKILL);
                    }

                    sleep(5);
                }

                // but here just to play safe
                free(config_path);
            }
        } else die("aux_lwallpaper already running.");
    } else help();

    return 0;
}
