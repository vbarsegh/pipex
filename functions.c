#include "pipex.h"

void ft_strncpy(char *dest, const char *src, unsigned int n)
{
 unsigned int i;

 i = 0;
 while (i < n && src[i] != '\0')
 {
  dest[i] = src[i];
  i++;
 }
 dest[i] = '\0';
 i++;
}

char *ft_strjoin(const char *s1, const char *s2)
{
 char *str;
 size_t s1_len;
 size_t s2_len;

 s1_len = 0;
 s2_len = 0;
 while (s1[s1_len])
  s1_len++;
 while (s2[s2_len])
  s2_len++;
 str = (char *)malloc(sizeof(char) * (s1_len + s2_len + 2));//dlya /
 if (!str)
  return (NULL);
 ft_strncpy(str, s1, s1_len + 1);
    str[s1_len] = '/';
 ft_strncpy(&str[s1_len + 1], s2, s1_len + s2_len + 1);
 return (str);
}