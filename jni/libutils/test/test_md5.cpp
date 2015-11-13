/**
 * @file   test_crc32.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Mon Jan 18 11:39:45 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_md5sum.h"
#include "util_log.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <string>

using namespace std;

#define BUFFER_SIZE 2048

int md5_file (const char* filename) {
        string str_md5 = UtilMd5::HashFile(filename);
        printf ("md5:%s\n", str_md5.c_str());
	return 0;
}

int md5_buffer (const char* filename) {
	DEBUG ("open file:%s",filename);
        unsigned char buffer[BUFFER_SIZE] = {0};
        int fd = open (filename, O_RDONLY);
	if (fd < 0) {
		ERROR ("open file=%s fail!\n", filename);
	}
        int count = read (fd, buffer, BUFFER_SIZE);
        if (count >= BUFFER_SIZE) {
                ERROR ("read size %d equal or bigger than %d, strlen=%d", count, BUFFER_SIZE, strlen((char*)buffer));
        }
	close (fd);

	string str_md5 = UtilMd5::HashBuffer(buffer, count);
	printf ("buffer:%s\n", buffer);
        printf ("md5:%s\n", str_md5.c_str());
        return 0;
}


int main (int argc, char* argv[]) {
	if (argc < 2) {
		printf ("usage: %s file\n", argv[1]);
		return -1;
	}

	DEBUG ("file mode");
	md5_file(argv[1]);

	DEBUG ("buffer mode");
	md5_buffer (argv[1]);
	return 0;
}
