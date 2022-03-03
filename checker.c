#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
// sizes
#define SIZE_NAMES_PATH 7
#define SIZE_INPUT_PATH 22
#define SIZE_EXPECTED_OUT_PATH 25
#define SIZE_FILE_STD_PATH 50
#define SIZE_GRADE 56

int main(int argc, char* argv[]) {
	if (argc != 2) {
		write(2, "Too many or too less arguments (should be 1)\n", 45);  // wrtie to perror
		exit(-1);
	}
    // fid for each file - process
    int fid_input;
    int fid_expected_out;
    int fid_curr_out;
    int fid_results;

    int fid_config = open(argv[1], O_RDONLY);  // try to open config.txt file
    if (fid_config == -1) { write(2, "Cannot open config file\n", 25);  exit(-1); }

    // strings for files' paths
    char path_std_names[SIZE_NAMES_PATH + 1];  // path for dir with students dirs
    char path_input[SIZE_INPUT_PATH + 1];  // path for input file
    char path_expected_out[SIZE_EXPECTED_OUT_PATH + 1];  // path for expected output file
    char temp;  // read \n from config file

    // read data - paths
    // names dir
    int read_names_path = read(fid_config, &path_std_names, SIZE_NAMES_PATH);
    if (read_names_path == -1) { 
		write(2, "Cannot read name path\n", 22); 
		// close all files that are open 
		close(fid_config);
		exit(-1); 
	}
    path_std_names[SIZE_NAMES_PATH] =  '\0';
    read(fid_config, &temp, 1);  // enter
    // input file
    int read_input_path = read(fid_config, &path_input, SIZE_INPUT_PATH);
    if (read_input_path == -1) { 
		write(2, "Cannot read input path\n", 23); 
		close(fid_config); 
		exit(-1); 
	}
    path_input[SIZE_INPUT_PATH] =  '\0';
    read(fid_config, &temp, 1);  // enter
    // expected output file
    int read_expected_out_path = read(fid_config, &path_expected_out, SIZE_EXPECTED_OUT_PATH);
    if (read_expected_out_path == -1) { 
		write(2, "Cannot read expected out path\n", 30);   
		// close all files that are open 
		close(fid_config);
		exit(-1); 
	}
    path_expected_out[SIZE_EXPECTED_OUT_PATH] =  '\0';


    // open names dir
    DIR* names_dir = opendir(path_std_names);  // pointer to students' names dir
    if (names_dir == NULL) { 
		write(2, "Cannot open names dir\n", 22);   
		// close all files that are open 
		close(fid_config);  
		exit(-1); 
	}
    
    // start check students
    struct dirent* curr_std = readdir(names_dir);  // pointer to current student dir
    int status;
    char path_file_std[SIZE_FILE_STD_PATH];  // path to student program

    while (curr_std != NULL) {
        // open input, output files
        if(strcmp(curr_std->d_name, ".") != 0 && strcmp(curr_std->d_name, "..") != 0) { 
        	fid_input = open(path_input, O_RDONLY);
        	if (fid_input == -1) { 
				write(2, "Cannot open input\n", 18);
				// close all files that are open 
				close(fid_config);  
				closedir(names_dir);
				exit(-1); 
			}
        	fid_curr_out = open("curr_out.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
        	if (fid_expected_out == -1) { 
				write(2, "Cannot open output\n", 19);
				// close all files that are open 
				close(fid_config);  
				closedir(names_dir); 
				close(fid_input); 
				exit(-1); 
			}
        	
        	// creates path to execvp 
        	strcpy(path_file_std, "./names");
        	strcat(path_file_std, "/");
        	strcat(path_file_std, curr_std->d_name);
        	strcat(path_file_std, "/main.exe");
        	
        	// create childs to run the students' programs
        	int fid_std = fork();  // fid for each student program, child
	
        	if (fid_std == 0) {  // child
		    	dup2(fid_input, 0);  // assing the file for input to be the input for child
		    	dup2(fid_curr_out, 1);  // assing the file for output to be the output for child
		    	execl(path_file_std, path_file_std, NULL);  // execute student program
        	}
	
        	else {  // father
        		waitpid(fid_std, &status, 0);
        		close(fid_input);
        		close(fid_curr_out);
        		
        		// check if the student succeeded
        		fid_results = open("results.csv", O_RDWR | O_CREAT | O_APPEND, 0666);
        		if (fid_results == -1) { 
					write(2, "Cannot open results\n", 20);
					// close all files that are open 
					close(fid_config);  
					closedir(names_dir); 
					exit(-1); 
				}
		
        		fid_std = fork();  // fid for each student program, child
		
        		if (fid_std == 0) {  // child
		    		dup2(fid_results, 1);  // assing the file for results to be the output for child
		    		char str_grade[SIZE_GRADE];
		    		strcpy(str_grade, curr_std->d_name);
		    		strcat(str_grade, ", ");
		    		write(1, str_grade, strlen(str_grade));

					int fid_comp = fork();
					if (fid_comp == 0) {  // child of create the comp.out file
						execl("/bin/gcc", "/bin/gcc", "-o", "comp.out", "comp.c", NULL);
					}
					else  // father
		    			execl("./comp.out", "./comp.out", "curr_out.txt", path_expected_out, NULL);  // check students output
        		}
        		
        		else {  // father
        			waitpid(fid_std, &status, 0);
					int exit_status = WEXITSTATUS(status);
					if (exit_status == 1) {  // diff
						write(fid_results, "0\n", 2);
					}
					else if (exit_status == 2) {  // same 
						write(fid_results, "100\n", 4);
					}
					else {  // error
						write(2, "An error accord in comp.out\n", 28);
						// close all files that are open 
						close(fid_config);  
						closedir(names_dir); 
						close(fid_results);
						execl("/bin/rm", "/bin/rm", "results.csv", NULL);  // remove the results csv because failed
						exit(-1);
					}
        		}
        	}
        }
        		
        curr_std = readdir(names_dir);  // read next dir
        close(fid_results);  // close because we are opening in the loop
    }
    
    closedir(names_dir);
    close(fid_results);
    close(fid_config);
    execl("/bin/rm", "/bin/rm", "curr_out.txt", NULL);  // remove the current output temp file
    exit(0);
}
