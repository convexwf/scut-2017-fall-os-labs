#ifndef FIND_H
#define FIND_H

#include <ctype.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MATCH      1
#define NOT_MATCH  0
#define MAX_COUNT 100
#define MAX_SIZE  256
#define MATCH_CHAR(c1,c2,ignore_case)  ( (c1==c2) || (ignore_case&&(tolower(c1)==tolower(c2))) // 匹配一个字符的宏

//命令行解析相关变量
char optString[] = "tn:p:m:c:P::";
static const struct option long_option[]={
	{"thread",no_argument,NULL,'t'},
   {"name",required_argument,NULL,'n'},
   {"prune",required_argument,NULL,'p'},
   {"mtime",required_argument,NULL,'m'},
   {"ctime",required_argument,NULL,'c'},
   {"print",optional_argument,NULL,'P'},
   {NULL,0,NULL,0}
};

// 多线程搜索相关变量s
typedef struct thread_arg{
	char s[MAX_SIZE];
} Thread_arg;
Thread_arg* thread_arg;
pthread_mutex_t mutex;

char dir_name[MAX_COUNT];	// 搜索目录，默认为当前目录
char search_name[MAX_SIZE];	// 搜索文件名字
char except_name[MAX_SIZE];	// 除外文件目录
char file_name[MAX_SIZE];
volatile int file_count(0);	// 符合搜索条件的文件数量
bool use_thread;	// 是否使用多线程
bool ignoreCase;	// 是否忽略大小写
bool is_exec;		// 是否执行所查找的文件（不需用户确认）
int c_time;	// 创建时间
int m_time;	// 修改时间
char printVar;
struct timeval start;
struct timeval end;

void init(void);	// 初始化（使用默认参数）
int parse_command(int argc, char** argv);	// 解析命令行
int strMatch(const char* src, const char* pattern);  // 通配符匹配
void add_slash(char* dirname);	// 如果目录的最后一个字母不是'/',则在最后加上一个'/'
int CtimeMatch(char* fileName);
int MtimeMatch(char* fileName);
void Print(void);

#endif
