#include "system.h"

pid_t
spawnvpio(int *fd0, int *fd1, int *fd2, char *av[])
{
	pid_t	pid;
	int	p0[2], p1[2], p2[2];
	int	rc, old0, old1, old2;

	old0 = old1 = old2 = -1;	/* avoid warnings */
	if (fd0) {
		if (mkpipe(p0, BIG_PIPE) == -1) {
			perror("pipe");
			return (-1);
		}
		assert(p0[0] > 1); /* should not use stdin and stdout */
		old0 = dup(0); assert(old0 > 0);
		rc = dup2(p0[0], 0); assert(rc != -1);
		(close)(p0[0]);
		make_fd_uninheritable(p0[1]);
	}
	if (fd1) {
		if (mkpipe(p1, BIG_PIPE) == -1) {
			perror("pipe");
			return (-1);
		}
		assert(p1[0] > 1); /* should not use stdin and stdout */
		old1 = dup(1); assert(old1 > 0);
		rc = dup2(p1[1], 1); assert(rc != -1);
		(close)(p1[1]);
		make_fd_uninheritable(p1[0]);
	}
	if (fd2) {
		if (mkpipe(p2, BIG_PIPE) == -1) {
			perror("pipe");
			return (-1);
		}
		assert(p2[0] > 1); /* should not use stdin and stdout */
		old2 = dup(2); assert(old2 > 0);
		rc = dup2(p2[1], 2); assert(rc != -1);
		(close)(p2[1]);
		make_fd_uninheritable(p2[0]);
	}

	/*
	 * Now go do the real work...
	 */
	pid = spawnvp(_P_NOWAIT, av[0], av);

	/* For Parent, restore handles */
	if (fd0) {
		rc = dup2(old0, 0); assert(rc != -1);
		(close)(old0);
		*fd0 = p0[1];
		if (pid < 0) {
			(close)(*fd0);
			*fd0 = -1;
		}
	}
	if (fd1) {
		rc = dup2(old1, 1); assert(rc != -1);
		(close)(old1);
		*fd1 = p1[0];
		if (pid < 0) {
			(close)(*fd1);
			*fd1 = -1;
		}
	}
	if (fd2) {
		rc = dup2(old2, 0); assert(rc != -1);
		(close)(old2);
		*fd2 = p2[0];
		if (pid < 0) {
			(close)(*fd2);
			*fd2 = -1;
		}
	}
	return (pid);

}