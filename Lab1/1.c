// COEN 146L : Lab 1 - template to use for file transfer (steps 1, 2, and 3)
// You need to adapt this template according to the requirements of each of the steps

#include <stdio.h>   // fprintf(), fread(), fwrite(), fopen(), fclose()
#include <stdlib.h>  // malloc(), free()
#include <pthread.h> // pthread_create()
#include <unistd.h>  // read(), write()
#include <fcntl.h>   // open(), close()
#include <errno.h>   // errno
#include <time.h>    // clock()
#include <sys/mman.h> // mmap()

#define BUF_SIZE 2048 //buffer size

// copies a files from src_filename to dst_filename using functions fopen(), fread(), fwrite(), fclose()
int func_copy(char* src_filename, char* dst_filename) {

	FILE *src;
  FILE *dst;
  void* buf;

	src = fopen(src_filename, "r");	// opens a file for reading
	if (src == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
		exit(0);
	}
	dst = fopen(dst_filename, "w");	// opens a file for writing; erases old file/creates a new file
	if (dst == NULL) { // fopen() error checking
		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
		exit(0);
	}

	buf = malloc((size_t)BUF_SIZE);  // allocate a buffer to store read data


	// reads content of file is loop iterations until end of file
	
	
	// writes bytes_read to dst_filename 
	
	// closes src file pointer
	// closes dst file pointer
	// frees memory used for buf
    
    //cout << "Hi 0: " << endl;
		size_t n;
		while ((n = fread(buf, 1, BUF_SIZE, src)) > 0) {
				size_t m = fwrite(buf, 1, n, dst);
				if (m != n) { /* handle write error */ }
		}

    fclose(src);
    fclose(dst);
    free(buf);


	return 0;
}

// copies a files form src_filename to dst_filename using syscalls open(), read(), write(), close()
// int syscall_copy(char* src_filename, char* dst_filename)  {
//     FILE *src_fd;
//     FILE *dst_fd;
//     void* buf;
	
// 	src_fd = open(src_filename, O_RDONLY);	// opens a file for reading
// 	if (src_fd < 0) { // open() error checking
// 		fprintf(stderr, "unable to open %s for reading: %i\n", src_filename, errno);
// 		close(src_fd);
// 		exit(0);
// 	}
// 	// O_WRONLY - write only
// 	// O_CREAT - create file if it doesn't exist
// 	// O_TRUNC - truncates file to length 0 if it already exists (deletes existing file)
// 	// opens a file for writing; erases old file/creates a new file
// 	dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC);	
// 	if (dst_fd < 0) { // open() error checking
// 		fprintf(stderr, "unable to open/create %s for writing: %i\n", dst_filename, errno);
// 		close(dst_fd);
// 		exit(0);
// 	}


// 	// read/ write loop
// 	buf = malloc((size_t)BUF_SIZE);   // allocate a buffer to store read data
// 	// reads up to BUF_SIZE bytes from src_filename
// 	// writes bytes_read to dst_filename
	
// 	// closes src_fd file descriptor
// 	// closes dst_fd file descriptor
// 	// frees memory used for buf

//     int nw = write(src_fd, buf, strlen(buf));
  
//     fclose(src_filename);
//     fclose(dst_filename);
//     free(buf);  

// 	return 0;
// }

// checks the runtime to copy a file using functions vs syscalls
// void check_copy_times(char* src_filename, char* dst_filename) {
// 	clock_t func_start, func_end, syscall_start, syscall_end;
	
// 	// capture runtime of func_copy() using start clock, call the copy, end the clock
	

// 	// capture runtime of syscall_copy() using start clock, call the copy, end the clock
	

// 	printf("time to copy using functions: %.7f\n", func_time);
// 	printf("time to copy using syscalls: %.7f\n", syscall_time);
// }

int main(int argc, char* argv[]) {
	if (argc != 3) {  // check correct usage
		fprintf(stderr, "usage: %s <src_filename> <dst_filename>\n", argv[0]);
		exit(1);
	}
	//get the source and destination files from the command line arguments
	//call the check copy times function
    func_copy(argv[1], argv[2]);
	return 0;
}