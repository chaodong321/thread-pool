#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>

/*
 *功能：设置进程coredump大小
 */
int set_coredump()
{
	struct rlimit lim;     /*existing limit*/
    struct rlimit new_lim; /*desired limit*/
	
	/* we want no limit on coredump size */
    if (lim.rlim_max == RLIM_INFINITY && lim.rlim_cur == RLIM_INFINITY) {
        printf ("Core dump size is unlimited.\n");
        return 1;
    }
    else {
        new_lim.rlim_max = RLIM_INFINITY;
        new_lim.rlim_cur = RLIM_INFINITY;
        if (setrlimit (RLIMIT_CORE, &new_lim) == 0) {
            printf ("Core dump size set to unlimited.\n");
            return 1;
        }
        if (errno == EPERM) {
            /* couldn't raise the hard limit to unlimited;
             * try increasing the soft limit to the hard limit instead */
            if (lim.rlim_cur < lim.rlim_max) {
                new_lim.rlim_cur = lim.rlim_max;
                if (setrlimit (RLIMIT_CORE, & new_lim) == 0) {
                    printf ("Could not set core dump size to unlimited; core dump size set to the hard limit.");
                    return 0;
                }
                else {
                    printf ("Failed to set core dump size to unlimited or to the hard limit.");
                    return 0;
                }
            }
            printf ("Could not set core dump size to unlimited; it's set to the hard limit.");
            return 0;
        }
    }
	return 0;
}

