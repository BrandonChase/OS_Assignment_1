/*
Brandon Chase ~ 1001132518

CSE 3320-001 Assignment 1

Sources:
	Code provided by David Levine in assignment handout
	https://stackoverflow.com/questions/4892680/sorting-a-vector-of-structs Code for sorting vector of structs based on struct property

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

//Sets given string parameter to the currernt working directory and alerts if error occurs
void getCurrentDirectory(char* directory_path)
{
	getcwd(directory_path, MAX_PATH_LENGTH);
	if(directory_path == NULL)
	{
		printf("***ERROR: Could not get current directory!\n");
	}
}

//Prints current working directory given as string
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

//Prints the formatted current date and time
void printDate()
{
	time_t current_time;
	current_time = time(NULL);
	printf("Current Time: %s\n", ctime(&current_time));
}

//Takes the name of a directory and switches to it
void changeDirectory(char* directory)
{
	//if error occurs user stays in same directory and is alerted
	if(chdir(directory) != 0)
	{
		printf("There was an error changing to %s! You are still in the same directory as before. Push any key to continue.\n", directory);
		getchar();
	}
}

//Takes in file name and path and returns File struct that has file name, size, and date last modified
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
	if(stat(full_file_path, &file_stat) >= 0) //if no errors
	{
		result_file.size = file_stat.st_size;
		result_file.time_last_modified = file_stat.st_ctime;
	} 

	return result_file;
}

//Goes through current directory and populates given files and directories vectors
void processCurrentDirectory(vector<File>& files, vector<Directory>& directories, char* directory_path)
{
	int number_of_entities = 0; //counts total number of files and directories
	DIR* current_directory = opendir(".");
	if(current_directory != NULL) //don't do anything if error opening directory
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

			else if((current_dirent->d_type) & DT_DIR) //if dirent entity is a directory
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

//Takes in vector of directories and the starting point in the list, and prints out a specified number of formatted directories
//from that point
void printDirectories(const vector<Directory>& directories, int directory_index)
{
	printf("Directories:\n");
	for(int offset = 0; offset < FRAME_SCROLL_LENGTH; offset++)
	{
		//stop if we've reached the end but not printed out ten entries
		if(directory_index + offset >= directories.size())
		{
			break;
		}

		Directory d = directories[directory_index + offset];
		printf("\t%d. %s\n", directory_index + offset, d.name);
	}

	printf("\n");
}

//Takes in vector of files and the starting point in the list, and prints out a specified number of formatted files
//from that point
void printFiles(const vector<File>& files, int file_index)
{
	printf("Files:\n");
	for(int offset = 0; offset < FRAME_SCROLL_LENGTH; offset++)
	{
		//stop if we've reached the end but not printed out ten entries
		if(file_index + offset >= files.size())
		{
			break;
		}

		File f = files[file_index + offset];
		printf("\t%d. %s ~ %dB ~ %s", file_index + offset, f.name, f.size, ctime(&f.time_last_modified)); //exluded \n b/c ctime includes it at end
	}

	printf("\n");
}

//Display available operations that the user can peform
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

//Compare function used by sortFilesBySize. Tells sort function to sort files by descending size
bool compareBySize(const File& a, const File& b)
{
	return a.size > b.size; //we want descending size
}

//Sorts given file list in descending file size
void sortFilesBySize(vector<File>& files)
{
	sort(files.begin(), files.end(), compareBySize);
}

//Compare function used by sortFilesByDate. Tells sort function to sort files by most recently modified first
bool compareByDate(const File& a, const File& b)
{
	return a.time_last_modified > b.time_last_modified; //we want descending time
}

//Sorts given file list by most recently modified first
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
	int file_index = 0;
	int directory_index = 0;

	while (1) //loop indefinitely until user enters quit command
	{
		system("clear"); //reset terminal display, looks better and easier to follow

		//----------Print System Information----------

		char directory_path[MAX_PATH_LENGTH];
		getCurrentDirectory(directory_path);
		printCurrentDirectory(directory_path);

		printDate();

		//----------Process Filesystem Data----------
		vector<Directory> directories;
		vector<File> files;
		processCurrentDirectory(files, directories, directory_path);

		//since building files list from scratch on every loop, need to re-sort new list if user has commanded it be sorted
		//thus, if user creates new file or modifies existing one or any other changes occur in directory, they will be updated in the 
		//reset sorting rules when user changes to different directory
		if(sortingBySize)
		{
			sortFilesBySize(files);
		}

		else if(sortingByDate)
		{
			sortFilesByDate(files);
		}

		//----------Print Output----------
		printDirectories(directories, directory_index);

		printFiles(files, file_index);

		printOperations();

		//----------Perform Operations----------
		printf("-------------------------------------------\n\n");
		printf("Enter Command: ");
		char c;
		scanf(" %c", &c);
		char command[256+MAX_FILENAME_LENGTH];
		int choice_index;

		switch(c)
		{
			//quit
			case 'q': 
				exit(0);
			
			//edit
			case 'e':
				printf("Enter index of file to edit: ");
				scanf(" %d", &choice_index);
				if(0 <= choice_index && choice_index < files.size())
				{
					strcpy(command, "pico ");
					strcat(command, files[choice_index].name);
					system(command);
				}
				break;
			
			//run program
			case 'r':
				printf("Enter index of file to run: ");
				scanf(" %d", &choice_index);
				if(0 <= choice_index && choice_index < files.size())
				{
					printf("\n");
					strcpy(command, "./");
					if(access(files[choice_index].name, F_OK|X_OK) == 0) // run only if file is executable
					{
						strcat(command, files[choice_index].name);
						system(command);
					}

					else
					{
						printf("***ERROR: Not executable!\n");
					}

					printf("\nEnter any character to continue: ");
					cin >> ws; //ignore trailing newline
					getchar();
					
				}
				break;
			
			//change directory
			case 'c':
				printf("Enter index of directory to change to: ");
				scanf(" %d", &choice_index);
				if(0 <= choice_index && choice_index < directories.size())
				{
					changeDirectory(directories[choice_index].name);
					//don't want to immediately sort files when moving to another directory
					sortingBySize = false;
					sortingByDate = false;
				}
				break;
			
			//sort files
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
					printf("\nEnter any character to continue: ");
					cin >> ws; //ignore trailing newline
					getchar();
				}
				break;

			//scroll back on files
			case 'p':
				//if would scroll back beyond beginning of list, reset to beginning
				if(file_index - FRAME_SCROLL_LENGTH < 0)
				{
					file_index = 0;
				}

				else
				{
					file_index -= FRAME_SCROLL_LENGTH;
				}
				break;
			
			//scroll forward on files
			case 'n':
				//if fewer files than one page, don't do anything
				if(files.size() < FRAME_SCROLL_LENGTH)
				{
					
				}

				//if would scroll beyond list, don't do anything 
				else if(file_index + FRAME_SCROLL_LENGTH > files.size() - 1)
				{
					
				}

				else
				{
					file_index += FRAME_SCROLL_LENGTH;
				}
				break;

			//scroll backwards on directories
			case 'P':
				//if would scroll back beyond beginning of list, reset to beginning
				if(directory_index - FRAME_SCROLL_LENGTH < 0)
				{
					directory_index = 0;
				}

				else
				{
					directory_index -= FRAME_SCROLL_LENGTH;
				}
				break;
			
			//scroll forwards on directories
			case 'N':
				//if fewer directories than one page, don't do anything
				if(directories.size() < FRAME_SCROLL_LENGTH)
				{
					
				}
				//if would scroll beyond list, don't do anything 
				else if(directory_index + FRAME_SCROLL_LENGTH > directories.size() - 1)
				{
					
				}
				else
				{
					directory_index += FRAME_SCROLL_LENGTH;
				}
				break;
		}
	}
}