#ifndef UTIL_H
#define UTIL_H

#define _GNU_SOURCE
#define _POSIX_SOURCE

#define VERSION "1.0.9"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>
#include <dirent.h>
#include <sys/stat.h>


typedef char* Filepath;
typedef char* Filename;
typedef char* Cmd;
typedef int Signal;

typedef struct XWinwrap
{
    pid_t pid;
    char config_path[100]; 
    int indx;
    pid_t parr[10];
    char* xwinwrap_cmd[30];
} XWinwrap;

void getConfigPath(char**);
bool checkFile(const Filepath, const Filename);
bool isAuxLwallpaperRunning(void);
bool isXwinwrapRunning(void);
bool isWineserverRunning(void);
bool isCompositorRunning(void);
bool checkForCompositor(void);
void createLogFile(const Filepath);
void daemonize(void);
void setup(void);
void doChecks(void);
Cmd getCompositorName(void);
bool shouldCompose(void);
void initXWinwrap(const Filepath);
void initCompositor(void);
void writePid(const pid_t, const Cmd);
void terminateAndExit(void);
void pkill(const Cmd, const Signal);
void die(const char*, ...);
void help(void);

#endif
