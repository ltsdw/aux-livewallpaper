#ifndef UTIL_H
#define UTIL_H

#define _GNU_SOURCE
#define _POSIX_SOURCE

#define VERSION "1.0.3"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>
#include <dirent.h>


typedef char* Path;
typedef char* File;
typedef char* Cmd;

typedef struct XWinwrap
{
    pid_t pid;
    char config_path[100]; 
    int indx;
    pid_t parr[10];
    char* xwinwrap_cmd[30];
} XWinwrap;

Path getConfigPath(void);
bool checkFile(Path, File);
pid_t checkProcess(Cmd);
bool isXwinwrapRunning(void);
void createLogFile(Path);
void daemonize(void);
void initXWinwrap(Path);
void terminateAndExit(void);
void pkill(char*);
void die(const char[], ...);
void help(void);

#endif
