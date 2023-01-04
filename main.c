#include "headers.h"

Queue *commandqueue;
int status_ = 0;
time_t totaltimespent = 0;
time_t begin = 0, end = 0;

char *backgroundprocesses[1000];
int backgroundprocessesid[1000];
int numberofbackgroundprocess = 0;
int foregroundprocessid = -1;
char foregroundprocessname[100];
int inpre = 1;
int outre = 1;

void handleinputoutputredirection(char **args)
{
    int originalout = 0;
    int originalin = 0;

    dup2(STDOUT_FILENO, originalout);
    dup2(STDIN_FILENO, originalin);

    int i = 1;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], "<") == 0)
        {
            int fd = open(args[i + 1], O_RDONLY);
            dup2(fd, STDIN_FILENO);
            // remove i and i+1 from args and keep args till that point

            // NOW CHECK if THERE IS OUTPUT REDIRECTION
            if (args[i + 2] != NULL && strcmp(args[i + 2], ">") == 0)
            {
                int fd = open(args[i + 3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            }
            else if (args[i + 2] != NULL && strcmp(args[i + 2], ">>") == 0)
            {
                int fd = open(args[i + 3], O_WRONLY | O_APPEND);
                dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            }
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            int fd = open(args[i + 1], O_WRONLY | O_APPEND);
            dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            args[i] = NULL;
        }
        i++;
    }

    dup2(originalout, STDOUT_FILENO);
    dup2(originalin, STDIN_FILENO);
}

int execute(char *command, char **args, int background) // THIS GETS CALLED IN CHILD PROCESS !!!!!!
{
    int originalout = 0;
    int originalin = 0;

    if (outre)
        dup2(STDOUT_FILENO, originalout);
    if (inpre)
        dup2(STDIN_FILENO, originalin);

    int i = 1;
    int fd;
    while (args[i] != NULL)
    {
        if (inpre && (strcmp(args[i], "<") == 0))
        {
            fd = open(args[i + 1], O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
            // remove i and i+1 from args and keep args till that point

            // NOW CHECK if THERE IS OUTPUT REDIRECTION
            if (outre && args[i + 2] != NULL && strcmp(args[i + 2], ">") == 0)
            {
                fd = open(args[i + 3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
                close(fd);
            }
            else if (outre && args[i + 2] != NULL && strcmp(args[i + 2], ">>") == 0)
            {
                fd = open(args[i + 3], O_WRONLY | O_APPEND);
                dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
                close(fd);
            }
            args[i] = NULL;
            break;
        }
        else if (outre && (strcmp(args[i], ">") == 0))
        {
            fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            args[i] = NULL;
            close(fd);
            break;
        }
        else if (outre && strcmp(args[i], ">>") == 0)
        {
            fd = open(args[i + 1], O_WRONLY | O_APPEND);
            dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            args[i] = NULL;
            close(fd);
            break;
        }
        i++;
    }

    if (strcmp(command, "ls") == 0)
        ls(args);

    else if (strcmp(command, "pinfo") == 0)
        pinfo(args);

    else if (strcmp(command, "discover") == 0)
        discover(args);

    else if (strcmp(command, "jobs") == 0)
        jobs(backgroundprocesses, backgroundprocessesid, numberofbackgroundprocess, args);

    else if (strcmp(command, "sig") == 0)
    {
        int processno = -1;
        int signalnumber = -1;

        processno = atoi(args[1]);
        signalnumber = atoi(args[2]);

        sig(backgroundprocessesid, numberofbackgroundprocess, processno, signalnumber);
    }
    else if (strcmp(command, "bg") == 0)
    {
        int backgroundprocessno = atoi(args[1]);
        bg(backgroundprocessesid, numberofbackgroundprocess, backgroundprocessno);
    }
    else if (strcmp(command, "fg") == 0)
    {
        int backgroundprocessno = atoi(args[1]);

        begin = 0;
        end = 0;
        begin = time(NULL);
        fg(backgroundprocessesid, numberofbackgroundprocess, backgroundprocessno);
        end = time(NULL);
        totaltimespent += end - begin;
    }
    else // FOR EXECVP TYPE COMMANDS
    {
        int forkret = fork();

        // IN PARENT
        if (forkret > 0)
        {
            if (background)
                return forkret; // NO NEED TO WAIT
            else                // if process is to be run in foreground
            {
                foregroundprocessid = forkret;
                strcpy(foregroundprocessname, args[0]);

                signal(SIGTTIN, SIG_IGN);
                signal(SIGTTOU, SIG_IGN); // IGNORING CERTAIN SIGNALS

                tcsetpgrp(0, forkret); // giving foreground control to child process

                begin = 0;
                end = 0;
                begin = time(NULL);
                waitpid(forkret, &status_, WUNTRACED);
                end = time(NULL);
                totaltimespent += end - begin;

                tcsetpgrp(0, getpgid(0)); // REGAINING CONTROL OF FOREGROUND

                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL); // ACTIVATING CERTAIN SIGNALS

                if (outre)
                    dup2(originalout, STDOUT_FILENO);
                if (inpre)
                    dup2(originalin, STDIN_FILENO);

                return forkret; // DOESN'T MATTER WHAT WE RETURN
            }
        }
        else
        {
            setpgid(0, 0);                 // setting process group id of child to its process id
            if (execvp(args[0], args) < 0) // child will exit after this
            {
                perror("Error");
                exit(1);
            }
        }
    }

    if (outre)
        dup2(originalout, STDOUT_FILENO);
    if (inpre)
        dup2(originalin, STDIN_FILENO);

    return 0;
}

void executebuiltin(char *command, char **args) // run as main commands without forking or anything
{
    int originalout = 0;
    int originalin = 0;

    dup2(STDOUT_FILENO, originalout);
    dup2(STDIN_FILENO, originalin);

    int i = 1;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], "<") == 0)
        {
            int fd = open(args[i + 1], O_RDONLY);
            dup2(fd, STDIN_FILENO);
            // remove i and i+1 from args and keep args till that point

            // NOW CHECK if THERE IS OUTPUT REDIRECTION
            if (args[i + 2] != NULL && strcmp(args[i + 2], ">") == 0)
            {
                int fd = open(args[i + 3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            }
            else if (args[i + 2] != NULL && strcmp(args[i + 2], ">>") == 0)
            {
                int fd = open(args[i + 3], O_WRONLY | O_APPEND);
                dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            }
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            int fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">>") == 0)
        {
            int fd = open(args[i + 1], O_WRONLY | O_APPEND);
            dup2(fd, STDOUT_FILENO); // STDOUT CLOSES
            args[i] = NULL;
        }
        i++;
    }

    if (strcmp(command, "echo") == 0)
        echo(&args[1]);

    else if (strcmp(command, "cd") == 0)
        cd(args);

    else if (strcmp(command, "pwd") == 0)
        pwd();

    dup2(originalout, STDOUT_FILENO);
    dup2(originalin, STDIN_FILENO);
}

void bghandler()
{
    int pid = -1;
    if ((pid = waitpid(-1, &status_, WNOHANG)) > 0) // get a signal from all bg processes
    {
        // get the process name
        int found = 0;
        char *processname = (char *)malloc(1000);
        for (int i = 0; i < numberofbackgroundprocess; i++)
        {
            if (backgroundprocessesid[i] == pid)
            {
                strcpy(processname, backgroundprocesses[i]);
                found = 1;
                break;
            }
        }

        if (found)
        {
            fprintf(stderr, "%s with PID %d exited %s\n", processname, pid, WIFEXITED(status_) ? "normally" : "abnormally");
            prompt();
            free(processname);
            numberofbackgroundprocess--;
        }
    }
    return;
}

void executeall(char *input)
{
    // TOKENISING INPUT
    char delimiters[20] = " \t";
    char *token = strtok(input, delimiters); // taking care of space and tabs

    while (token != NULL) // till whole string is read
    {
        char *args[1000];
        int i = 0;
        while ((token != NULL) && (strcmp(token, "&") != 0) && (strcmp(token, ";") != 0))
        {
            args[i] = token;
            token = strtok(NULL, delimiters);
            i++;
        }
        args[i] = NULL;

        if (token != NULL)
        {
            // built - in command , without invoking fork
            // pid_t forkReturn = -2;

            if ((strcmp(args[0], "cd") == 0) || (strcmp(args[0], "echo") == 0) || (strcmp(args[0], "pwd") == 0)) // EXECUTED IN SHELL PROCESS ONLY
                executebuiltin(args[0], args);

            else if (strcmp(args[0], "history") == 0) // EXECUTED IN SHELL PROCESS ONLY
                history(commandqueue);

            else
            {
                if (strcmp(token, "&") == 0) // IN SHELL PROCESS ONLY , ONLY WITH EXECVP TYPE COMMANDS
                {
                    backgroundprocesses[numberofbackgroundprocess] = (char *)malloc(sizeof(args[0]));
                    strcpy(backgroundprocesses[numberofbackgroundprocess], args[0]);
                    backgroundprocessesid[numberofbackgroundprocess] = execute(args[0], args, 1); // EXECUTED IN SHELL PROCESS ONLY;
                    printf("[%d] %d\n", numberofbackgroundprocess, backgroundprocessesid[numberofbackgroundprocess]);
                    numberofbackgroundprocess++;
                }

                if (strcmp(token, ";") == 0) // means it is a foreground process and we have to wait for it
                {
                    int pid = execute(args[0], args, 0); // SIMPLY EXECUTE
                    printf("(%d) executed %s\n", pid, args[0]);
                }
            }

            // OUR OLD C - PROGRAM CONTINUES
            token = strtok(NULL, "  \t");
        }
        else // it is a foreground process and we have to wait for it
        {
            if ((strcmp(args[0], "cd") == 0) || (strcmp(args[0], "echo") == 0) || (strcmp(args[0], "pwd") == 0))
                executebuiltin(args[0], args);

            else if (strcmp(args[0], "history") == 0)
                history(commandqueue);

            else
            {
                int pid = execute(args[0], args, 0); // SIMPLY EXECUTE
            }
        }
    }
    return;
}

void handlepipes(char *input)
{
    // given an input , check the number of pipes
    char delimiters_pipe[20] = "|";
    char *processes[100];
    int num_proc = 0;
    char *token_pipe = strtok(input, delimiters_pipe); // dividing input into processes
    while (token_pipe != NULL)
    {
        processes[num_proc] = token_pipe;
        token_pipe = strtok(NULL, delimiters_pipe);
        num_proc++;
    }

    // HANDLE CASE WHEN NO PIPE IS PRESENT !!!!
    if (num_proc == 1) // no need to pipe
    {
        executeall(input);
        return;
    }

    inpre = 0;
    outre = 0;
    int originalout = 0; // clone of stdout
    dup2(STDOUT_FILENO, originalout);

    int originalin = 0; // clone of stdin
    dup2(STDIN_FILENO, originalin);

    for (int i = 0; i < num_proc - 1; i++)
    {
        int fildes[2];
        if (pipe(fildes) < 0)
        {
            perror("Error in piping");
            exit(1);
        }

        if (dup2(fildes[1], STDOUT_FILENO) < 0)
        {
            perror("Error in duping");
            exit(1);
        } // redirecting output to fildes[1]

        // check for input/output redirection
        for (size_t j = 0; j < strlen(processes[i]); j++) // ONLY 1ST PROCESS CAN HAVE INPUT REDIRECTION
        {
            if (processes[i][j] == '<')
                inpre = 1;
        }
        executeall(processes[i]);
        inpre = 0;

        dup2(originalout, STDOUT_FILENO); // restoring STDOUT
        close(fildes[1]);                 // CLOSING WRITE END OF PIPE

        dup2(fildes[0], STDIN_FILENO); // redirecting input to pipe[0] for next process
        close(fildes[0]);              // CLOSING READ END OF PIPE
    }
    for (size_t j = 0; j < strlen(processes[num_proc - 1]); j++) // ONLY LAST PROCESS CAN HAVE OUTPUT REDIRECTION
    {
        if (processes[num_proc - 1][j] == '>')
            outre = 1;
    }
    executeall(processes[num_proc - 1]);
    dup2(originalin, STDIN_FILENO);
    inpre = 1;
    outre = 1;
    return;
}

void die(const char *s)
{
    perror(s);
    exit(1);
}

struct termios orig_termios;

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);

    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

char *takeinput()
{
    char *inp = malloc(sizeof(char) * 100);
    char c;

    setbuf(stdout, NULL);
    enableRawMode(); // PROBLEM HERE !!!
    prompt();

    if (totaltimespent >= 1)
    {
        printf("\033[1;36m took %lds \033[0m", totaltimespent);
        totaltimespent = 0;
    }
    memset(inp, '\0', 100);
    int pt = 0; // LENGTH OF INPUT STRING
    while (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (iscntrl(c))
        {
            if (c == 10) // ENTER KEY
                break;
            else if (c == 127)
            { // backspace
                if (pt > 0)
                {
                    if (inp[pt - 1] == 9) // BACKSPACE FROM TAB
                    {
                        for (int i = 0; i < 7; i++)
                        {
                            printf("\b");
                        }
                    }
                    inp[--pt] = '\0';
                    printf("\b \b");
                }
            }
            else if (c == 9)
            { // TAB character

                // IMPLEMENT AUTOCOMPLETION HERE
                char wordtosearch[100];
                int j;
                for (j = pt - 1; j > -1; j--)
                {
                    if (inp[j] == 32) // detecting space bar
                        break;
                }
                strncpy(wordtosearch, &inp[j + 1], pt - j); // hi i am
                wordtosearch[pt - j - 1] = '\0';

                DIR *pdr = opendir(".");

                char *addcompleted = (char *)malloc(100);
                memset(addcompleted, '\0', 100);
                int lc = 0; // length of completed word

                int countopts = 0;
                struct direct *opts[1000];
                struct direct *pde;
                while ((pde = readdir(pdr)) != NULL)
                {
                    if (strncmp(pde->d_name, wordtosearch, strlen(wordtosearch)) == 0)
                    {
                        opts[countopts] = pde;
                        countopts++;
                    }
                }

                // completing addcomplete
                for (size_t i = strlen(wordtosearch); i < strlen(opts[0]->d_name); i++)
                {
                    int temp = 1;
                    for (int j = 1; j < countopts; j++)
                    {
                        if ((opts[0]->d_name)[i] != (opts[j]->d_name)[i])
                        {
                            temp = 0;
                            break;
                        }
                    }
                    if (!temp)
                        break;
                    addcompleted[lc++] = (opts[0]->d_name)[i];
                }

                for (int i = 0; i < lc; i++)
                    inp[pt++] = addcompleted[i];

                if (countopts != 1)
                {
                    printf("\n");
                    for (int i = 0; i < countopts; i++)
                    {
                        printf("%s", opts[i]->d_name);

                        struct stat fileStat;
                        stat(opts[i]->d_name, &fileStat);
                        if (S_ISDIR(fileStat.st_mode))
                            printf("/");
                        printf("\n");
                    }
                }

                printf("\n");
                prompt();
                printf("%s", inp);
            }
            else if (c == 4) // for CTRL-D
            {
                exit(1);
            }
        }
        else
        {
            inp[pt++] = c;
            printf("%c", c);
        }
    }
    printf("\n");
    disableRawMode();

    return inp;
}

void ctrlChandler()
{
    printf("\n");
    prompt();
}

void ctrlZhandler()
{
    if (foregroundprocessid != -1)
    {
        backgroundprocessesid[numberofbackgroundprocess] = foregroundprocessid;
        strcpy(backgroundprocesses[numberofbackgroundprocess++], foregroundprocessname);
        kill(foregroundprocessid, 20);

        foregroundprocessid = -1;
        foregroundprocessname[0] = '\0';
    }
}

int main()
{
    signal(SIGCHLD, bghandler); // WILL TELL US IF A PROCESS HAS EXITED
    signal(SIGTSTP, ctrlZhandler);
    signal(SIGINT, ctrlChandler);

    initialise();

    commandqueue = createqueue(); // created queue
    char historypath[1000];
    strcpy(historypath, initdir);
    strcat(historypath, "/history.txt");

    FILE *historyfile = fopen(historypath, "r");
    // initialising our queue
    char line[100];
    while (fgets(line, 100, historyfile) != NULL)
        enqueue(line, commandqueue); // NOT ! addtoqueue

    fclose(historyfile);

    char input[100];

    while (1) // taking input
    {
        int wrhis = 0;

        char *inp = takeinput(); // TOOK INPUT
        strcpy(input, inp);
        free(inp);

        totaltimespent = 0;

        // MAINTAINING HISTORY
        wrhis = enqueue(input, commandqueue); // ADDING TO COMMAND QUEUE
        historyfile = fopen(historypath, "a");
        if (wrhis == 1) // on successful operation , just append to file
            fprintf(historyfile, "%s\n", input);
        else if (wrhis == -1) // do nothing
            ;
        else
        {
            fclose(historyfile);
            historyfile = fopen(historypath, "w"); // to clear and rewrite
            dequeue(commandqueue);
            wrhis = enqueue(input, commandqueue);

            Queuenode *currnode = commandqueue->head;
            while (currnode != NULL) // write new content to file
            {
                fprintf(historyfile, "%s\n", currnode->command);
                currnode = currnode->nextnode;
            }
        }
        fclose(historyfile);

        handlepipes(input); // WHERE ALL THE ACTION IS
    }
    return 1;
}