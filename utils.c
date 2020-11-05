#include "utils.h"

bool debug = false;

int is_number(const char* str)
{
	
	for(int i =0; str[i] != 0; i++)
	{
		if(!isdigit(str[i]))
			return 0;
	}
	
	return 1;
}

void print_progress(float progress, int max){
	const char prefix[]= "Transfering: [";
	const char suffix[]= "]";
	const size_t prefix_length= sizeof(prefix)-1;
	const size_t suffix_length = sizeof(suffix)-1;
	char * buffer= calloc(max+prefix_length+suffix_length+1,1); //+1 for end
	int i=0;
	
	strcpy(buffer, prefix);
	
	progress=progress/max*100;
	for (; i< 100; ++i){
	
		buffer[prefix_length+i]=i<progress?'#':'.';		
	}	
	
	strcpy(&buffer[prefix_length +i],suffix);
	printf("\b%c[2K\r%s %d%%\n", 27, buffer,(int)progress);
	
	fflush(stdout);
	free(buffer);
}

void corrupt(unsigned char* frame, int size)
{
	int corrupted_count = rand()%(int)(size*0.1);

	for (int n = 0; n < corrupted_count; n++)
	{
		int i = (rand()%(size-6))+4;
		frame[i] = rand()%0xff; 
	}
}