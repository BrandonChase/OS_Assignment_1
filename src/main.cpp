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
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

#include <vector>
#include <iostream>
#include <algorithm>

#define MAX_NUMBER_OF_FILES_DIRECTORIES 1024
#define MAX_FILENAME_LENGTH 2048
#define MAX_PATH_LENGTH 4096
#define FRAME_SCROLL_LENGTH 10 //number of files/directories to display in one print window

using namespace std;

typedef struct
{
	char name[MAX_FILENAME_LENGTH];
	size_t size;
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
	strcpy(full_file_path, "");//resets variable, somehow stayed the same since last function call despite going out of scope
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

void processFiles(vector<File>& files, char* directory_path, int& number_of_entities)
{
	DIR* current_directory = opendir(".");
	if(current_directory != NULL)
	{
		struct dirent* current_dirent;
		while((current_dirent = readdir(current_directory)) && number_of_entities <= MAX_NUMBER_OF_FILES_DIRECTORIES)
		{
			if((current_dirent->d_type) & DT_REG) //if dirent entity is a directory
			{
				File temp_file = generateFile(directory_path, current_dirent->d_name);
				files.push_back(temp_file);
				number_of_entities++;
			}
		}
		closedir(current_directory);
	}

	else
	{
		printf("***ERROR: Could not process directory!");
	}
}

void processDirectories(vector<Directory>& directories, int& number_of_entities)
{
	DIR* current_directory = opendir(".");
	if(current_directory != NULL)
	{
		struct dirent* current_dirent;
		while ((current_dirent = readdir(current_directory)) && number_of_entities <= MAX_NUMBER_OF_FILES_DIRECTORIES)
		{
			if((current_dirent->d_type) & DT_DIR) //if dirent entity is a directory
			{
				Directory temp_directory;
				strcpy(temp_directory.name, current_dirent->d_name);
				directories.push_back(temp_directory);
				number_of_entities++;
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
		printf("\t%d. %s ~ %dB ~ %s\n", i++, f.name, f.size, ctime(&f.time_last_modified));
	}

	printf("\n");
}

void printOperations()
{
	printf("Operations:\n");
	printf("\te     Edit\n");
	printf("\tr     Run\n");
	printf("\tc     Change Directory\n");
	printf("\ts     Sort Files\n");
	printf("\tp/n   Scroll Files\n");
	printf("\tP/N   Scroll Directories\n");
	printf("\tq     Quit\n");

}

bool compareBySize(const File& a, const File& b)
{
	return a.size > b.size; //we want descending size
}

void sortFilesBySize(vector<File>& files)
{
	sort(files.begin(), files.end(), compareBySize);
}

bool compareByDate(const File& a, const File& b)
{
	return a.time_last_modified > b.time_last_modified; //we want descending time
}

void sortFilesByDate(vector<File>& files)
{
	sort(files.begin(), files.end(), compareByDate);
}

int main(int argc, char *argv[])
{
	//if given a directory as a command line argument, change to it as starting directory
	if(argc == 2)
	{
		changeDirectory(argv[1]);
	}
	

	bool sortingBySize = false;
	bool sortingByDate = false;
	while (1)
	{
		system("clear"); //reset terminal display

		//----------Print System Information----------

		char directory_path[MAX_PATH_LENGTH];
		getCurrentDirectory(directory_path);
		printCurrentDirectory(directory_path);

		printDate();

		//----------Process Filesystem Data----------
		int number_of_entities = 0;
		vector<Directory> directories;
		processDirectories(directories, number_of_entities);

		vector<File> files;
		processFiles(files, directory_path, number_of_entities);

		//since building files list from scratch on every loop, need to resort new list if user wants sorted
		if(sortingBySize)
		{
			sortFilesBySize(files);
		}

		else if(sortingByDate)
		{
			sortFilesByDate(files);
		}

		printDirectories(directories);

		printFiles(files);

		printOperations();

		//----------Perform Operations----------
		printf("-------------------------------------------\n\n");

		char c;
		char command[256+MAX_FILENAME_LENGTH], input[MAX_FILENAME_LENGTH];
		pid_t child;

		scanf(" %c", &c);
		switch(c)
		{
			case 'q':
				exit(0);
			case 'e':
				printf("Edit what?: ");
				scanf(" %s", input);
				strcpy(command, "pico ");
				strcat(command, input);
				system(command);
				break;
			case 'r':
				printf("Run what?: ");
				scanf(" %s", input);
				printf("\n");
				strcpy(command, "./");
				strcat(command, input);
				system(command);
				printf("\nEnter any character to continue.\n");
				cin >> ws; //ignore trailing newline
				getchar();
				break;
			case 'c':
				printf("Change to which directory?: ");
				scanf("%s", input);
				changeDirectory(input);
				//don't want to immediately sort files when moving to another directory
				sortingBySize = false;
				sortingByDate = false;
				break;
			case 's':
				printf("Sort by size (s) or date last modified(d)?: ");
				scanf(" %c", &c);
				if(c == 's') 
				{
					sortingBySize = true;
					sortingByDate = false;
				}

				else if(c == 'd')
				{
					sortingByDate = true;
					sortingBySize = false;
				}

				else
				{
					printf("***ERROR: \'%c\' is invalid!\n");
					printf("\nEnter any character to continue.\n");
					cin >> ws; //ignore trailing newline
					getchar();
				}
				break;

		}
	}
}