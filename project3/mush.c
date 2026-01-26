/*

Brent Grossman
01/25/2026
cs374
Project 3 - Write a Shell in C

*/

#include <stdio.h>      // printf(), fgets(), perror()
#include <stdlib.h>     // exit()
#include <string.h>     // strtok(), strcmp()
#include <unistd.h>     // fork(), execvp(), chdir()
#include <sys/wait.h>   // wait()

#define MAX_LINE 2048
#define MAX_TOKENS 128



int main(void) {

    char line[MAX_LINE];
    char *tokens[MAX_TOKENS] = {0};
    char *token;
    int i;

    while(1) {

        i = 0; // reset the token counter

        // print prompt
        printf("mush> ");
        fflush(stdout);

        // read user command line input
        fgets(line, sizeof line, stdin);

        // check for end of file signal when user presses ctrl-D
        if(feof(stdin)) {
            exit(0);
        }

        // tokenize
        if ((token = strtok(line, " \n")) != NULL) do {
            tokens[i++] = token;
        } while ((token = strtok(NULL, " \n")) != NULL);
        
        tokens[i] = NULL;  // NULL-terminate the array

        // check for NULL if user hits enter key with no commands
        if(tokens[0] == NULL) {
            continue;
        }

        // check for cd command
        if(strcmp(tokens[0], "cd") == 0) {
            if(tokens[1] == NULL) {
                fprintf(stderr, "usage: cd directory\n");
            }
            else if(chdir(tokens[1]) == -1) {
                perror(tokens[1]);  // prints "dirname: No such file or directory"
            }
            continue;  // skip the fork/exec and go back to prompt
        }
        // check for exit command
        if(strcmp(tokens[0], "exit") == 0) {
            if(tokens[1] == NULL) {
                exit(0);
            }
            else {
                exit(atoi(tokens[1]));
            }
        }

        pid_t pid = fork();

        if(pid == 0) {
            // this is the child, it calls execvp here to replace the current process
            execvp(tokens[0], tokens);
            perror("execvp");
            exit(1);  // if execvp fails, exit(1) kills the child
        }
        else {
            // this is the parent, it calls wait(NULL) to wait for the child to finish, then loops back
            wait(NULL);
        }
    }
}