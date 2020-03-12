/*
 * so-cpp.c
 */

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

int main(int argc, char *argv[])
{
	unsigned int i;
	int ret;
	size_t no_header_dirs;
	char **header_dirs;
	hash_t defines;
	FILE *fin;
	FILE *fout;

	/* Init structures */

	ret = hashmap_init(&defines);
	DIE(ret == FAILURE, FAILURE);

	fin = NULL;
	fout = NULL;

	/* Execute */
	ret = parse_input(argc, argv, defines, &header_dirs,
			&no_header_dirs, &fin, &fout);
	DIE(ret != OK, ret);

	ret = process_file(defines, header_dirs, no_header_dirs);
	DIE(ret != OK, ret);

	/* Free everything */
	destroy_hash(defines);

	if (header_dirs[0] != NULL)
		free(header_dirs[0]);
	for (i = 1; i < no_header_dirs; ++i)
		free(header_dirs[i]);
	free(header_dirs);

	if (fin != NULL)
		fclose(fin);
	if (fout != NULL)
		fclose(fout);

	return OK;
}
