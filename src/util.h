#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


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
//void spawnProcess(const char* cmd, char* const args[], XWinwrap*);
pid_t spawnProcess(const char* cmd, char* const args[]);
Path getConfigPath(void);
bool checkFile(Path, File);
bool checkProcess(Cmd);
void createLogFile(Path);
void daemonize(void);
void cleanAndExit(void);
void absBinPath(Path, char*, const char*);
void initXWinwrap(Path);
void terminateXWinwrap();

#endif
