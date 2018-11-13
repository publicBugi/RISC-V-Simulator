#include "IO.h"

int loadArray(int **array, char file[]){
	printf("Loading file.\n");
	FILE *ptr;	// File pointer
	ptr = fopen(file, "rb"); // Open File stream
	if (ptr != NULL){
		unsigned int buffer;					// Temporary buffer
		int pc_max;
		fseek(ptr, 0L, SEEK_END);				// Find end of file
		pc_max = ftell(ptr);					// Save size of file (maximum program counter)
		printf("File size = %d\n", pc_max);
		rewind(ptr);							// Go to start of file
		*array = (int*) malloc(pc_max*sizeof(int));			// Allocate memory to program data
		for (int i = 0; i < pc_max; i++){		// Iterate through program, save to program array.
			fread(&buffer, sizeof(int), 1, ptr); 	// Fetch instruction from file.
			*(*(array) + i) = buffer;					 	// Save instruction to array.
			//printf("%#010x\n", *(*(array) + i)); 		// Debugging tool
		}
		fclose(ptr);
		return(pc_max);
	}
	else {
		printf("Unable to open file.\n");
		exit(1);
	}
}