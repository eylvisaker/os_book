/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_LINE		80 /* 80 chars per line, per command */

int main(void)
{
	char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
   	int should_run = 1;
		
    char last_input[1000];
    memset(last_input, 0, sizeof(last_input));
    memset(args, 0, sizeof(args));

   	while (should_run){   
       	printf("osh>");
       	fflush(stdout);

        char input[1000];

	    fgets(input, sizeof(input), stdin);

        // history feature
        if (strcmp(input, "!!\n") == 0) {
            if (strcmp(last_input, "") == 0) {
                printf("No commands in history.\n");
                continue;
            }
            strcpy(input, last_input);
            printf("osh>%s", input);
        }
        else {
            strcpy(last_input, input);
        }

        int arg_start=0;
        int argc = 0;
    	for(int i = 0; input[i] != 0; i++)
        {
            if (input[i] == ' ' || input[i] == '\n') {
                int len = i - arg_start;
                if (len <= 0) { continue; }
               
                args[argc] = malloc(len + 1);
                strncpy(args[argc], &input[arg_start], len);
               
                args[argc][len] = 0;

                arg_start = i+1;
                argc++;
            }
        }       

        if (argc == 0) { continue; }

        if (strcmp(args[0], "exit") == 0) {
           printf("exiting...\n");
           should_run = 0;
           exit(0);
           break; 
        }

        int run_in_background = !(strcmp(args[argc - 1], "&"));

        if (run_in_background) { 
            // printf("Not waiting for any child of mine...\n");
            argc --;
            free(args[argc]);
            args[argc] = 0;
        }
        
        int pid = fork();

        if (pid < 0) {
            printf("Failed to fork.\n");
            exit(1);
        }

        if (pid == 0) {
            for(int i = 0; i < argc; i++) {
                if (strcmp(args[i], ">") == 0) {
                    if (argc < i+1) {
                        printf("No output file specified.\n");
                        exit(2);
                    }

                    argc = i - 1;
                    args[i] = 0;
                    int fd = open(args[i+1], O_CREAT | O_RDWR, S_IRWXU);
                    dup2(fd, STDOUT_FILENO);
                    break;
                }
                else if (strcmp(args[i], "<") == 0) {
                    if (argc < i+1) {
                        printf("No input file specified.\n");
                        exit(2);
                    }

                    argc = i - 1;
                    args[i] = 0;
                    int fd = open(args[i+1], O_RDONLY);
                    dup2(fd, STDIN_FILENO);
                    break;
                }
            }

            /*printf("%i arguments\n", argc);
            for(int i = 0; args[i] != 0; i++) {
                printf("%s ", args[i]);
            }
            printf("\n");*/        

            int err = execvp(args[0], args);
            if (err == -1) {
               printf("Error: %s\n", strerror(errno));
            }
            should_run = 0;
        }
        else {
            if (!run_in_background) {
                wait(NULL);
            }
        }

        for(int i = 0; i < argc; i++) {
            free(args[i]);
            args[i] = 0;
        }
    }  
	return 0;
}
