#include <pwd.h>
#include <sys/time.h>
#include "color.h"
#include "find.h"

void init(void)
{
	getcwd(dir_name, MAX_SIZE);
	//sprintf(dir_name, "/");
	sprintf(search_name, "*");
	add_slash(dir_name);
	use_thread = false;
	ignoreCase = false;
	is_exec = false;
	c_time = 0;
	m_time = 0;
	printVar = '\n';
    thread_arg = (Thread_arg*)malloc(sizeof(Thread_arg));
	pthread_mutex_init(&mutex, NULL);
}

int parse_command(int argc, char** argv) {
	if(argc > 1)
	{
		if(argv[1][0]=='.')
		{
			getcwd(dir_name, MAX_SIZE);
			if(argv[1][1]=='.')
			{
				sprintf(strrchr(dir_name, '/'), argv[1]+2);
			}
			else
			{
				sprintf(strchr(dir_name, '\0'), argv[1]+1);
			}
		}
        if(argv[1][0]=='~')
        {
            sprintf(dir_name, "/home/%s%s", getpwuid(getuid())->pw_name, argv[1]+1);
        }
		if(argv[1][0]=='/')
		{
			sprintf(dir_name, argv[1]);
		}
	}
	int prefix = 1;
	int opt = 0;
	while((opt=getopt_long_only(argc, argv, optString,long_option, NULL))!=-1)
    {
		//printf("I am not here \n");
        switch(opt)
        {
		case 't':
			use_thread = true;
			break;
		case 'n':
			sprintf(search_name, optarg);
			break;
		case 'p':
			sprintf(except_name, optarg);
			break;
		case 'm':
			prefix = 1;
			if(optarg[0] == '-') prefix = -1;
			m_time = atoi(optarg+1) * prefix;
			break;
		case 'c':
			prefix = 1;
			if(optarg[0] == '-') prefix = -1;
			c_time = atoi(optarg+1) * prefix;
			break;
		case 'P':
			if(optarg != NULL && optarg[0] == '0') printVar = '\0';
			break;
		default:
			break;
        }
    }
	return 0;
}

int strMatch( const char *str1, const char *str2) {
	int slen1 = strlen(str1);
	int slen2 = strlen(str2);

	//实际使用时根据strl的长度来动态分配表的内存
	char matchmap[128][128];
	memset(matchmap, 0, 128*128);
	matchmap[0][0] = 1;
	int i, j, k;
	//遍历目标字符串符串
	for(i = 1; i<= slen1; ++i)
	{
		//遍历通配符串
		for(j = 1; j<=slen2; ++j)
		{
			//当前字符之前的字符是否已经得到匹配
			if(matchmap[i-1][j-1])
			{
				//匹配当前字符
				if(str1[i-1] == str2[j-1] || str2[j-1] == '?')
				{
					matchmap[i][j] = 1;
					//考虑星号在末尾的情况
					if( i  == slen1 && j < slen2)
					{
						for ( k = j+1 ; k <= slen2 ; ++k )
						{
							if( '*' == str2[k-1])
							{
								matchmap[i][k] = 1;
							}else{
								break;
							}
						}
					}
				}else if(str2[j-1] == '*')
				{
					//遇到星号，目标字符串到末尾都能得到匹配
					for(k = i-1; k<=slen1; ++k)
					{
						matchmap[k][j] = 1;
					}
				}
			}
		}
		//如果当前字符得到了匹配则继续循环，否则匹配失败
		for(k = 1; k<=slen2; ++k)
		{
			if(matchmap[i][k])
			{
				break;
			}
		}
		if(k>slen2)
		{
			return 0;
		}
	}
	return matchmap[slen1][slen2];
}

void add_slash(char* dirname) {
	int len=strlen(dirname);
    if (dirname[len-1] != '/')
	{
		strcat(dirname, "/");
	}
}

int CtimeMatch(char* fileName) {
	struct stat buf;
	if(c_time > 0)
	{
		if(stat(fileName, &buf) == 0)
		{
			if(difftime(time(NULL), buf.st_ctime)/(60*60*24) > c_time)
			{
				return MATCH;
			}
			return NOT_MATCH;
		}
	}
	else if(c_time < 0)
	{
		if(stat(fileName, &buf) == 0)
		{
			if(difftime(time(NULL), buf.st_ctime)/(60*60*24) < fabs(c_time))
			{
				return MATCH;
			}
			return NOT_MATCH;
		}
	}
	else
	{
		return MATCH;
	}
}

int MtimeMatch(char* fileName) {
	struct stat buf;
	if(m_time > 0)
	{
		if(stat(fileName, &buf) == 0)
		{
			if(difftime(time(NULL), buf.st_mtime)/(60*60*24) > m_time)
			{
				return MATCH;
			}
			return NOT_MATCH;
		}
	}
	else if(m_time < 0)
	{
		if(stat(fileName, &buf) == 0)
		{
			if(difftime(time(NULL), buf.st_mtime)/(60*60*24) < fabs(m_time))
			{
				return MATCH;
			}
			return NOT_MATCH;
		}
	}
	else
	{
		return MATCH;
	}
}

void* trave_dir(void* _thread_arg)
{
	Thread_arg* thread_arg = (Thread_arg*) _thread_arg;
	char search_dir[MAX_SIZE];
	sprintf(search_dir, thread_arg->s);
    DIR *d;//声明一个句柄
    struct dirent *file;//readdir函数的返回值就存放在这个结构体中
	char* next_search_dir = (char*)malloc(MAX_SIZE);
    struct stat buf;
    if(!(d = opendir(search_dir)))
    {
        printf("error opendir %s!!!\n",search_dir);
        return NULL;
    }
	add_slash(search_dir);
	pthread_t threads[100];
	memset(threads, 0, sizeof(threads));
	int t = 0;
    while((file = readdir(d)) != NULL)
    {
		if(file->d_name[0] == '.') continue;	//除去父目录和本目录
		sprintf(next_search_dir, "%s%s", search_dir, file->d_name);

		if(CtimeMatch(next_search_dir) &&
			MtimeMatch(next_search_dir) &&
			strMatch(file->d_name, search_name) )
		{
			sprintf(file_name, next_search_dir);
            printf(RED "%s%c" NONE, file_name, printVar);
		}

		if( (stat(next_search_dir, &buf)==0) &&
			    (S_ISDIR(buf.st_mode)!=0) &&
                (S_ISLNK(buf.st_mode)<=0) &&
			    (!strMatch(next_search_dir, except_name)))
		{
			Thread_arg* next_thread_arg = (Thread_arg*)malloc(sizeof(Thread_arg));
			if(use_thread)
			{
				sprintf(next_thread_arg->s, next_search_dir);
				pthread_create(&threads[t++], NULL, trave_dir, next_thread_arg);
			}
			else
			{
				sprintf(next_thread_arg->s, next_search_dir);
				trave_dir((void*)next_thread_arg);
			}
		}
	}
	for(int i = 0; i < t; i++) {
		pthread_join(threads[i], NULL);
	}
    closedir(d);
}
/*
int trave_dir(char* search_dir)
{
    DIR *d;//声明一个句柄
    struct dirent *file;//readdir函数的返回值就存放在这个结构体中
	char* next_search_dir = (char*)malloc(MAX_SIZE);
    struct stat buf;
    if(!(d = opendir(search_dir)))
    {
        printf("error opendir %s!!!\n",search_dir);
        return -1;
    }
	add_slash(search_dir);
    while((file = readdir(d)) != NULL)
    {
		if(file->d_name[0] == '.') continue;	//除去父目录和本目录
		sprintf(next_search_dir, "%s%s", search_dir, file->d_name);

		if(CtimeMatch(next_search_dir) &&
			MtimeMatch(next_search_dir) &&
			strMatch(file->d_name, search_name) )
		{
			sprintf(file_name[file_count++], next_search_dir);
		}

		if( (stat(next_search_dir, &buf)==0) &&
			(S_ISDIR(buf.st_mode)!=0) &&
			(!strMatch(next_search_dir, except_name)))
		{
			trave_dir(next_search_dir);
		}
	}
    closedir(d);
    return 0;
}
*/
void Print(void)
{
	printf(GREEN "search_dir:\t%s\n" NONE, dir_name);
	printf(GREEN "search_name:\t%s\n" NONE, search_name);
	if(use_thread) printf(GREEN "use_thread:\t%s\n" NONE, "true");
	else  printf(GREEN "use_thread:\t%s\n" NONE, "false");
	printf(GREEN "ctime:\t%d\n" NONE, c_time);
	printf(GREEN "mtime:\t%d\n" NONE, m_time);

	printf(GREEN "search_result:\n" NONE);
    printf("\n");
}

int main(int argc, char** argv)
{
	init();
	if(parse_command(argc, argv) != 0)
	{
		perror("Can't parse command\n");
		return -1;
	}
	Print();
	sprintf(thread_arg->s, dir_name);

    gettimeofday(&start, NULL);
    trave_dir((void*)thread_arg);
	gettimeofday(&end, NULL);
    double interval = (end.tv_sec - start.tv_sec)*1000 + (double)(end.tv_usec-start.tv_usec)/1000;

	printf("Cost Time:\t%f ms\n", interval);

    return 0;
}
