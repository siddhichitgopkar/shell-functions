#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUFFSIZE 100

/**
 * Creates a child process in order to execute a command
 * supplied as the argument.
 * @arg string command to be executed
 * @return 1 if successful
 */
int LaunchProcess(char* string[]) {
    int ex;
    pid_t pid;
    // Creates a child process to run the command
    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) {
        // Executes the given command
        ex = execvp(string[0], &string[0]);
        // Displays error if execute fails
        if (ex == -1) {
            perror(string[0]);
        } //if
    } else {
        // Waits for child process to complete before returning
        wait(NULL);
    } //if
    return 1;
} //LaunchProcess

/**
 * Creates a child process in order to execute a command
 * and append the result to the supplied file.
 *
 * @arg string command to be executed
 * @arg filename name of file to store result in
 * @arg doubleoutput file descriptor of filename
 * @return 1 if successful
 */
int DupLaunchDoubleOut(char* string[], char* filename, int doubleoutput) {
    pid_t pid;
    int fd;
    // Creates a child process to run the command
    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) {
        fd = open(filename, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
        // Makes the stdout go to file
        dup2(fd, 1);
        close(fd);
        // Executes the given command
        execvp(string[0], &string[0]);
        // Displays the error if execute fails
        if (execvp(string[0], &string[0]) == -1) {
            perror(string[0]);
        } //if
    } else {
        // Waits for child process to complete before returning
        wait(NULL);
    } //if
    return 1;
} // DupLaunchDoubleOut

/**
 * Creates a child process in order to execute a command
 * and overwrite the supplied file with the result.
 *
 * @arg string command to be executed
 * @arg filename name of file to store result in
 * @arg doubleoutput file descriptor of filename
 * @return 1 if successful
 */
int DupLaunchOut(char* string[], char* filename, int output) {
    pid_t pid;
    int fd;
    // Creates a child process to run the command
    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) {
        fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        // makes the stdout go to file
        dup2(fd, 1);
        close(fd);
        // Executes the given command
        execvp(string[0], &string[0]);
        // Displays the error if execute fails
        if (execvp(string[0], &string[0]) == -1) {
            perror(string[0]);
        } //if
    } else {
        // Waits for child process to complete before returning
        wait(NULL);
    } //if
    return 1;
} //DupLaunchOut

/**
 * Creates a child process in order to execute the command
 * supplied by the given file.
 *
 * @arg string command to be executed
 * @arg filename name of file storing the command
 * @arg doubleoutput file descriptor of filename
 * @return 1 if successful
 */
int DupLaunchIn(char* string[], char * file, int input) {
    pid_t pid;
    // Creates a child process to run the command
    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) {
        // Executes the given command
        execvp(string[0], &string[0]);
        // Displays the error if execute fails
        if (execvp(string[0], &string[0]) == -1) {
            perror(string[0]);
        } //if
    } else {
        // Waits for child process to complete before returning
        wait(NULL);
    } //if
    return 1;
} //DupLaunchIn


/**
 * Changes the directory of the shell based on supplied command.
 *
 * @arg string command to be executed
 * @arg count number of strings in string array
 * @return 1 if successful
 */
int ChangeDir(char* string[], int count) {
    // Goes back 2 directories if only supplied "cd"
    if (count == 1) {
        chdir("..");
        chdir("..");
    } else {
        const char* elemone = string[1];
        // Does nothing if supplied "cd ."
        if (strcmp(elemone, ".") == 0) {
            return 0;
            // Goes back 1 directory if supplied "cd .."
        } else if (strcmp(elemone, "..") == 0) {
            chdir("..");
            // Goes to home directory if supplied "cd ~"
        } else if (strcmp(elemone, "~") == 0) {
            chdir(getenv("HOME"));
        } else {
            // Displays error if chdir fails
            if (chdir(elemone) == -1) {
                perror(string[1]);
            } else {
                // Changes to supplied directory
                chdir(elemone);
            } //else
        } //else
    } //else
    return 1;
} //ChangeDir

/**
 * Acts as a shell that supports any command, including
 * cd (change directory), as well as input and output redirection.
 *
 * @arg argc number of command line arguments
 * @arg argv array of command line arguments
 */
int main(int argc, char* argv[]) {
    //Starts in home directory
    const char* homedir = getenv("HOME");
    chdir(homedir);
    int n;
    int c = 0;
    // Flag that tells how long to keep asking the user for args
    int flag = 0;
    char path[BUFFSIZE];

    // Calculates how long the home directory is
    while (homedir[c] != '\0') {
        c = c + 1;
    } //while

    while (flag != 1) {
        // Array that holds the user inputted args
        char* string[120];
        int count = 0;
        // Sets path to the cwd
        getcwd(path, BUFFSIZE);
        printf("1730sh:");
        int numspaces = 0;
        char* buf = malloc(BUFFSIZE);
        setbuf(stdout, NULL);

        // Prints out the appropriate prompt based on the pwd and user input
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

        // Counts to see how many spaces there are
        if ((n = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
            // Counts how many spaces there are
            for (int i = 0; i < n; i++) {
                if (buf[i] == ' ') {
                    numspaces++;
                } //if
            } //for
        } //if

        // Deliminator that is being used to separate the workds
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
            int doubleout = 0;

            int inindex = 0;
            int outindex = 0;
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
                if (strcmp(string[i],">>") == 0) {
                    doubleout = 1;
                    doubleoutindex = i + 1;
                } //if
            } //for

            //checks to see if there is a redirection in the arguments the user inputted
            int redirectflag = 0;
            if (doubleout == 1 || in == 1 || out == 1) {
                redirectflag = 1;
            } //if

            // Starts while loop over if nothing is supplied
            if (count == 0);
            // Exits program if exit is supplied
            else if (strcmp(string[0], "exit") == 0) {
                free(buf);
                exit(0);
                // Calls change directory if cd is supplied
            } else if (strcmp(string[0], "cd") == 0) {
                ChangeDir(string, count);
                // Checks for redirection if a symbol for it is supplied
            } else if (redirectflag == 1) {
                // Redirects output
                if (out == 1) {
                    // Creates new file or resets existing one
                    int output = creat(string[outindex], 0644);
                    char *outstring[outindex - 1];
                    // Creates array of words after redirection symbol
                    for(int i = 0; i < outindex - 1; i++) {
                        outstring[i] = string[i];
                    } //for
                    char * filename;
                    outstring[outindex - 1] = NULL;
                    filename = string[outindex];
                    DupLaunchOut(outstring, filename, output);
                } //if
                // Redirects output to append to file
                if(doubleout == 1) {
                    int doubleoutput = open(string[doubleoutindex], O_CREAT | O_WRONLY
                    | O_EXCL, S_IRUSR | S_IWUSR);
                    // Checks if file already exits and appends to it if so
                    if (doubleoutput < 0) {
                        if (errno == EEXIST) {
                            doubleoutput = open(string[doubleoutindex], S_IRUSR | S_IWUSR);
                            if (doubleoutput < 0) {
                                perror("open");
                            } // if
                            char *doubleoutstring[doubleoutindex - 1];
                            // Creates array of words after redirection symbol
                            for(int i = 0; i < doubleoutindex - 1; i++) {
                                doubleoutstring[i] = string[i];
                            } //for
                            char * filename;
                            doubleoutstring[doubleoutindex - 1] = NULL;
                            filename = string[doubleoutindex];
                            DupLaunchDoubleOut(doubleoutstring, filename, doubleoutput);
                        } // if
                    } else {
                        // Makes new file if it does not already exist and adds to it
                        char *doubleoutstring[doubleoutindex - 1];
                        // Creates array of words after redirection symbol
                        for(int i = 0; i < doubleoutindex - 1; i++) {
                            doubleoutstring[i] = string[i];
                        } //for
                        char * filename;
                        doubleoutstring[doubleoutindex - 1] = NULL;
                        filename = string[doubleoutindex];
                        DupLaunchOut(doubleoutstring, filename, doubleoutput);
                    } // if
                } // if
                // Redirects input
                if (in == 1) {
                    int input = open(string[inindex], 0644);
                    char *instring[inindex + 2];
                    // Creates array of words after redirection symbol
                    for(int i = 0; i < inindex - 1; i++) {
                        instring[i] = string[i];
                    } //for
                    instring[inindex - 1] = string[inindex];
                    instring[inindex] = NULL;
                    char * file = string[inindex];
                    DupLaunchIn(instring, file, input);
                } //if
            } else {
                // If no flags or keywords exist then executes command
                LaunchProcess(string);
            } //else
            // Resets string array for next input
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
