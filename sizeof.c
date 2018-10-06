#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>
#include <stdarg.h>
#include <stdlib.h>

// https://rosettacode.org/wiki/Walk_a_directory/Recursively#Library:_POSIX
// forgive me, senpai.

enum {
	WALK_OK = 0,
	WALK_BADPATTERN,
	WALK_NAMETOOLONG,
	WALK_BADIO,
};
const char * FILE_SIZES[2] = { "B", "KB", "MB", "GB", "TB", "PB" };

#define WS_NONE		0
#define WS_RECURSIVE	(1 << 0)
#define WS_DEFAULT	WS_RECURSIVE
#define WS_FOLLOWLINK	(1 << 1)	/* follow symlinks */
#define WS_DOTFILES	( 1 << 2 )	/* per unix convention, .file is hidden */


int walk_dir(char *dname, int spec, long double *size)
{
	struct dirent *dent;
	DIR *dir;
	struct stat st;
	char fn[FILENAME_MAX];
	int res = WALK_OK;
	int len = strlen(dname);

	if (len >= FILENAME_MAX - 1)
		return WALK_NAMETOOLONG;

	strcpy(fn, dname);
	fn[len++] = '/';

	if (!(dir = opendir(dname))) {
		warn("can't open %s", dname);
		return WALK_BADIO;
	}

	errno = 0;
	while ((dent = readdir(dir))) {
        if (!(spec & WS_DOTFILES) && dent->d_name[0] == '.')
			continue;

		if (!strcmp(dent->d_name, ".") || !strcmp(dent->d_name, ".."))
			continue;

		strncpy(fn + len, dent->d_name, FILENAME_MAX - len);
		if (lstat(fn, &st) == -1) {
			warn("Can't stat %s", fn);
			res = WALK_BADIO;
			continue;
		}

		/* don't follow symlink unless told so */
		if (S_ISLNK(st.st_mode) && !(spec & WS_FOLLOWLINK))
			continue;

		/* will be false for symlinked dirs */
		if (S_ISDIR(st.st_mode)) {
			/* recursively follow dirs */
			if ((spec & WS_RECURSIVE))
				walk_dir(fn, spec, size);
		}
        if (S_ISREG(st.st_mode)) {
            *size += st.st_size;
        }
    }

	if (dir) closedir(dir);
	return res ? res : errno ? WALK_BADIO : WALK_OK;
}

long double walk_dir_size(char *dname, int spec)
{
  long double sizebuffer = 0;

  if (walk_dir(dname, spec, &sizebuffer)){
    perror("Failed to walk directory.");
    exit(1);
  }
  return sizebuffer;
}

long double file_size(char *fp){
    struct stat st;
    stat(fp, &st);
    return (long double)st.st_size;
}

int isdir(char *fp){
  struct stat st;
  stat(fp, &st);
  return S_ISDIR(st.st_mode);
}

int is_digit(char *string){
    // Returns 0 if okay.
    const char numbers[11] = "0987654321";
    int i, y;
    for (i = 0; i < strlen(string); i++){
        for (y = 0; y < strlen(numbers); i++){
            if (string[i] != numbers[y]){
                return 1;
            }
        }
    }
    return 0;
}

void humanize(long double bytes, char * buffer){
    if (bytes > 0) {
    	double i, p;
    	long double s;
    	i = floor(log(bytes) / log(1024));
    	p = pow(1024, i);
    	s = bytes/p;
    	sprintf(buffer, "%llf %s", s, FILE_SIZES[(int)i]);
    }
    else {
    	sprintf(buffer, "0 B");
    }
}


int main(int argc, char *argv[]){
    long double bytes_cnt;
    char output[24];
    long size;
    char *buf;
    char *fp;
    char *cwd;

    size = pathconf(".", _PC_PATH_MAX);
    if ((buf = (char *)malloc((size_t)size)) != NULL)
        cwd = getcwd(buf, (size_t)size);
        if (argc == 1){
            fp = cwd;
        }

    if (argc > 1){
        if (is_digit(argv[1]) == 1){
            fp = argv[1];
        }
        else {
            printf("Not implemented");
            exit(0);
        }
    }

    if (isdir(fp)){
        bytes_cnt = walk_dir_size(fp, WS_DEFAULT);
    }
    else {
        bytes_cnt = file_size(fp);
    }

    humanize(bytes_cnt, output);
    printf("%s\n", output);
    return 0;

}
