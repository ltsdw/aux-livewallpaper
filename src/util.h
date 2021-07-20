#ifndef UTIL_H
#define UTIL_H

#define VERSION "0.0.1.1"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdarg.h>


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

int terminateProcess(pid_t);
pid_t spawnProcess(const char* cmd, char* const args[]);
Path getConfigPath(void);
bool checkFile(Path, File);
bool checkProcess(Cmd);
void createLogFile(Path);
void daemonize(void);
void cleanAndExit(void);
void absBinPath(Path, char*, const char*);
void initXWinwrap(Path);
void terminateXWinwrap(void);
void die(const char[], ...);
void help(void);

#endif
