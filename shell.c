#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFSIZE 100

int LaunchProcess(char* string[]){
    int ex;
    pid_t pid;
    if ((pid = fork()) < 0) { // error
        perror("fork");
    } else if (pid == 0) { // in child process
        ex = execvp(string[0], &string[0]);
        if (ex == -1) {
            perror(string[0]);
        } //if
    } else {
        wait(NULL);
    } //if
    return 1;
} //LaunchProcess


int ChangeDir(char* string[]) {
    if (string[1] == NULL) {
        chdir("..");
        chdir("..");
    } //if
    const char* elemone = string[1];
    if (strcmp(elemone, ".") == 0) {
        return 0;
    } else if (strcmp(elemone, "..") == 0) {
        chdir("..");
    } else {
        chdir(elemone);
    } //else
    return 1;
} //ChangeDir

int main(int argc, char* argv[]) {
    const char* homedir = getenv("HOME");
    chdir(homedir);

    char* string[120];
    int n;
    int count = 0;
    int flag = 0;
    char path[BUFFSIZE];



    while (homedir[count] != '\0') {
        count = count + 1;
    } //while



    while (flag != 1) {
        getcwd(path, BUFFSIZE);
        printf("1730sh:");
        //int count = 0;
        int numspaces = 0;
        char* buf = malloc(BUFFSIZE);
        setbuf(stdout, NULL);

        //change so it works if it includes the home directory
        if (strcmp(path, (getenv("HOME"))) == 0 ) {
            printf("~");
        } else {
            char* ret = strstr(path, (getenv("HOME")));
            if (ret) {
                printf("~");
                printf("%d\n", count);
                while (path[count] != '\0') {
                    printf("%c", path[count]);
                    count++;
                } //while
            } else {
                printf("%s", path);
            } //else
// char *token = strtok(path, getenv("HOME"));
            //printf(token);
        } //else

        printf("$ ");

        if ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
            //counts how many spaces there are
            for (int i = 0; i < n; i++) {
                if (buf[i] == ' ') {
                    numspaces++;
                } //if
            } //for
        } //if

        const char delim[3] = " \n";

        char * token = strtok(buf, delim);

        //delim the string of args the user has inputted
        while(token != NULL ) {
            (string[count]) = token;
            count++;
            token = strtok(NULL, delim);
        } //while
        (string[numspaces + 1]) = NULL;

        if (strcmp(string[0], "exit") == 0 ) {
            free(buf);
            exit(0);
        } //if

        if (strcmp(string[0], "cd") == 0) {
            ChangeDir(string);
        } else {
            LaunchProcess(string);
        } //else
/*
        //for loop to go through all elements
        //if statement to see if one of then has a <
        int fdinput = open("input.txt", O_RDONLY);
        dup2(fdi, STDIN_FILENO);

        // could also use O_APPEND instead of O_TRUNC, as needed
        int fdoutput = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(fdo, STDOUT_FILENO);
*/

        free(buf);
    } //while
    return EXIT_SUCCESS;
}//main
