#include "IO.h"

long loadArray(int **array, char file[]){ // ARRAY INDIRECTION
	FILE *ptr;
	long file_size;

	// Open File
	ptr = fopen(file, "rb");

	// If file exists
	if (ptr != NULL){
		// Find size of file and store value in bytes
		fseek(ptr, 0, SEEK_END);
		file_size = ftell(ptr);
		rewind(ptr);

		// Allocate array; Filesize in bytes / Int size (4) = Elements
		*array = (int*) calloc(file_size/sizeof(int), file_size);

		// Read file to array; 4 Bytes per element; Filesize/4 elements.
		fread(*array, sizeof(int), file_size/sizeof(int), ptr);
		fclose(ptr);

		// Return file size in int elements.
		return file_size / sizeof(int);
	}
	else {
		// Opening file failed; Exit.
		exit(1);
	}

}