#include "index.h"

/* Macro for quick acces to the current directory name string */
#define DIR_NAME_STR				  (index->dir[index->n_dirs].dir_name)
/* Macro for quick acces to the specific directory name string */
#define _DIR_NAME_STR(__dir)		  (index->dir[__dir].dir_name)
/* Macro for quick acces to file name string */
#define FILE_NAME_STR				  (index->dir[index->n_dirs].file_name[index->dir[index->n_dirs].n_files])
/* Macro for quick acces to the specifc file name string in the specific directory */
#define _FILE_NAME_STR(__dir, __file) (index->dir[__dir].file_name[__file])
/* Macro for quick acces to the file path string */
#define FILE_PATH_STR				  (index->dir[index->n_dirs].file_path[index->dir[index->n_dirs].n_files])
/* Macro for quick acces to the specific file path string in the specific directory */
#define _FILE_PATH_STR(__dir, __file) (index->dir[__dir].file_path[__file])
/* Macro for quick access to the number of found directories with files that match regex */
#define NUM_OF_DIRS					  (index->n_dirs)
/* Macro for getting quick access to number of files in current directory */
#define NUM_OF_FILES				  (index->dir[index->n_dirs].n_files)
/* Macro for quick access to the number of files found in a particular project directory */
#define _NUM_OF_FILES(__dir)		  (index->dir[__dir].n_files)

__always_inline char *alloc_str_mem(size_t size)
{
	char *str = NULL;
	if ((str = malloc(sizeof(char) * (size + 1))) == NULL) {
		perror("Failes allocate memory for string");
		exit(-1);
	}

	return str;
}

__always_inline void realloc_proj_str_mem(index_t *const index, size_t size)
{
	if ((DIR_NAME_STR = realloc(DIR_NAME_STR, sizeof(char) * (size + 1))) == NULL) {
		perror("Failed to reallocate string memory");
		exit(-2);
	}
}

__always_inline void realloc_bom_name_str_mem(index_t *const index, size_t size)
{
	if ((FILE_NAME_STR = realloc(FILE_NAME_STR, sizeof(char) * (size + 1))) == NULL) {
		perror("Failed to reallocate string memory");
		exit(-2);
	}
}

__always_inline void realloc_file_path_str_mem(index_t *const index, size_t size)
{
	if ((FILE_PATH_STR = realloc(FILE_PATH_STR, sizeof(char) * (size + 1))) == NULL) {
		perror("Failed to reallocate string memory");
		exit(-2);
	}
}

__always_inline void realloc_dir_mem(index_t *index)
{
	index->n_dirs_alloc += DEF_DIR_ALLOC_SIZE;
	if ((index->dir = realloc(index->dir, sizeof(*index->dir) * index->n_dirs_alloc)) == NULL) {
		perror("Failed to reallocate memory index_t->*dir");
		exit(-3);
	}

	size_t offset = index->n_dirs_alloc - DEF_DIR_ALLOC_SIZE;
	for (int i = offset; i < index->n_dirs_alloc; i++) {
		if (((index->dir[i].file_name = malloc(sizeof(char *) * DEF_BOM_ALLOC_SIZE)) == NULL)) {
			perror("Falied to allocate memory for index_t->struct *_dir->char *file_name");
			exit(-4);
		}
		if (((index->dir[i].file_path = malloc(sizeof(char *) * DEF_BOM_ALLOC_SIZE)) == NULL)) {
			perror("Falied to allocate memory for index_t->struct *_dir->char *file_path");
			exit(-5);
		}
	}
	/* Initialize newly allocated data */
	for (int i = offset; i < index->n_dirs_alloc; i++) {
		index->dir[i].dir_name = NULL;
		index->dir[i].n_files = 0;
		index->dir[i].n_files_alloc = DEF_BOM_ALLOC_SIZE;
		for (int j = 0; j < index->dir[i].n_files_alloc; j++) {
			index->dir[i].file_name[j] = NULL;
			index->dir[i].file_path[j] = NULL;
		}
	}
}

__always_inline void realloc_bom_mem(index_t *index)
{
	index->dir[index->n_dirs].n_files_alloc += DEF_BOM_ALLOC_SIZE;
	if ((index->dir[index->n_dirs].file_name = realloc(
			 index->dir[index->n_dirs].file_name, sizeof(char *) * index->dir[index->n_dirs].n_files_alloc)) == NULL) {
		perror("Failed to realloca memory index_t->*dir->char *file_name");
		exit(-6);
	}
	if ((index->dir[index->n_dirs].file_path = realloc(
			 index->dir[index->n_dirs].file_path, sizeof(char *) * index->dir[index->n_dirs].n_files_alloc)) == NULL) {
		perror("Failed to realloca memory index_t->*dir->char *file_path");
		exit(-7);
	}
	/* Initialize newly allocated file name/path string pairs */
	size_t offset = index->dir[index->n_dirs].n_files_alloc - DEF_BOM_ALLOC_SIZE;
	for (int i = offset; i < index->dir[index->n_dirs].n_files_alloc; i++) {
		index->dir[index->n_dirs].file_name[i] = NULL;
		index->dir[index->n_dirs].file_path[i] = NULL;
	}
}

static int finde_files(const char *const name, index_t *const index, int depth, regex_t regex)
{
	struct dirent *dp;
	DIR *dir;

	if (!(dir = opendir(name))) {
		perror("Failed to index file in directory");
		exit(-10);
	}
	while ((dp = readdir(dir)) != NULL) {
		char path[DEF_PATH_SIZE];
		/* If directory/file name starts with a dot symbol, skip it */
		if (dp->d_name[0] == '.') continue;
		/* Build file path string */
		strcpy(path, name);
		strcat(path, "/");
		strcat(path, dp->d_name);
		/* Directory found */
		if (dp->d_type == DT_DIR) {
			depth++;  // We about to go deeper
			depth = finde_files(path, index, depth, regex);
			/* When we returned to root directory and found files */
			if (depth == 0 && NUM_OF_FILES > 0) {
				/* Allocate more struct _dir if we reache limit of allocated dirs */
				if ((index->n_dirs_alloc - 1) == NUM_OF_DIRS) { realloc_dir_mem(index); }
				/* Check if dir_name string was allocated earlier */
				if (DIR_NAME_STR != NULL) {
					size_t len = strlen(dp->d_name);
					/* Expand dir_name string if current dir_name is longer */
					if (strlen(DIR_NAME_STR) < len) { realloc_proj_str_mem(index, len); }
				} else {
					DIR_NAME_STR = alloc_str_mem(strlen(dp->d_name));
				}
				strcpy(DIR_NAME_STR, dp->d_name);
				/* Increment number of used directories */
				NUM_OF_DIRS++;
			}
		} else {  // File found
			/* Match file name with regex */
			if (!regexec(&regex, dp->d_name, 0, NULL, 0)) {
				/* Allocate more space for the file name/path string pairs if allocated limit is reached */
				if ((index->dir[index->n_dirs].n_files_alloc) == NUM_OF_FILES) { realloc_bom_mem(index); }
				/* Check if memory for file name/path pair was allocate earlier */
				if (FILE_NAME_STR != NULL) {
					size_t name_len = strlen(dp->d_name);
					size_t path_len = strlen(path);
					if (strlen(FILE_NAME_STR) < name_len) { realloc_bom_name_str_mem(index, name_len); }
					if (strlen(FILE_PATH_STR) < path_len) { realloc_file_path_str_mem(index, path_len); }
				} else {
					FILE_NAME_STR = alloc_str_mem(strlen(dp->d_name));
					FILE_PATH_STR = alloc_str_mem(strlen(path));
				}
				strcpy(FILE_NAME_STR, dp->d_name);
				strcpy(FILE_PATH_STR, path);
				/* Increment number of files in current directory */
				NUM_OF_FILES++;
			}
		}
	}

	depth--;
	closedir(dir);
	return depth;
}

index_t *index_init(char *const regex)
{
	/* Allocate memory for index_t */
	index_t *index = NULL;
	if ((index = malloc(sizeof(index_t))) == NULL) {
		perror("Failed to allocate memory for index_t *index");
		exit(1);
	}
	/* Initialize dir number and number of allocated struct _dir */
	index->n_dirs = 0;
	index->n_dirs_alloc = DEF_DIR_ALLOC_SIZE;

	/* Set regex */
	size_t regex_len = 0;
	char *_regex;
	/* If custom regex was supplied */
	if (regex != NULL) {
		regex_len = strlen(regex) + 1;
		_regex = regex;
	} else {
		regex_len = strlen(DEF_REGEX) + 1;
		_regex = DEF_REGEX;
	}
	if ((index->regex = malloc(sizeof(char) * regex_len)) == NULL) {
		perror("Failed to allocate memory for index_t->char *regex");
		exit(2);
	}
	strcpy(index->regex, _regex);

	if ((index->dir = malloc(sizeof(*index->dir) * index->n_dirs_alloc)) == NULL) {
		perror("Falied to allocate memory for index_t->struct _dir *dir");
		exit(3);
	}
	for (int i = 0; i < index->n_dirs_alloc; i++) {
		if (((index->dir[i].file_name = malloc(sizeof(char *) * DEF_BOM_ALLOC_SIZE)) == NULL)) {
			perror("Falied to allocate memory for index_t->struct *_dir->char *file_name");
			exit(4);
		}
		if (((index->dir[i].file_path = malloc(sizeof(char *) * DEF_BOM_ALLOC_SIZE)) == NULL)) {
			perror("Falied to allocate memory for index_t->struct *_dir->char *file_path");
			exit(5);
		}
	}
	/* Finish initializing index_t */
	for (int i = 0; i < index->n_dirs_alloc; i++) {
		index->dir[i].dir_name = NULL;
		index->dir[i].n_files = 0;
		index->dir[i].n_files_alloc = DEF_BOM_ALLOC_SIZE;
		for (int j = 0; j < index->dir[i].n_files_alloc; j++) {
			index->dir[i].file_name[j] = NULL;
			index->dir[i].file_path[j] = NULL;
		}
	}

	return index;
}

void index_set_regex(index_t *const index, const char *const regex)
{
	if (strlen(regex) > strlen(index->regex)) {
		if ((index->regex = realloc(index->regex, sizeof(char) * (strlen(regex) + 1))) == NULL) {
			perror("Failed to reallocate memory for index_t->char *regex");
			exit(-9);
		}
		strcpy(index->regex, regex);
	} else {
		strcpy(index->regex, regex);
	}
}

void index_files(const char *const directory, index_t *index)
{
	regex_t regex;
	if (regcomp(&regex, index->regex, REG_EXTENDED)) {
		perror("Failed to compile regex");
		exit(1);
	}

	char *path = realpath(directory, NULL);
	if (path == NULL) {
		perror("Failed to resolve path");
		exit(2);
	}

	if (index->n_dirs != 0) {
		for (int i = 0; i < index->n_dirs; i++) {
			index->dir[i].n_files = 0;
		}
		index->n_dirs = 0;
	}

	finde_files(path, index, 0, regex);

	free(path);
	regfree(&regex);
}

void index_list(const index_t *const index)
{
	int n_files = 0;
	for (int i = 0; i < index->n_dirs; i++) {
		printf("\u25BC [%s] %d %s\n", index->dir[i].dir_name, index->dir[i].n_files,
			((index->dir[i].n_files > 1) ? "files" : "file"));
		for (int j = 0; j < index->dir[i].n_files; j++) {
			printf("%s %s\n", ((j == (index->dir[i].n_files - 1)) ? "\u2514" : "\u251C"), index->dir[i].file_name[j]);
		}
		n_files += index->dir[i].n_files;
	}
	puts("\n--------------------------------------------------");
	printf("Total:\n\tdirectories: %d\n\tfiles: %d\n", index->n_dirs, n_files);
}

void index_free(index_t *index)
{
	if (index == NULL) return;

	for (int i = 0; i < index->n_dirs_alloc; i++) {
		if (index->dir[i].dir_name != NULL) free(index->dir[i].dir_name);
		for (int j = 0; j < index->dir[i].n_files_alloc; j++) {
			if (index->dir[i].file_name[j] != NULL) { free(index->dir[i].file_name[j]); }
			if (index->dir[i].file_path[j] != NULL) { free(index->dir[i].file_path[j]); }
		}
		free(index->dir[i].file_name);
		free(index->dir[i].file_path);
	}
	free(index->dir);
	free(index->regex);
	free(index);
}