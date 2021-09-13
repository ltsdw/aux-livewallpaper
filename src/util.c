#include "util.h"
#include "config.h"


static int terminateProcess(pid_t pid)
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

/*
 * pname_: name of the process to search for
 * return: the pid found on success match and 0 on fail
*/
pid_t checkProcess(Cmd pname_)
{
    DIR* dir;
    struct dirent* ent;
    char pname[512];
    char buf[256];

    FILE* fp;

    if (!(dir = opendir("/proc")))
    {
        die("unable to open /proc directory");
    }

    while((ent = readdir(dir)) != NULL)
    {
        long pid;
        char state;

        long lpid = atol(ent->d_name);

        if (lpid <= 0) continue;

        snprintf(buf, sizeof buf, "/proc/%ld/stat", lpid);

        fp = fopen(buf, "r");

        if (fp)
        {
            if ((fscanf(fp, "%ld (%[^)]) %c", &pid, pname, &state) != 3))
            {
                fclose(fp);
                closedir(dir);
                die("failed to parse pid and process name");
            }

            if (!strncmp(pname_, pname, sizeof pname) && (state == 'R' || state == 'S'))
            {
                fclose(fp);
                closedir(dir);
                return pid;
                break;
            }
        }

        if (fp) fclose(fp);
    }

    return 0;
}

bool isXwinwrapRunning(void)
{
    return checkProcess("xwinwrap");
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

static pid_t spawnProcess(const char* cmd, char* const args[])
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
                            "--input-vo-keyboard=no", "--osd-level=0", "--hwdec=vaapi-copy",
                            "--vo=vaapi", "-wid", "WID", "--loop-file=yes", media_file, NULL};

    createLogFile(config_path);

    if ((spawnProcess(xwinwrap_cmd[0], xwinwrap_cmd) ) < 0)
        die("error at spawning xwinwrap.");
}

void pkill(Cmd pname_)
{
    pid_t pid = checkProcess(pname_);
    if (pid) terminateProcess(pid);
}

void terminateAndExit(void)
{
    pkill("xwinwrap");
    pkill("aux_lwallpaper");

    exit(EXIT_SUCCESS);
}

static void removeExeFromAbsPath(char* path, char* buf)
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

__attribute__((unused)) static void absBinPath(char* buf, char* buf_, const char* argv0)
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
