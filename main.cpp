#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

//who | sort | uniq -c | sort -nk1

int main()
{

    int fd1[2];
    pipe(fd1);

    if (!fork())
    {
        close(STDOUT_FILENO);
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[0]);
        close(fd1[1]);

        execlp("who", "who", NULL);

    }
    else
    {
        int fd2[2];
        pipe(fd2);

        if (!fork())
        {
            close(STDIN_FILENO);
            dup2(fd1[0], STDIN_FILENO);

            close(fd1[0]);
            close(fd1[1]);

            close(STDOUT_FILENO);
            dup2(fd2[1], STDOUT_FILENO);
            close(fd2[0]);
            close(fd2[1]);

            execlp("sort", "sort", NULL);
        }
        else
        {
            int fd3[2];
            pipe(fd3);

            if (!fork())
            {
                close(STDIN_FILENO);
                dup2(fd2[0], STDIN_FILENO);

                close(fd1[0]);
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);

                close(STDOUT_FILENO);
                dup2 (fd3[1], STDOUT_FILENO );

                close(fd3[0]);
                close(fd3[1]);

                execlp("uniq", "uniq", "-c", NULL);
            }
            else
            {
                close(fd1[0]);
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);

                close(STDIN_FILENO);
                dup2(fd3[0], STDIN_FILENO);
                close(fd3[0]);
                close(fd3[1]);

                mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
                int fid = open ( "/home/box/result.out", O_CREAT | O_WRONLY, mode);
                close(STDOUT_FILENO);
                dup2 ( fid, STDOUT_FILENO );

                close(fd3[0]);
                close(fd3[1]);
                close (fid);

                execlp("sort", "sort", "-nk1", NULL);
            }
        }
    }




    return 0;

}