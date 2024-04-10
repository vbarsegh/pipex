#include "pipex.h"


char    *ft_strstr(char *str, char *to_find)
{
    int        i;
    int        j;

    i = 0;
    if (*to_find == '\0')
        return (str);
    while (str[i] != '\0')
    {
        j = 0;
        while (to_find[j] != '\0' && str[i + j] == to_find[j])
        {
            if (to_find[j + 1] == 0)
                return (&str[i]);
            j++;
        }
        i++;
    }
    return (0);
}


int found_which_line_was_start_with_path(char **envp, t_pipex *pipex)
{
    int i;

    i = 0;
    while (envp[i])
    {
        if (ft_strstr(envp[i], "PATH="))
            break;
        i++;
    }
    if (!envp[i])
    {
        close_fds_in_parent_proces(pipex);
        exit(write(2, "Cannot find PATH= in envp", 25));
    }
    return (i);
}

char *check_if_cmd_is_X_OK(char *envpi, char **split_cmd, t_pipex *pipex)
{
 char *cmd;
 char **matrix;
 int  i;
 char *arr;

 arr = split_cmd[0];
 matrix = ft_split(envpi + 5, ':');
 if(!matrix)
{
	free_split_line(split_cmd);
	close_fds_in_parent_proces(pipex);
	free(pipex->fd);
	exit(1);
}
 cmd = arr;
 if (access(cmd, X_OK) != -1)
  return(cmd);
 i = 0;
 while(matrix[i])
 {
  cmd = ft_strjoin(matrix[i++], arr);
  if (!cmd)
  {
	free_split_line(split_cmd);
	free_split_line(matrix);
	close_fds_in_parent_proces(pipex);
	free(pipex->fd);
	exit(write(2, "JOIN was failed", 15));
   	//perror_exit(JOIN_ERR, pipex);
  }
  if (access(cmd,X_OK) != -1)
  	break;
  free(cmd);
}
  //perror_exit(ACCESS_ERR, pipex);
 if (!matrix[i])
 {
	free_split_line(split_cmd);
	free_split_line(matrix);
	close_fds_in_parent_proces(pipex);
	free(pipex->fd);
	exit(write(2, "Command not found\n", 18));
 }
 return (cmd);
}

void fork_for_1st_cmd(char **argv, char **envp, t_pipex *pipex, int i)
{
    int fd1;
    char **split_cmd;
    char *path_plus_cmd;

	path_plus_cmd = NULL;
    split_cmd = ft_split(argv [pipex->current_cmd + 2], 32);
    if (!split_cmd)
        perror_exit(SPLIT_ERR, pipex,split_cmd, path_plus_cmd);
    path_plus_cmd = check_if_cmd_is_X_OK(envp[i],split_cmd, pipex);
    close_fds_1st_cmd(pipex);
    fd1 = open(argv[1],O_RDONLY);
    if(fd1 < 0)
        perror_exit(FD1, pipex, split_cmd, path_plus_cmd);
    if (dup2(fd1,STDIN_FILENO) == -1)
        perror_exit(DUP2, pipex, split_cmd, path_plus_cmd);
    if(dup2(pipex->fd[pipex->current_cmd][1],STDOUT_FILENO) == -1)
        perror_exit(DUP2, pipex, split_cmd, path_plus_cmd);
    close(fd1);
    close(pipex->fd[pipex->current_cmd][1]);
    if (execve(path_plus_cmd,split_cmd,envp) == -1)
        perror_exit(EXECVE_ERR, pipex,split_cmd, path_plus_cmd);
}

void fork_for_middle_cmds(char **argv, char **envp, t_pipex *pipex, int i)
{
 char **split_cmd;
 char *path_plus_cmd;

	path_plus_cmd = NULL;
    split_cmd = ft_split(argv [pipex->current_cmd + 2], 32);
    if (!split_cmd)
        perror_exit(SPLIT_ERR, pipex,split_cmd, path_plus_cmd);
    path_plus_cmd = check_if_cmd_is_X_OK(envp[i],split_cmd, pipex);
 	close_fds_mid_cmds(pipex);
 	if (dup2(pipex->fd[pipex->current_cmd - 1][0],STDIN_FILENO) == -1)
    	perror_exit(DUP2, pipex, split_cmd, path_plus_cmd);
 	if (dup2(pipex->fd[pipex->current_cmd][1], STDOUT_FILENO) == -1)
    	perror_exit(DUP2, pipex, split_cmd, path_plus_cmd);
 	close(pipex->fd[pipex->current_cmd - 1][0]);
 	close(pipex->fd[pipex->current_cmd][1]);
 	if (execve(path_plus_cmd,split_cmd,envp) == -1)
    	perror_exit(EXECVE_ERR, pipex, split_cmd, path_plus_cmd);
}

void fork_for_last_cmd(char **argv, char **envp, t_pipex *pipex, int i)
{
    int fd_out;
    char **split_cmd;
    char *path_plus_cmd;

    path_plus_cmd = NULL;
    split_cmd = ft_split(argv [pipex->current_cmd + 2], 32);
    if (!split_cmd)
        perror_exit(SPLIT_ERR, pipex,split_cmd, path_plus_cmd);
    path_plus_cmd = check_if_cmd_is_X_OK(envp[i],split_cmd, pipex);
    close_fds_last_cmd(pipex);
    if (dup2(pipex->fd[pipex->current_cmd - 1][0], STDIN_FILENO) == -1)
        perror_exit(DUP2, pipex, split_cmd, path_plus_cmd);
    fd_out = open(argv[pipex->cmd_count + 2], O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fd_out < 0)
        perror_exit(FD_OUT, pipex, split_cmd, path_plus_cmd); // Если открытие не удалось, выводим ошибку и завершаем программу
    if (dup2(fd_out, STDOUT_FILENO) == -1)
        perror_exit(DUP2, pipex, split_cmd, path_plus_cmd);
    close(pipex->fd[pipex->current_cmd - 1][0]);
    close(fd_out);
    if (execve(path_plus_cmd, split_cmd, envp) == -1)
        perror_exit(EXECVE_ERR, pipex, split_cmd, path_plus_cmd); // Если execve не удался, выводим ошибку и завершаем программу
}

void fork_function(char **argv, char **envp, t_pipex *pipex)
{
    int i;

    i = found_which_line_was_start_with_path(envp, pipex);
    while (pipex->current_cmd < pipex->cmd_count)
    {
        pipex->pid = fork();
        if (pipex->pid == -1)
        {
            close_fds_in_parent_proces(pipex);
            exit(write(2, "fork was failed", 15));
        }
        if (pipex->pid == 0)
        {
            if (pipex->current_cmd == 0)
                fork_for_1st_cmd(argv, envp, pipex, i);
            else if (pipex->current_cmd == pipex->cmd_count - 1)
                fork_for_last_cmd(argv, envp, pipex, i);
            else
                fork_for_middle_cmds(argv, envp, pipex, i);
        }
        pipex->current_cmd++;
    }

    close_fds_in_parent_proces(pipex);

    while (pipex->cmd_count > 0)
    {
        if (wait(NULL) == -1)
            exit(EXIT_SUCCESS);
        pipex->cmd_count--;
    }
}

void pipe_function(char **argv, char **envp, t_pipex *pipex)
{
    pipex->fd = malloc(sizeof(int[2]) * (pipex->cmd_count - 1));
    if (!pipex->fd)
        exit(1);
    while (pipex->current_cmd < pipex->cmd_count - 1)
    {
        if (pipe(pipex->fd[pipex->current_cmd]) == -1)
        {
            free(pipex->fd);
            exit(1);
        }
        pipex->current_cmd++;
    }
    pipex->current_cmd = 0;
    fork_function(argv, envp, pipex);
}

int main(int argc, char **argv, char **envp)
{
    t_pipex pipex;

    pipex.cmd_count = argc - 3;
    pipex.current_cmd = 0;
    if (argc < MIN_ARGC)
        exit(write(1, "Invalid count of argc", 20));
    if (access(argv[1], F_OK) == -1)
    {
        write(1, "No such file or directory\n", 26);
    }
    pipe_function(argv, envp, &pipex);
    system("leaks pipex");
}

void close_fds_1st_cmd(t_pipex *pipex)
{
    int var;

    var = 0;
    while (var < pipex->cmd_count - 1)
    {
        if (var == pipex->current_cmd)
            close(pipex->fd[var][0]);
        else
        {
            close(pipex->fd[var][0]);
            close(pipex->fd[var][1]);
        }
        var++;
    }
}

void close_fds_mid_cmds(t_pipex *pipex)
{
    int var;

    var = 0;
    while (var < pipex->cmd_count - 1)
    {
        if (var == pipex->current_cmd)
            close(pipex->fd[var][0]);
        else if (var == pipex->current_cmd - 1)
            close(pipex->fd[var][1]);
        else
        {
            close(pipex->fd[var][0]);
            close(pipex->fd[var][1]);
        }
        var++;
    }
}

void close_fds_last_cmd(t_pipex *pipex)
{
    int var;

    var = 0;
    while (var < pipex->cmd_count - 1)
    {
        if (var == pipex->current_cmd - 1)
            close(pipex->fd[var][1]);
        else
        {
            close(pipex->fd[var][0]);
            close(pipex->fd[var][1]);
        }
        var++;
    }
}

void close_fds_in_parent_proces(t_pipex *pipex)
{
    pipex->current_cmd = 0;
    while (pipex->current_cmd < pipex->cmd_count)
    {
        close(pipex->fd[pipex->current_cmd][0]);
        close(pipex->fd[pipex->current_cmd][1]);
        pipex->current_cmd++;
    }
    free(pipex->fd);
}