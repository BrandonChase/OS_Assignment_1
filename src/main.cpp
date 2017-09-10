/*
Brandon Chase ~ 1001132518

Assignment 1

Sources:
	Code provided by David Levine in assignment handout
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> //getcwd()
#include <dirent.h>
#include <string.h>
#include <time.h>

#include <vector>

#define MAX_FILENAME_LENGTH 2048
#define MAX_PATH_LENGTH 4096

using namespace std;

typedef struct
{
	char name[MAX_FILENAME_LENGTH];
	int size;
	time_t time_last_modified;
} File;

typedef struct 
{
	char name[MAX_FILENAME_LENGTH];
} Directory;

void getCurrentDirectory(char* directory_path)
{
	getcwd(directory_path, MAX_PATH_LENGTH);
	if(directory_path == NULL)
	{
		printf("***ERROR: Could not get current directory!\n");
	}
}

void printCurrentDirectory(const char* directory_path)
{
	if(directory_path != NULL)
	{
		printf("Current Directory: %s\n", directory_path);
	}
	else
	{
		printf("Current Directory: ***ERROR***\n");
	}
}

void printDate()
{
	time_t current_time;
	current_time = time(NULL);
	printf("Current Time: %s\n", ctime(&current_time));
}

void changeDirectory(char* directory)
{
	if(chdir(directory) != 0)
	{
		printf("There was an error changing to %s! You are still in the same directory as before. Push any key to continue.\n", directory);
		getchar();
	}
}

File generateFile(char* directory_path, char* file_name)
{
	File result_file;
	strcpy(result_file.name, file_name);

	//construct absolute path name for desired file
	char full_file_path[MAX_PATH_LENGTH+MAX_FILENAME_LENGTH];
	strcat(full_file_path, directory_path);
	strcat(full_file_path, "/");
	strcat(full_file_path, file_name);

	struct stat file_stat;
	if(stat(full_file_path, &file_stat) >= 0) //no errors
	{
		result_file.size = file_stat.st_size;
		result_file.time_last_modified = file_stat.st_ctime;
	} 

	return result_file;
}

void processFiles(vector<File>& files, char* directory_path)
{
	DIR* current_directory = opendir(".");
	if(current_directory != NULL)
	{
		struct dirent* current_dirent;
		while((current_dirent = readdir(current_directory)))
		{
			if((current_dirent->d_type) & DT_REG) //if dirent entity is a directory
		{
				File temp_file = generateFile(directory_path, current_dirent->d_name);
				files.push_back(temp_file);
			}
		}
		closedir(current_directory);
	}

	else
	{
		printf("***ERROR: Could not process directory!");
	}
}

void processDirectories(vector<Directory>& directories)
{
	DIR* current_directory = opendir(".");
	if(current_directory != NULL)
	{
		struct dirent* current_dirent;
		while ((current_dirent = readdir(current_directory)))
		{
			if((current_dirent->d_type) & DT_DIR) //if dirent entity is a directory
			{
				Directory temp_directory;
				strcpy(temp_directory.name, current_dirent->d_name);
				directories.push_back(temp_directory);
			}
		}

		closedir(current_directory);
	}

	else
	{
		printf("***ERROR: Could not process directory!");
	}
}

void printDirectories(const vector<Directory>& directories)
{
	printf("Directories:\n");
	int i = 0;
	for(Directory d : directories)
	{
		printf("\t%d. %s\n", i++, d.name);
	}

	printf("\n");
}

void printFiles(const vector<File>& files)
{
	printf("Files:\n");
	int i = 0;
	for(File f : files)
	{
		printf("\t%d. %s\n", i++, f.name);
	}

	printf("\n");
}

void printOperations()
{
	printf("Operations:\n");
	printf("\te   Edit\n");
	printf("\tr   Run\n");
	printf("\tc   Change DIrectory\n");
	printf("\ts   Sort Files\n");
	printf("\tq   Quit\n");

}

int main(int argc, char *argv[])
{
	//if given a directory as a command line argument, change to it as starting directory
	if(argc == 2)
	{
		changeDirectory(argv[1]);
	}
	
	while (1)
	{
		system("clear"); //reset terminal display

		//----------Print System Information----------

		char directory_path[MAX_PATH_LENGTH];
		getCurrentDirectory(directory_path);
		printCurrentDirectory(directory_path);

		printDate();

		//----------Process FIlesystem Data----------

		vector<Directory> directories;
		processDirectories(directories);

		vector<File> files;
		processFiles(files, directory_path);

		printDirectories(directories);

		printFiles(files);

		//----------Perform Operations----------
		printf("-------------------------------------------\n\n");

		char c;
		char command[256+MAX_FILENAME_LENGTH], input[MAX_FILENAME_LENGTH];

		scanf(" %c", &c);
		switch(c)
		{
			case 'q':
				exit(0);
			case 'e':
				printf("Edit what?: ");
				scanf(" %s", input);
				strcpy(command, "pico ");
				strcat(command, s);
				system(command);
				break;
			case 'r':
				printf("Run what?: ");
				scanf(" %s", command);
				system(command);
				break;*/
			case 'c':
				printf("Change to which directory?: ");
				scanf("%s", input);
				changeDirectory(input);
				break;
		}
	}
}