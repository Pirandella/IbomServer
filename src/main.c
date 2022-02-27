#include <stdio.h>

#include "index.h"
#include "http.h"

int main(int argc, char **argv)
{
	index_t *index = index_init(NULL);

	index_files("/home/sergej/Documents/KiCAD", index);
	index_list(index);

	// http_start(3200);

	index_free(index);

	return 0;
}