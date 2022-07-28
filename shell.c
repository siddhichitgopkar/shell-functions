#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFSIZE 100

int LaunchProcess(char* string[]) {
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


int ChangeDir(char* string[], int count) {
    if (count == 1) {
        chdir("..");
        chdir("..");
    } else {
        const char* elemone = string[1];
        if (strcmp(elemone, ".") == 0) {
            return 0;
        } else if (strcmp(elemone, "..") == 0) {
            chdir("..");
        } else if (strcmp(elemone, "~") == 0) {
            chdir(getenv("HOME"));
        } else {
            if (chdir(elemone) == -1) {
                perror(string[1]);
            } else {
                chdir(elemone);
            } //else
        } //else
    } //else
    return 1;
} //ChangeDir

int main(int argc, char* argv[]) {
    const char* homedir = getenv("HOME");
    chdir(homedir);
    int n;
    int c = 0;
    int flag = 0;
    char path[BUFFSIZE];

    while (homedir[c] != '\0') {
        c = c + 1;
    } //while

    while (flag != 1) {
        char* string[120];
        int count = 0;
        getcwd(path, BUFFSIZE);
        printf("1730sh:");
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
                int tempc = c;
                while (path[tempc] != '\0') {
                    printf("%c", path[tempc]);
                    tempc++;
                } //while
            } else {
                printf("%s", path);
            } //else
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
        for (int i = 0; i <= numspaces; i++) {
            (string[count]) = token;
            count++;
            token = strtok(NULL, delim);
        } //while
        (string[count]) = NULL;

        int in = 0;
        int out = 0;
        int doublein = 0;
        int doubleout = 0;

        int inindex = 0;
        int outindex = 0;
        int doubleinindex = 0;
        int doubleoutindex = 0;

        for (int i = 0; i < count; i++) {
            if (strcmp(string[i],"<") == 0) {
                in = 1;
                inindex = i + 1;
            } //if
            if (strcmp(string[i],">") == 0) {
                out = 1;
                outindex = i + 1;
            } //if
            if (strcmp(string[i],"<<") == 0) {
                doublein = 1;
                doubleinindex = i + 1;
            } //if
            if (strcmp(string[i],">>") == 0) {
                doubleout = 1;
                doubleoutindex = i + 1;
            } //if
        } //for

        int redirectflag = 0;
        if (doubleout == 1 || doublein == 1 || in == 1 || out == 1) {
            redirectflag = 1;
        } //if

        if (count == 0);
        else if (strcmp(string[0], "exit") == 0 ) {
            free(buf);
            exit(0);
        } else if (strcmp(string[0], "cd") == 0) {
            ChangeDir(string, count);
        } else if (redirectflag == 1) {
            if (in == 1) {
                int fdi = open(string[inindex], O_RDONLY);
                dup2(fdi, STDIN_FILENO);
            } //if
        } else {
            LaunchProcess(string);
        } //else
        for (int i = 0; i < count; i++) {
            if (string[i] != NULL) {
                string[i] = NULL;
            } // if
        } //for
        free(buf);
    } //while
    return EXIT_SUCCESS;
}//main
