#include<string.h>


void memset(void *ptr, int value, uint64_t num)
{
        int i = 0;
        for(; i < num; i++)
                ((char *)ptr)[i] = 0;
}

int strlen(char* string) {

        int i;
        if(string == 0)
        {
                //puts("empty string\n");
                return -1;
        }
        for (i=0; string[i] != '\0'; i++);

        return i;
}

char* strcopy (char* dest, const char* source, int numchar) {

	int i;
	for (i=0; source[i] != '\0' && i < numchar-1; i++) {
		dest[i] = source[i];
	}
	dest[i] = '\0';

	return dest;
}

int strcmp (const char* s1, const char* s2, int numchar) {

	int i;

	for (i=0; i<numchar && s1[i] != '\0' && s2[i] != '\0'; i++) {
		if (s1[i] < s2[i])
			return -1;	
		else if (s1[i] > s2[i])
			return 1;						 
	}
	if (i == numchar)
		return 0;
		    
	if (s1[i] == '\0' && s2[i] != '\0')
		return -1;
	else if (s1[i] != '\0')
		return 1;	
	else
		return 0;
}

char* strcat(char* dest, const char* source) {

        //      We assume, in good faith, that dest and source are
        //      null-terminated, and dest has more than enough space to
        //      accomodate src.
        int i, j;
        if( source == 0 )
                return dest;
        if( dest == 0 && source != 0)
        {

                for (j=0; source[j] != '\0'; j++)
                {
                        dest[j] = source[j];
                }
                dest[j] = '\0';
                return dest;
        }
        // Iterate through all of dest until we arrive at '\0' character
        for (i=0; dest[i] != '\0'; i++);

        for (j=0; source[j] != '\0'; j++, i++) {
                dest[i] = source[j];
        }
        dest[i] = '\0';

        return dest;
}
