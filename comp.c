#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int compFiles(int fid_one, int fid_two) {
	// This function gets two fids and checks if the two files they are connected to are the same. 
	// If so, returns 2, else returns 1. If an error has accourd, return -1.
	
	char buffer_one; // the read data from file 1
	char buffer_two; // the read data from file 2
	
	int flag = 2;
	
	int count_one; // count read data from file 1 
	int count_two; // count read data from file 2
	
	while (flag == 2) {
		// read new data
		count_one = read(fid_one, &buffer_one, 1);
		count_two = read(fid_two, &buffer_two, 1);
		
		if (count_one == -1 || count_two == -1) { // error accourd
			flag = -1;
		}
		else if (count_one == 0 && count_two == 0) { // both ended
			return 2;
		}
		else if ((count_one * count_two) == 0) { // only one of them ended
			flag = 1;
		}
		else if (buffer_one != buffer_two) { // both read 1 char, but its different
			flag = 1;
		}
	}
	return flag; // whether they are equal or not
}

int main(int argc, char* argv[]) {
	// check if gotten only two args
	if (argc != 3) { return -1; }
	
	// open the files
	int fid1 = open(argv[1], O_RDONLY);
	int fid2 = open(argv[2], O_RDONLY);
	
	// check if they are the same
	int exit_number = compFiles(fid1, fid2);
	exit(exit_number);
}

