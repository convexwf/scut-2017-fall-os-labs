#include<sys/types.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<stdio.h>

int main(int argc ,char*argv[])
{
    if(argc<2)
	{
        perror("No Program\n");
		return -1;
    }
	else
	{
        char command[256];
        strcpy(command,argv[1]);
        int i;
        for( i=2;i<argc;i++){
            strcat(command," ");
            strcat(command,argv[i]);
        }

        //获取系统时间
        struct timeval start, end;
        gettimeofday(&start, NULL);
        system(command);
        gettimeofday(&end, NULL);

        //错误
        if(errno!=0)
		{
          printf("error:%s\n",strerror(errno));
          exit(0);
        }
		else
		{
            struct rusage ru;
            memset(&ru,0,sizeof(struct rusage));
            getrusage(RUSAGE_CHILDREN,&ru);


            //输出结果
            printf("\n\n...................程序性能分析............................\n");

            double interval = (end.tv_sec - start.tv_sec)*1000 + (double)(end.tv_usec-start.tv_usec)/1000;

            printf("命令执行时间为%fms\n",interval);

            printf("用户态执行时间为%fms\t系统态执行为%fms\n",
                       (double)ru.ru_utime.tv_usec/1000+ru.ru_utime.tv_sec*1000,(double)ru.ru_stime.tv_usec/1000+ru.ru_stime.tv_sec*1000);

            printf("进程被抢占的次数为%ld\n",ru.ru_nivcsw);
            printf("进程主动放弃CPU的次数为%ld\n",ru.ru_nvcsw);
            printf("进程执行过程中发生缺页的次数为%ld\n",ru.ru_majflt);
        }
        return 0;
    }
}
