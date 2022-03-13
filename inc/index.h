#ifndef _INDEX_H_
#define _INDEX_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>
#include <limits.h>
#define __USE_MISC
#include <stdlib.h>
#include <dirent.h>

/* Macros to get specific directories name */
#define INDEX_DIR_NAME(_idx, _dir)		   (_idx->dir[_dir].dir_name)
/* Macros to get specific file name from the specific directory */
#define INDEX_FILE_NAME(_idx, _dir, _file) (_idx->dir[_dir].file_name[_file])
/* Macros to get specific file path from the specific directory */
#define INDEX_FILE_PATH(_idx, _dir, _file) (_idx->dir[_dir].file_path[_file])
/* Macros to get number of dirctories */
#define INDEX_DIR_NUM(_idx)				   (_idx->n_dirs)
/* Macros to get number of files in the specific directory */
#define INDEX_FILE_NUM(_idx, _dir)		   (_idx->dir[_dir].n_files)

#define DEF_REGEX "^.*\\.(html|htm)$"  // Default regular expression

#define DEF_PATH_SIZE	   2048	 // Default path buffer size
#define DEF_DIR_ALLOC_SIZE 16	 // Ammount of struct _dir allocated by default
#define DEF_BOM_ALLOC_SIZE 5	 // Ammonut of pointers allocated for file name/path string paris

typedef struct {					 // Directories and files index stucture
	char *regex;					 // Regex string that will be used to match bom files
	char *path;						 // Path to the projects root directory
	char *name;						 // Projects root directory name
	struct _dir {					 // Contains data about avaliable bom files
		char *dir_name;				 // Project directory name
		char **file_name;			 // Bom file name
		char **file_path;			 // Full path to a bom file
		unsigned int n_files;		 // Number of bom files in projct directory
		unsigned int n_files_alloc;	 // Number of allocated bom name/path strings pairs
	} * dir;
	unsigned int n_dirs;		// Number of used struct _dir */
	unsigned int n_dirs_alloc;	// Number of allocated struct _dir
} index_t;

index_t *index_init(char *const regex);
void index_set_regex(index_t *index, const char *regex);
void index_files(const char *directory, index_t *index);
void index_list(const index_t *index);
void index_free(index_t *index);

#endif /* _INDEX_H_ */