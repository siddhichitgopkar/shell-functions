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


int DupLaunchOut(char* string[], char* filename) {
    pid_t pid;
    int fd;
    if ((pid = fork()) < 0) { // error
        perror("fork");
    } else if (pid == 0) { // in child process
        fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        dup2(fd, 1);   // makes the stdout go to file
        close(fd);
        execvp(string[0], &string[0]);
        if (execvp(string[0], &string[0]) == -1) {
            perror(string[0]);
        } //if
    } else {
        wait(NULL);
    } //if
    return 1;
} //DupLaunchOut


int DupLaunchIn(char* string[], char * file) {
    pid_t pid;
    if ((pid = fork()) < 0) { // error
        perror("fork");
    } else if (pid == 0) { // in child process
        execvp(string[0], &string[0]);
        if (execvp(string[0], &string[0]) == -1) {
            perror(string[0]);
        } //if
    } else {
        wait(NULL);
    } //if
    return 1;
} //DupLaunchIn



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
    const char* homedir = getenv("HOME"); //homedirectory set
    chdir(homedir); //changes to the home directory when the program is executed
    int n;
    int c = 0;
    int flag = 0; //flag that tells how long to keep asking the user for args
    char path[BUFFSIZE]; //declares path

    //calculates how long the home directory is
    while (homedir[c] != '\0') {
        c = c + 1;
    } //while

    while (flag != 1) {
        char* string[120]; //array that holds the user inputted args
        int count = 0;
        getcwd(path, BUFFSIZE); //sets path to the cwd
        printf("1730sh:");
        int numspaces = 0;
        char* buf = malloc(BUFFSIZE);
        setbuf(stdout, NULL);

        //prints out the appropriate prompt based on the pwd and user input
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

        //counts to see how many spaces there are
        if ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
            //counts how many spaces there are
            for (int i = 0; i < n; i++) {
                if (buf[i] == ' ') {
                    numspaces++;
                } //if
            } //for
        } //if

        //the deliminator that is being used to separate the workds
        const char delim[3] = " \n";

        //token to help with deliminating the strings
        char * token = strtok(buf, delim);

        //delim the string of args the user has inputted
        for (int i = 0; i <= numspaces; i++) {
            (string[count]) = token;
            if (string[0] == NULL) {
                break;
            } //if
            count++;
            token = strtok(NULL, delim);
        } //for
        if (string[0] == NULL);
        else {
            (string[count]) = NULL;


            //variables to check for redirection
            int in = 0;
            int out = 0;
            int doublein = 0;
            int doubleout = 0;

            int inindex = 0;
            int outindex = 0;
            int doubleinindex = 0;
            int doubleoutindex = 0;


            //checks to see if there are any and where the redirections are
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

            //checks to see if there is a redirection in the arguments the user inputted
            int redirectflag = 0;
            if (doubleout == 1 || doublein == 1 || in == 1 || out == 1) {
                redirectflag = 1;
            } //if

            if (count == 0);
            else if (strcmp(string[0], "exit") == 0) {
                free(buf);
                exit(0);
            } else if (strcmp(string[0], "cd") == 0) {
                ChangeDir(string, count);
            } else if (redirectflag == 1) {
                if (out == 1) {
                    int output = creat(string[outindex], 0644);
                    char *outstring[outindex - 1];
                    for(int i = 0; i < outindex - 1; i++) {
                        outstring[i] = string[i];
                    } //for
                    char * filename;
                    outstring[outindex - 1] = NULL;
                    filename = string[outindex];
                    DupLaunchOut(outstring, filename);
                } //if
                if (in == 1) {
                    int input = open(string[inindex], 0644);
                    char *instring[inindex + 2];
                    for(int i = 0; i < inindex - 1; i++) {
                        instring[i] = string[i];
                    } //for
                    instring[inindex - 1] = string[inindex];
                    instring[inindex] = NULL;
                    char * file = string[inindex];
                    DupLaunchIn(instring, file);
                } //if
            } else {
                LaunchProcess(string);
            } //else
            for (int i = 0; i < count; i++) {
                if (string[i] != NULL) {
                    string[i] = NULL;
                } // if
            } //for
        } //else
        free(buf);
    } //while
    return EXIT_SUCCESS;
}//main
