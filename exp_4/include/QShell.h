#ifndef QSHELL_H
#define QSHELL_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#define MAX_PROMPT 1024
#define MAXLINE 4096 //the length of all args is ARG_MAX
#define MAXARG 20

struct passwd *pwd;
char outer_place[MAX_PROMPT];
char* command = NULL;
char** parameters;
int paraNum;
char *buffer;
char prompt[MAX_PROMPT];
const int max_name_len = 256;
const int max_path_len = 1024;

void init();
void type_prompt(char*);
int read_command(char **command,char **parameters);
int builtin_command(char *command, char **parameters);
int outer_command(char *command, char **parameters);
//void doit(int argc, char** argv);


#endif
