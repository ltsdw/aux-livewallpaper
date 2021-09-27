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

            char* config_path;

            getConfigPath(&config_path);

            if (!config_path) die("couldn't get the configuration path.");
            else
            {
                while (isAuxLwallpaperRunning())
                {
                    const bool is_any_running = isAnyRunning();
                    const bool is_xwinwrap_running = isXwinwrapRunning();
                    const bool is_compositor_running = isCompositorRunning();

                    if (!is_xwinwrap_running && !is_any_running) initXWinwrap(config_path);

                    // it's needed to sleep here
                    // otherwise xwinwrap would get the pid of compositor
                    // sleep(1);

                    if (should_compose && !is_compositor_running && !is_any_running) initCompositor();

                    if (checkFile(config_path, "mpv.log"))
                    {
                        pkill("xwinwrap", SIGKILL);

                        sleep(1);

                        initXWinwrap(config_path);
                    }

                    if (is_any_running && is_xwinwrap_running)
                    {
                        pkill("xwinwrap", SIGKILL);

                        if (should_compose && is_compositor_running) pkill(compositor_name, SIGKILL);
                    }

                    sleep(5);
                }

                free(config_path);
            }
        } else die("aux_lwallpaper already running.");
    } else help();

    return 0;
}
