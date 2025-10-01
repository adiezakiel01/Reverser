#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX 1024

int main() {
    int sample_fd = open("sample.usp", O_RDONLY); /*Open input file*/
    if (sample_fd == -1) 
    {
        write(STDERR_FILENO, "Failed to open sample.usp file\n", 30);
        return 1;
    }

    int result_fd = open("result.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); /*Open output file*/
    if (result_fd == -1) 
    {
        write(STDERR_FILENO, "Failed to open result.txt file\n", 31);
        close(sample_fd);
        return 1;
    }

    char buffer[MAX]; /*Max Line length: 1024*/
    int bytes_read;

    bytes_read = read(sample_fd, buffer, sizeof(buffer));
    if (bytes_read == -1) 
    {
        write(STDERR_FILENO, "Error reading from file\n", 24);
        close(sample_fd);
        close(result_fd);
        return 1;
    }

    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) 
    {
        write(STDERR_FILENO, "Pipe creation failed\n", 21);
        close(sample_fd);
        close(result_fd);
        return 1;
    }

    pid_t pid = fork(); /*Create child process*/

    if (pid == -1) 
    {
        write(STDERR_FILENO, "Fork Failed\n", 12);
        close(sample_fd);
        close(result_fd);
        return 1;
    }

    if (pid == 0) 
    { /*Child process*/
        close(pipe_fd[0]); 

        /*Reverse the content*/
        for (int i = 0; i < bytes_read / 2; i++) {
            char temp = buffer[i];
            buffer[i] = buffer[bytes_read - i - 1];
            buffer[bytes_read - i - 1] = temp;
        }

        /*Write reversed content to the pipe*/
        write(pipe_fd[1], buffer, bytes_read);
        close(pipe_fd[1]); 

        return 0; /*Exit child*/
    } 

    else 
    { /*Parent process*/
        close(pipe_fd[1]); 
        
        waitpid(pid, NULL, 0);

        char reversed[MAX];
        int bytes_from_pipe = read(pipe_fd[0], reversed, sizeof(reversed));
        if (bytes_from_pipe == -1) 
        {
            write(STDERR_FILENO, "Error reading from pipe\n", 24);
            close(sample_fd);
            close(result_fd);
            return 1;
        }

        /*Write reversed content to result.txt*/
        write(result_fd, reversed, bytes_from_pipe);

        close(pipe_fd[0]); 
    }

    close(sample_fd);
    close(result_fd);

    return 0;
}
