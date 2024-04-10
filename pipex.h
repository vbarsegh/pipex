#ifndef PIPEX_H
# define PIPEX_H

#  define MIN_ARGC 5
#  define SPLIT_ERR 4
#  define EXECVE_ERR 3
#  define JOIN_ERR 2
#  define ACCESS_ERR 6
#  define FORK_ERR 7
#  define DUP2 8
#  define FD1 9
#  define FD_OUT 10

#include <fcntl.h>
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/wait.h>

typedef struct s_pipex
{
 int (*fd)[2];
 int cmd_count;
 int current_cmd;
 int pid;
 // char **split_cmd;
 // char *path_plus_cmd;
}   t_pipex;

char *ft_strjoin(const char *s1, const char *s2);
//char	*my_strncpy(char *dest, const char *src, unsigned int n);
void ft_strncpy(char *dest, const char *src, unsigned int n);
char    *ft_strstr(char *str, char *to_find);
char **ft_split(char const *s, char c);
char *ft_strjoin(const char *s1, const char *s2);
void free_split_cmd_line(char split_cmd1, char split_cmd2);
int found_which_line_was_start_with_path(char **envp, t_pipex *pipex);
char *check_if_cmd_is_X_OK(char *envpi, char **split_cmd, t_pipex *pipex);
void fork_for_1st_cmd(char **argv, char **envp, t_pipex *pipex, int i);
void fork_for_middle_cmds(char **argv, char **envp, t_pipex *pipex, int i);
void fork_function(char **argv, char **envp, t_pipex *pipex);
void    pipe_function(char **argv, char **envp, t_pipex *pipex);
void close_fds_1st_cmd(t_pipex *pipex);
void close_fds_mid_cmds(t_pipex *pipex);
void close_fds_last_cmd(t_pipex *pipex);
void close_fds_in_parent_proces(t_pipex *pipex);
void    perror_exit(int err_num, t_pipex *pipex, char **split_cmd, char *path_plus_cmd);
void    free_split_line(char **matrix);
void    free_split_cmd_and_path(char **split_cmd, char *path_plus_cmd);






#endif