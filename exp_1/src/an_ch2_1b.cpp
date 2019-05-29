#include <stdio.h>
#include <unistd.h>
#include <time.h>

char* getCurrentTime(void)
{
    time_t timer;
    struct tm *tblock;
    timer = time(NULL);
    tblock = localtime(&timer);
    return asctime(tblock);
}

int main(int argc, char** argv)
{
	while(1)
	{
		printf("Those output come from child, %s", getCurrentTime());
		sleep(1);
	}
}
