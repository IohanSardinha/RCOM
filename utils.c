#include "utils.h"

int is_number(const char* str)
{
	
	for(int i =0; str[i] != 0; i++)
	{
		if(!isdigit(str[i]))
			return 0;
	}
	
	return 1;
}