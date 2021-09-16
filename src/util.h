#ifndef UTIL_H
#define UTIL_H

#define _GNU_SOURCE
#define _POSIX_SOURCE

#define VERSION "1.0.6"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>
#include <dirent.h>


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

void getConfigPath(const Filepath);
bool checkFile(const Filepath, const Filename);
pid_t checkProcess(const Cmd);
pid_t checkProcess_alt(const Cmd);
bool isXwinwrapRunning(void);
bool isWineserverRunning(void);
void createLogFile(const Filepath);
void getLastLine(const Filepath, const Filename);
void daemonize(void);
pid_t getPid(void);
void initXWinwrap(const Filepath);
void terminateAndExit(void);
void writePid(const pid_t, const Cmd);
void removePid(const pid_t);
void pkill(const Cmd, const Signal);
void die(const char*, ...);
void help(void);

#endif
