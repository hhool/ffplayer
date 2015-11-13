/**
 * @file   test_crc32.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Mon Jan 18 11:39:45 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_crc32.h"
#include "util_log.h"

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define BUFFER_SIZE 2048

int main (int argc, char* argv[]) {
	if (argc < 2) {
		printf ("usage: %s file\n", argv[0]);
		return -1;
	}
	unsigned char buffer[BUFFER_SIZE] = {0};
	int fd = open (argv[1], O_RDONLY);
	int count = read (fd, buffer, BUFFER_SIZE);
	if (count >= BUFFER_SIZE) {
		ERROR ("file size bigger than %d", BUFFER_SIZE);
	}
	
        unsigned int crc = UtilCrc32::crc32(buffer, count);
        printf ("crc :%08x (%d)\n", crc, crc);
	return 0;
}
