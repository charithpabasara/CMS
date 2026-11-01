#include <stdio.h>
#include <stdlib.h>
#include "../lib/clearscreen.h"

void clearScreen()					// This function clear the screen
{
	#ifdef _WIN32
			system ("cls");			// Check the system is windows
	#else
			system ("clear");		// Check the system is linux
	#endif
}