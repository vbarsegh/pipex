#include "pipex.h"

void    perror_exit(int err_num, t_pipex *pipex, char **split_cmd, char *path_plus_cmd)
{
    if (err_num == FD1)
    {
        free_split_cmd_and_path(split_cmd, path_plus_cmd);
        perror("Cannot open fd1 file descriptor");
    }
    else if (err_num == DUP2)
    {
        free_split_cmd_and_path(split_cmd, path_plus_cmd);
        perror("dup2 is failed");
    }
    else if (err_num == EXECVE_ERR)
        perror("execve failed");
    else if (err_num == SPLIT_ERR)
        perror("split failed");
    else if (err_num == ACCESS_ERR)
        perror("access failed");
    else if(err_num == FD_OUT)
    {
        free_split_cmd_and_path(split_cmd, path_plus_cmd);
        perror("fd_out is failed");
    }
    close_fds_in_parent_proces(pipex);
    exit (1);
}

void    free_split_cmd_and_path(char **split_cmd, char *path_plus_cmd)
{
    free_split_line(split_cmd);
    free(path_plus_cmd);
}

void    free_split_line(char **matrix)
{
    int i;

    i = 0;
    while(matrix[i])
    {
        free(matrix[i]);
        i++;
    }
    free(matrix);
    matrix = NULL;
}