#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX 1024

int main()
{
    char buffer[MAX];
    int bytes_read;
    int i;

    write(STDERR_FILENO, "Reverser started\n", 17);
    
    /*Read data from stdin*/
    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0)
    {
        write(STDERR_FILENO, "Reverser read from pipe\n", 24);

        /*Reverse the string in the buffer*/
        for (i = 0; i < bytes_read / 2; i++)
        {
            char temp = buffer[i];
            buffer[i] = buffer[bytes_read - i - 1];
            buffer[bytes_read - i - 1] = temp;
        }

        write(STDERR_FILENO, "Reverser writing to stdout\n", 26);

        /*Write the reversed content to stdout*/
        if (write(STDOUT_FILENO, buffer, bytes_read) == -1)
        {
            write(STDERR_FILENO, "Error writing to stdout\n", 24);
            return 1;
        }

        write(STDERR_FILENO, "Reverser finished writing to pipe\n", 34);
    }

    return 0;
}
