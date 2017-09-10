#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

int main(void)
{
	pid_t child;
	DIR* d;
	struct dirent* de;
	int i, c, k;
	char s[256], cmd[256];
	time_t t;

	while (1)
	{
		t = time(NULL);
		printf("Time: %s\n", ctime(&t));

		getcwd(s, 200);
		printf("\nCurrent Directory: %s \n", s);

		d = opendir(".");
		c = 0;
		while ((de = readdir(d)))
		{
			if((de->d_type) & DT_DIR)
			{
				printf("( %d Directory: %s ) \n", c++, de->d_name);
			}
		}
		closedir(d);
		d=opendir(".");
		c=0;
		while((de = readdir(d)))
		{
			if((de->d_type) & DT_REG)
			{
				printf("( %d File: %s ) \n", c++, de->d_name);
			}

			if((c % 8) == 0)
			{
				printf("HIT N for Next\n");
				k = getchar();
			}
		}

		closedir(d);
		printf("-------------------------------------------\n");
		c = getchar(); getchar();
		switch(c)
		{
			case 'q':
				exit(0);
			case 'e':
				printf("Edit what?: ");
				scanf("%s", s);
				strcpy(cmd, "pico ");
				strcat(cmd, s);
				system(cmd);
				break;
			case 'r':
				printf("Run what?: ");
				scanf("%s", cmd);
				system(cmd);
				break;
			case 'c':
				printf("Change to?: ");
				scanf("%s", cmd);
				chdir(cmd);
				break;
		}
	}
}