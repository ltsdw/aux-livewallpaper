#include "util.h"
#include "config.h"


int terminateProcess(pid_t pid)
{
    const int status = 0;

    if (pid > 0)
        kill(pid, SIGKILL);

    return status;
}

/* this function should be used only once
 * will return null on a second call */
Path getConfigPath(void)
{
    const Path path_ = "/.config/live_wallpaper";
    Path path = getenv("HOME");
    strncat(path, path_, 200);
    realpath(path, NULL);

    return path;
}

bool checkFile(Path path, File file)
{
    char buf_[2];
    char* buf;
    bool found = false;

    asprintf(&buf, "%s %s/%s %s", "tail -1", path, file, "| grep -iE -m1 \"operation failed|Error|BadDrawable\"");

    FILE* pp = popen(buf, "r");

    if (pp)
    {
        if (fgets(buf_, sizeof(buf_), pp))
            found = true;
    } else
        die("something went wrong at running popen(%s, \"r\")", buf);

    pclose(pp);

    return found;
}

bool checkProcess(Cmd cmd)
{
    char buf_[2];
    char* buf;
    bool found = false;

    asprintf(&buf, "%s %s %s", "ps aux | grep", cmd, "| grep -v grep");

    FILE* pp = popen(buf, "r");

    if (pp)
    {
        if (fgets(buf_, sizeof(buf_), pp))
            found = true;
    } else
        die("something went wrong at running popen(%s, \"r\")", buf);

    pclose(pp);

    return found;
}

void createLogFile(Path config_path)
{
    FILE* fp;
    char* old;
    char* new;

    asprintf(&old, "%s/%s", config_path, "mpv.log.OLD");
    asprintf(&new, "%s/%s", config_path, "mpv.log");

    rename(new, old);

    fp = fopen(new, "w");
    fclose(fp);
}

void daemonize(void)
{
    /* fork off the parent process */
    pid_t pid = fork();

    if (pid < 0)
        die("error at forking parent process.");
    /* on success let the parent terminate */
    else if (pid)
        exit(EXIT_SUCCESS);
    else
    {
        /* on success the child process becomes session leader */
        if (setsid() < 0)
            die("failed to set child process.");

        /* changing dir to root directory */
        chdir("/");

        /* close all open file descriptors */
        for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
        {
            close(fd);
        }
    }
}

pid_t spawnProcess(const char* cmd, char* const args[])
{
    pid_t pid = fork();

    if (pid < 0)
        die("something went wrong.");
    else if (pid)
        waitpid(pid, NULL, WNOHANG);
    else
    {
        execv(cmd, args);
        die("something went wrong.");
    }

    return pid;
}

void initXWinwrap(Path config_path)
{
    char* log_file_flag;
    char* media_file;

    asprintf(&log_file_flag, "%s%s/%s", "--log-file=", config_path, "mpv.log");
    asprintf(&media_file, "%s/medias/%s", config_path, media);

    char* xwinwrap_cmd[] = {"/usr/bin/xwinwrap", "-g", "1366x768", "-ni", "-s",
                            "-nf", "-b", "-un", "-ov", "-fdt", "-argb", "-d",
                            "--",
                            "/usr/bin/mpv", "--msg-level=ffmpeg=fatal,vo=fatal", log_file_flag,
                            "--audio=no", "--osc=no", "--cursor-autohide=no", "--no-input-cursor",
                            "--input-vo-keyboard=no", "--osd-level=0", "--hwdec=vaapi",
                            "--vo=vaapi", "-wid", "WID", "--loop-file=yes", media_file, NULL};

    createLogFile(config_path);

    if ((spawnProcess(xwinwrap_cmd[0], xwinwrap_cmd) ) < 0)
        die("error at spawning xwinwrap.");
}

void terminateXWinwrap(void)
{
    char* cmd_kill_xwinwrap[] = {"/usr/bin/pkill", "-9", "xwinwrap", NULL};
    char* cmd_kill_mpv[] = {"/usr/bin/pkill", "-9", "mpv", NULL};

    /* TODO 
     * implement a more procise way to terminate processes by pid
     * from the xwinwrap->parr[] structure */
    if (spawnProcess(cmd_kill_xwinwrap[0], cmd_kill_xwinwrap) < 0)
        die("error at killing process xwinwrap.");

    if (spawnProcess(cmd_kill_mpv[0], cmd_kill_mpv) < 0) 
        die("error at killing process mpv.");
}

void cleanAndExit(void)
{
    char* cmd_kill_aux_lwallpaper[] = {"/usr/bin/pkill", "-15", "aux_lwallpaper", NULL};

    /* TODO 
     * implement a way to kill process using the Config structure */


    if (spawnProcess(cmd_kill_aux_lwallpaper[0], cmd_kill_aux_lwallpaper) < 0)
        die("error spawning processes.");

    terminateXWinwrap();

    exit(EXIT_SUCCESS);
}

void removeExeFromAbsPath(char* path, char* buf)
{
    char* token = strtok(path, "/");
    char* tmp[200];
    size_t bnd = sizeof(tmp) - 1;
    int c = 0;

    while (token != NULL)
    {
        tmp[c] = token;
        token = strtok(NULL, "/");
        c++;
    }

    strncat(buf, "/", bnd);

    for (int i = 0; i <= c-2; i++)
    {
        strncat(buf, tmp[i], bnd);
        strncat(buf, "/", bnd);
    }
}

void absBinPath(char* buf, char* buf_, const char* argv0)
{
    char tmp[200];
    size_t bnd = sizeof(tmp) - 1;

    if (argv0[0] == '/')
        strncpy(buf, argv0, bnd);
    else
    {
        if (!getcwd(tmp, bnd))
            die("getcdw error.");

        strncat(tmp, "/", bnd);
        strncat(tmp, argv0, bnd);
    }

    if (!realpath(tmp, buf))
        die("realpath error.");

    removeExeFromAbsPath(tmp, buf_);
}

void die(const char fmt[], ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);

    if (fmt[strlen(fmt)-1] == ':')
    {
        fputc(' ', stderr);
        perror(NULL);
    }
    else fputc('\n', stderr);

    exit(EXIT_FAILURE);
}

void help(void)
{
    die("version: %s\n"
        "    -h --help          for help.\n"
        "    -s                 to start the thing.\n"
        "    -d                 to stop the thing.\n"
        "    -v --version       for versioning.", VERSION);
}
