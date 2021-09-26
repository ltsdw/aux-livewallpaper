#include "util.h"
#include "config.h"


static void makePidfileDir(void)
{
    struct stat st;

    if (stat("/tmp/lwallpaper", &st))
    {
        mkdir("/tmp/lwallpaper", 0700);

        FILE* fp = fopen("/tmp/lwallpaper/lwallpaper.pid", "w");

        if (fp) fclose(fp);
    }
}

static void makeLwallpaperDir(void)
{
    Filepath xdg_config_home = getenv("XDG_CONFIG_HOME");
    Filepath config_path = {0};

    if (!xdg_config_home)
    {
        const Filepath user = getenv("USER");
        if (user)
        {
            asprintf(&config_path, "/%s/%s/%s/", "home", user, ".config");

            free(user);
        } else die("getenv(\"USER\") failed");
    } else asprintf(&config_path, "%s/%s", xdg_config_home, "live_wallpaper");

    if (config_path)
    {
        struct stat st;
        if (stat(config_path, &st) == -1)
        {
            Filepath medias_path;

            mkdir(config_path, 0700);

            asprintf(&medias_path, "%s/%s", config_path, "medias");

            if (medias_path)
            {
                mkdir(medias_path, 0700);

                free(medias_path);
                free(config_path);
            } else die("failed to create lwallpaper/medias directory");
        }
    } else die("unable to create configuration directory");
}

static bool mediaExist(void)
{
    if (!media[0]) die("media file is empty, check config.h.");

    Filename media_file;

    char* config_path;

    getConfigPath(&config_path);

    if (config_path)
    {
        asprintf(&media_file, "%s/%s/%s", config_path, "medias", media);

        free(config_path);

        if (media_file)
        {

            bool res = access(media_file, F_OK);

            free(media_file);

            return (!res);
        }
    }

    return false;
}

static bool doesBinExists(const Cmd cmd)
{
    return (!access(cmd, F_OK));
}

Cmd getCompositorName(void)
{
    if (compositor[0]) return compositor[0];

    return "";
}

bool shouldCompose(void)
{
    return getCompositorName()[0];
}

void setup(void)
{
    makeLwallpaperDir();
    makePidfileDir();
}

void doChecks(void)
{
    Cmd compositor_name = getCompositorName();
    Cmd cmd_mpv = "/usr/bin/mpv";

    if (!doesBinExists(cmd_mpv)) die("%s: not found", cmd_mpv);

    if (shouldCompose()) if (!doesBinExists(compositor_name)) die("%s: not found", compositor_name);

    if (!mediaExist()) die("media %s not found", media);
}

static int terminateProcess(pid_t pid, Signal signum)
{
    if (pid > 0) kill(pid, signum);
    else return -1;

    return 0;
}

/*
 * buf: dinamically allocated memory should be freed after use
 */
void getConfigPath(char** buf)
{
    char* tmp;

    const Filepath path = "/.config/live_wallpaper";
    Filepath home = getenv("HOME");

    asprintf(&tmp, "%s%s", home, path);

    if (tmp)
    {
        asprintf(buf, "%s", tmp);

        free(tmp);
    }
}

/*
 * pname_: name of the process to search for
 * return: the pid found on success match and 0 on fail
*/
static pid_t checkProcess_alt(const Cmd pname_)
{
    DIR* dir;
    struct dirent* ent;

    if (!(dir = opendir("/proc"))) die("unable to open /proc directory");

    while((ent = readdir(dir)))
    {
        char* buf;
        long pid;
        char state;

        long lpid = atol(ent->d_name);

        if (lpid <= 1000) continue;

        asprintf(&buf, "/proc/%ld/stat", lpid);

        if (buf)
        {
            FILE* fp = fopen(buf, "r");

            free(buf);

            if (fp)
            {
                char* pname;

                if ((fscanf(fp, "%ld (%m[^)]) %c", &pid, &pname, &state) != 3))
                {
                    fclose(fp);
                    closedir(dir);

                    if (pname) free(pname);

                    die("failed to parse pid and process name");
                }

                if (!strncmp(pname_, pname, strlen(pname)) && (state == 'R' || state == 'S'))
                {
                    fclose(fp);
                    closedir(dir);
                    free(pname);

                    return pid;
                }

                fclose(fp);
            }
        }
    }

    return 0;
}

/*
 * pname_: name of the process to search for
 * return: the pid found on success match and 0 on fail
*/
static pid_t checkProcess(const Cmd pname_)
{
    const Filename pid_file = "/tmp/lwallpaper/lwallpaper.pid";

    FILE* fp = fopen(pid_file, "r");

    if (!fp) return checkProcess_alt(pname_);

    pid_t pid;
    char* cmd;

    while((fscanf(fp, "%d %ms", &pid, &cmd)) == 2)
    {
        if (cmd && (!strncmp(pname_, cmd, strlen(cmd))))
        {
            fclose(fp);
            free(cmd);

            return pid;
        }
    }

    fclose(fp);

    // case lwallpaper.pid is empty and we're looking for the daemon itself, it isn't running
    if (!strncmp(pname_, "aux_lwallpaper", strlen("aux_lwallpaper"))) return false;

    // case search in pid_file doesn't return
    return checkProcess_alt(pname_);
}

bool isXwinwrapRunning(void) { return checkProcess("xwinwrap"); }

bool isWineserverRunning(void) { return checkProcess("wineserver"); }

bool isCompositorRunning(void) { return checkProcess(getCompositorName()); }

bool isAuxLwallpaperRunning(void) { return checkProcess("aux_lwallpaper"); }

void createLogFile(const Filepath config_path)
{
    char* old;
    char* new;

    asprintf(&old, "%s/%s", config_path, "mpv.log.OLD");
    asprintf(&new, "%s/%s", config_path, "mpv.log");

    if (old && new)
    {
        FILE* fp = fopen(new, "r");

        if (fp)
        {
            fclose(fp);

            rename(new, old);

            free(old);
        }

        fp = fopen(new, "w");

        if (fp) fclose(fp);

        free(new);
    }
}

/*
 * buf: memory allocated dinamically, needs to be free after use
 */
static void getLastLine(const Filepath config_log_file, char* buf)
{
    char c;
    int len = 0;

    FILE* fp = fopen(config_log_file, "r");

    if (!fp) die("failed to open %s", config_log_file);

    fseek(fp, -1, SEEK_END);
    c = fgetc(fp);

    // file is empty
    if (c == EOF) return;

    while (c == '\n')
    {
        fseek(fp, -2, SEEK_CUR);
        c = fgetc(fp);
    }

    while (c != '\n')
    {
        fseek(fp, -2, SEEK_CUR);
        ++len;
        c = fgetc(fp);
    }

    fseek(fp, 1, SEEK_CUR);

    if (!fgets(buf, len, fp)) die("failed to get last line");

    fclose(fp);
}

bool checkFile(const Filepath path, const Filename file)
{
    char buf[4096];
    char* log_file;

    asprintf(&log_file, "%s/%s", path, file);

    if (log_file)
    {
        getLastLine(log_file, buf);

        free(log_file);

        return (strstr(buf, "operation failed") || strstr(buf, "Error") || strstr(buf, "BadDrawable"));
    }

    return false;
}

void daemonize(void)
{
    /* fork off the parent process */
    pid_t pid = fork();

    if (pid < 0) die("error at forking parent process.");
    /* on success let the parent terminate */
    else if (pid) exit(EXIT_SUCCESS);
    else
    {
        umask(0);
        /* on success the child process becomes session leader */
        if (setsid() < 0) die("failed to set child process.");

        /* changing dir to root directory */
        chdir("/");

        /* close all open file descriptors */
        for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
        {
            close(fd);
        }
    }
}

/* cmd: command to be spawned
 * args: arguments to command
 * should_daemonize: if true it will add 1 to the pid
 */
static pid_t spawnProcess(const Cmd cmd, char* const args[], const bool should_daemonize)
{
    int status;

    pid_t pid = fork();

    if (pid < 0) die("something went wrong.");
    else if (pid) 
    {
        waitpid(pid, &status, WNOHANG);

        /*TODO
         * compositor picom doesn't go well while it goes daemonize and this daemon closes
         * any file descriptors.
         *
         * while picom isn't in daemon mode it's not possible to wait for the
         * status exit, so it's also not possible to catch any error.
         *
         * WIFEXITED(status) - WEXITSTATUS(status)
         */
    } else if (execv(cmd, args)) die("something went wrong with execv()");

    if (should_daemonize) ++pid;

    return pid;
}

void writePid(const pid_t pid, const Cmd cmd)
{
    FILE* fp;

    const Filename pid_file = "/tmp/lwallpaper/lwallpaper.pid";

    fp = fopen(pid_file, "a");

    if (fp)
    {
        pid_t t_pid;

        while ((fscanf(fp, "%d", &t_pid) == 1))
        {
            if (t_pid == pid)
            {
                fclose(fp);
                return;
            }
        }

        fprintf(fp, "%d %s\n", pid, cmd);

        fclose(fp);

    } else die("unable to open/create %s pid_file", pid_file);

}

static void removePid(const pid_t pid)
{
    FILE* fp;
    FILE* tmp_fp;

    const Filename pid_file = "/tmp/lwallpaper/lwallpaper.pid";
    const Filename tmp_pid_file = "/tmp/lwallpaper/tmp~lwallpaper.pid";

    fp = fopen(pid_file, "r");
    tmp_fp = fopen(tmp_pid_file, "w");

    if (fp)
    {
        pid_t t_pid;
        char* cmd;

        while((fscanf(fp, "%d %ms", &t_pid, &cmd) == 2))
        {
            if (cmd)
            {
                if (t_pid != pid) fprintf(tmp_fp, "%d %s\n", t_pid, cmd);
                free(cmd);
            }
        }

        rename(tmp_pid_file, pid_file);
        fclose(fp);
        fclose(tmp_fp);
    }
}

void initXWinwrap(Filepath config_path)
{
    char* log_file_flag;
    char* media_file;

    asprintf(&log_file_flag, "%s%s/%s", "--log-file=", config_path, "mpv.log");
    asprintf(&media_file, "%s/medias/%s", config_path, media);

    if (log_file_flag && media_file)
    {
        char* xwinwrap_cmd[] = {"/usr/bin/xwinwrap", "-g", "1366x768", "-ni", "-s",
                                "-nf", "-b", "-un", "-ov", "-fdt", "-argb",
                                "--",
                                "/usr/bin/mpv", "--msg-level=ffmpeg=fatal,vo=fatal", log_file_flag,
                                "--audio=no", "--osc=no", "--cursor-autohide=no", "--no-input-cursor",
                                "--input-vo-keyboard=no", "--osd-level=0", "--hwdec=vaapi-copy",
                                "--vo=vaapi", "-wid", "WID", "--loop-file=yes", media_file, NULL};

        createLogFile(config_path);

        pid_t pid = spawnProcess(xwinwrap_cmd[0], xwinwrap_cmd, false);

        writePid(pid, "xwinwrap");

        free(log_file_flag);
        free(media_file);
    }
}

void initCompositor(void)
{
    Cmd compositor_name = getCompositorName();

    pid_t pid = spawnProcess(compositor_name, compositor, false);

    writePid(pid, compositor_name);
}

void pkill(const Cmd pname_, const Signal signum)
{
    pid_t pid = checkProcess(pname_);

    if (pid)
    {
        removePid(pid);
        terminateProcess(pid, signum);
    }
}

void terminateAndExit(void)
{
    if (isXwinwrapRunning()) pkill("xwinwrap", SIGKILL);

    if (isAuxLwallpaperRunning()) pkill("aux_lwallpaper", SIGTERM);

    if (shouldCompose() && isCompositorRunning()) pkill(getCompositorName(), SIGTERM);

    exit(EXIT_SUCCESS);
}

void die(const char* fmt, ...)
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
