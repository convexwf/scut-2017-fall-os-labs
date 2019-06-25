#include "QShell.h"
#include "color.h"

int main(int argc, char** argv)
{
    init();
    while(true)
    {
        type_prompt(prompt);
        paraNum = read_command(&command,parameters);
        if(-1 == paraNum)
            continue;
        paraNum--;//count of units in buffer
        if(builtin_command(command, parameters)) continue;
        if(outer_command(command, parameters)) continue;

    }

}

void init() {
    parameters = (char**) malloc(sizeof(char *)*(MAXARG+2));
    buffer = (char*) malloc(sizeof(char) * MAXLINE);
    pwd = getpwuid(getuid());
    getcwd(outer_place,max_path_len);
}
void type_prompt(char *prompt) {

    char hostname[max_name_len];
    char pathname[max_path_len];
    int length;

    getcwd(pathname,max_path_len);
    if(gethostname(hostname,max_name_len)==0)
        sprintf(prompt,L_GREEN "[Qshell]%s@%s:" NONE,pwd->pw_name,hostname);
    else
    sprintf(prompt,L_GREEN "[Qshell]%s@unknown:" NONE,pwd->pw_name);
    length = strlen(prompt);
    if(strlen(pathname) < strlen(pwd->pw_dir) ||
       strncmp(pathname,pwd->pw_dir,strlen(pwd->pw_dir))!=0)
        sprintf(prompt+length, L_BLUE "%s\n" NONE, pathname);
    else
    sprintf(prompt+length, L_BLUE "~%s\n" NONE, pathname+strlen(pwd->pw_dir));
    length = strlen(prompt);
    if(geteuid()==0)
        sprintf(prompt+length,"#");
    else
        sprintf(prompt+length,"$");
    return;
}
int read_command(char **command,char **parameters) {
    printf("%s",prompt);
    char* Res_fgets = fgets(buffer,MAXLINE,stdin);
    if(Res_fgets == NULL)
    {
        printf("\n");
        exit(0);
    }

    if(buffer[0] == '\0')
        return -1;
    char *pStart,*pEnd;
    int count = 0;
    int isFinished = 0;
    pStart = pEnd = buffer;
    while(isFinished == 0)
    {
        while((*pEnd == ' ' && *pStart == ' ') || (*pEnd == '\t' && *pStart == '\t'))
        {
            pStart++;
            pEnd++;
        }

        if(*pEnd == '\0' || *pEnd == '\n')
        {
            if(count == 0)
                return -1;
            break;
        }

        while(*pEnd != ' ' && *pEnd != '\0' && *pEnd != '\n')
            pEnd++;


        if(count == 0)
        {
            char *p = pEnd;
            *command = pStart;
            while(p!=pStart && *p !='/')
                p--;
            if(*p == '/')
                p++;
            //else //p==pStart
            parameters[0] = p;
            count += 2;
        }
        else if(count <= MAXARG)
        {
            parameters[count-1] = pStart;
            count++;
        }
        else
        {
            break;
        }

        if(*pEnd == '\0' || *pEnd == '\n')
        {
            *pEnd = '\0';
            isFinished = 1;
        }
        else
        {
            *pEnd = '\0';
            pEnd++;
            pStart = pEnd;
        }
    }

    parameters[count-1] = NULL;

    return count;
}
int builtin_command(char *command, char **parameters) {
    //extern struct passwd *pwd;
    if(strcmp(command,"exit")==0 || strcmp(command,"quit")==0)
    {
        exit(0);
    }
    else if(strcmp(command,"cd")==0)
    {
        char *cd_path = NULL;

        if(parameters[1] == NULL)
            //make "cd" to "cd .." as in bash
        {
            parameters[1] = (char*)malloc(2 * sizeof(char));
            parameters[1][0]= '~';
            parameters[1][1]= '\0';
        }
        if(parameters[1][0] == '~')
        {
            cd_path = (char*)malloc(strlen(pwd->pw_dir)+strlen(parameters[1]));
            //'~' makes length 1 more,but instead of '\0'
            if(cd_path == NULL)
            {
                printf(L_RED "cd:malloc failed.\n" NONE);
            }
            strcpy(cd_path,pwd->pw_dir);
            strncpy(cd_path+strlen(pwd->pw_dir),parameters[1]+1,strlen(parameters[1]));
            //printf("path with ~:\n%s\n",cd_path);
        }
        else
        {
            cd_path = (char*)malloc(strlen(parameters[1]+1));
            if(cd_path == NULL)
            {
                printf(L_RED "cd:malloc failed.\n" NONE);
            }
            strcpy(cd_path,parameters[1]);
        }
        if(chdir(cd_path)!= 0)
            printf(L_RED "-QShell: cd: %s:%s\n" NONE,cd_path,strerror(errno));
        free(cd_path);
    }
    return 0;
}
int outer_command(char *command, char **parameters) {

    //int length = strlen(parameters);
    int background = 0;
    if(strcmp(parameters[paraNum - 1], "&") == 0) background = 1;

    if(strcmp(command, "doit") == 0)
    {
        //sprintf(command, "%s/doit", outer_place);

        //free(command);
        //command = (char*)malloc(sizeof(char)*100);
        //sprintf(command, "%s/doit", outer_place);
        //printf("%s", parameters[0]);
        int status = 0;
        pid_t child_pid = fork();

        if(child_pid == -1) //Failed to fork
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        }
        else if(child_pid > 0)
        {
            if(!background) waitpid(child_pid, &status, 0);
            return 1;
        }
        else
        {
            //chdir(outer_place);
            char cmd[200];
            sprintf(cmd, "%s/%s",outer_place, command);
            int current_length = strlen(cmd);
            for(int i = 1; i < paraNum; i++)
            {
                sprintf(cmd+current_length, " %s", parameters[i]);
                current_length = strlen(cmd);
            }
            //printf("%s\n", cmd);
            system(cmd);
            //execvp(command, parameters);
        }
    }
    return 0;
}
