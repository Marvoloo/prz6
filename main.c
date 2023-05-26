#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h> 

static int	ft_get_new_start(char const *s1, char const *set, int i, int j)
{
	while (s1[i] != '\0' && set[j] != '\0')
	{
		if (set[j] == s1[i])
		{
			j = -1;
			i ++;
		}
		j ++;
	}
	return (i);
}

static int	ft_get_new_end(char const *s1, char const *set, int i, int start)
{
	int	j;

	j = 0;
	while (i > start && set[j] != '\0')
	{
		if (set[j] == s1[i])
		{
			j = -1;
			i --;
		}
		j ++;
	}
	return (i);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	int		start;
	int		len;
	char	*s2;

	if (s1 == 0)
		return (0);
	len = strlen(s1) - 1;
	start = ft_get_new_start(s1, set, 0, 0);
	len = ft_get_new_end(s1, set, len, start) - start;
	if (len == 0)
		len = -1;
	s2 = (char *) malloc (sizeof(char) * (len + 2));
	if (s2 == 0)
		return (0);
	strncpy(s2, s1 + start, len + 2);
	return (s2);
}

void to_upper(char *str)
{
    char c;
    int len = strlen(str);
    write(1 ,"String in UPPERCASE: ", strlen("String in UPPERCASE: "));
    for (int i = 0; i < len; i++)
    {
        c = (char)toupper(str[i]);
        write(1, &c, 1);
    }
    write(1, "\n", 1);
}

void to_lower(char *str)
{
    char c;
    int len = strlen(str);
    write(1 ,"String in lowercase: ", strlen("String in lowercase: "));
    for (int i = 0; i < len; i++)
    {
        c = (char)tolower(str[i]);
        write(1, &c, 1);
    }
    write(1, "\n", 1);
}

void ft_atoi(char *str)
{
    int val;
    val = atoi(str);
    printf("Atoi String: %d\n", val);
}

void counter(char *str)
{
    int digits = 0;
    int alpha = 0;
    int others = 0;

    for (int i = 0; i < strlen(str); i++)
    {
        if (isdigit(str[i]) != 0)
            digits += 1;
        else if (isalpha(str[i]) != 0)
            alpha += 1;
        else 
            others += 1;
    }
    printf("Digits count: %d\n", digits);
    printf("Alphabets count: %d\n", alpha);
    printf("Others char count: %d\n", others);
}

void trim(char *str)
{
    char *tmp;
    tmp = ft_strtrim(str, "\t\n ");
    printf("Trimmed STR: %s\n", tmp);
    free(tmp);
}
void run(char *buf)
{
    if (buf[strlen(buf) - 1] == '\n')
    {
        char * tmp;
        tmp = (char *) malloc(sizeof(char) * strlen(buf) - 1);
        strncpy(tmp, buf, strlen(buf) - 1);
        tmp[strlen(buf) - 1] = 0;
        to_upper(tmp);
        to_lower(tmp);
        ft_atoi(tmp);
        counter(tmp);
        trim(tmp);
        free (tmp);
    }
    else
    {
        to_upper(buf);
        to_lower(buf);
        ft_atoi(buf);
        counter(buf);
        trim(buf);
    }
}

int main(int arhc, char **argv)
{
    char buf[2048];
    //buf = (char *)malloc(sizeof(char) * 2048);
    // while (1)
    // {
        memset(buf, 0, 2048);
        if (read(0, buf, 2048) < 0)
            return (1);
        // printf("Len: %ld\n", strlen(buf));
        if (strcmp(buf, "stop\n") == 0)
            return (0);
        run(buf);
        
    // }
    //free (buf);
}
