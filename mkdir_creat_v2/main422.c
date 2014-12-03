// Casey O'Leary
// CS 360 Main for ext 2 file system
#include "type.h"
#include "lab8422.h"


// Takes an input string, which it destroys
// splits on a whitespace to two already allocated pointers
void split(char *input, char *head, char *tail) 
{
		char *token = strtok(input, " ");
		strcpy(head, token);
		//printf("command=%s\n", head);
		token = strtok(NULL, " ");
		if (token != NULL)
		{
			strcpy(tail, token);
			//printf("path=%s\n", tail);
		}
		return;
}

int run_command(char *cmd, char *path)
{
	int j;
	for (j = 0; j < 11; j++) 
	{
		//printf("Comparing: %s to %s\n");
		if (strcmp(cmnds[j], cmd) == 0)
		{
			printf("Executing: %s %s\n", cmd, path);
			return fptr[j](path);
		}
	}
}

//int expected(char *cmd, char *path)

// Function that frees up any allocated memory during testing period for the two arrays of pointers
void clean_memory(char **commands, char **pathnames)
{
	int i = 0;
	while (commands[i] != NULL)
	{
		free(commands[i++]);
		//memset(commands[i++], 0, sizeof(commands[i++]));
	}
	i = 0;
	while (pathnames[i] != NULL)
	{
		free(pathnames[i++]);
		//memset(pathnames[i++], 0, sizeof(pathnames[i++]));
	}
	return;
}

// More extensive and simpler checks can be implemented so code is more clear
/*void check_command(char *command, char *path, int *return_value, int *flag_ptr)
{
	
}*/ 
	

int main(int argc, char *argv[])
{
	int i, j, k, cmd, error_count = 0, execution_value, fail_flag = 0, pid = 0, status = 0;
	unsigned int ino_num;
	
	//input files
	char *files[4] = { "input0", "input1", "input2", NULL };
	
	ssize_t read, len = 512;
	FILE *input = NULL;
	
	/*// arrays for the commands, assuming there are less than 25 commands/pathnames and they will not be longer
	// than 32 characters
	char *commands[32];
	char *pathnames[32];*/
	
	char *line = NULL;
	
	// Initialization of ext2 file system
	init();
	mount_root(argv[1]);
	printf("Dev=%d, inodeBegin=%d, bmap=%d, imap=%d, ninodes=%d\n", dev, inodeBegin, bmap, imap, ninodes);
	printf("ext_2 file system initialized....\n\n");
	
	k = 0;
	// Loop through disks and input files
	while (files[k] != NULL)
	{
		// Start 422 mkdir_creat testing
		if (!(input = fopen(files[k], "r"))) // check for error opening file from command args, this is the input file
		{
			//printf("Error opening file %s\n",files[k]);
			break;
		}
		printf("\n*********************** Starting mkdir & creat testing ***********************\n");
		// Get input from file
		i = 0;
		line = (char *) malloc(len +1);
		while ((read = getline(&line, &len, input)) != -1) {
			char *token, *command, *path;
			line[strlen(line)-1] = 0; // kill \r at end
		
			// Check for the fail flag, meaning the command is supposed to fail based on previous inputs
			if (line[0] != '#')
			{
				//allocate space for each new command and path
				command = (char *) malloc(128+1);
				path = (char *) malloc(512+1);
				
				// split up the input
				split(line, command, path);
		
				// put into arrays and store for later in case we want to do something else with them, i dont know what yet
				// had ideas initially, but then changed approach, still could do something with this later
				/*commands[i] = command; 
				pathnames[i] = path;*/
		
				
				// Run the Command and set the error checking value, which is already in the function so no need to duplicate
				execution_value = run_command(command, path);
		
				// Search for inode num with that pathname, to see if it was actually created on the disk
				// but only if a valid command & pathname combo
				// We are going to ignore change of directory commands (which needs to be changed/edited
				// Also a check to determine whether the flag should fail or not, which is in input file
				if (strcmp(command, "cd") != 0){ 
					if (( (ino_num = getino(dev, path)) > 0) && (execution_value > 0))
					{
						printf("Success, valid command. Inode created. ino_num: %d\n", ino_num);
						// Lets print out the contents of directory to give more proof that this happened
						//run_command("ls", NULL);
					}
					else if (fail_flag == 1)
					{
						printf("Command failed as expected.\n");
						fail_flag = 0;
					}
					else
					{
						printf("Failure, invalid command. No new Inode created.\n");
						error_count++;
					}
				}
				else
				{
					if (flag_fail == 1)
						printf("Command failed as expected.\n");
				}
				
		
		
		
				//free(command);
				//free(path);
				i++;
			}
			
			printf("\n");
		}
		// Set end of arrays
		//commands[i] = NULL;
		//pathnames[i] = NULL;
		//clean_memory(commands, pathnames);
		//free(line);
		fclose(input);
		k++;
		
		// Fork to a different process to clean the disk
		if ( (pid = fork()) < 0)
		{
			printf("Error forking.\n");
			exit(0);
		}
		if (pid)
		{
			// Do nothing until child is done
			//waitpid(pid, &status, 0);
		}
		else
		{
			char *program = "clean.sh";
			close(1); //close stdout so we dont see all the junk
			execl(program, program, argv[0], NULL);
			exit(0);
		}
			
	}
	
	printf("\n------------------------------ End Error Results ----------------------------- \n");
	if (!error_count)
	{
		printf("\n SUCCESS! All commands executed correctly and ones that expected to fail did!\n");
	}
	else
	{
		printf(" Almost! There were %d errors, not all commands executed correctly. \n", error_count);
	}
	return;
}
