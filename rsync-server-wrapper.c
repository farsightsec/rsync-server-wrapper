#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *ok_path = "/.rsync-server-wrapper-ok";
static const char *rsync_server_path = "/bin/rsync";

int
main(int argc, char **argv)
{
	if (argc != 3)
		exit (EXIT_FAILURE);

	/* require permission to run the rsync server */
	struct stat statbuf;
	if (stat(ok_path, &statbuf) < 0 || statbuf.st_uid != 0) {
		puts("rsync-server-wrapper: no access");
		exit(EXIT_FAILURE);
	}

	/* count the number of arguments in the command string */
	char *arg_str = argv[2];
	char *p = arg_str;
	int arg_count = 1;
	while (*p != '\0')
		if (*p++ == ' ')
			arg_count++;

	/* convert the command string into the new argument vector */
	int new_argv_idx = 0;
	char *new_argv[arg_count + 1];
	char *tok = strtok(arg_str, " ");
	do
		new_argv[new_argv_idx++] = tok;
	while ((tok = strtok(NULL, " ")) != NULL);
	new_argv[new_argv_idx] = NULL;

	/* require that "--server" appear as the first argument */
	if (arg_count < 2 || strcmp(new_argv[1], "--server") != 0)
		exit(EXIT_FAILURE);

	/* execute the server */
	if (execve(rsync_server_path, new_argv, NULL) < 0) {
		perror("execve");
		exit(EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
