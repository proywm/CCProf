/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Compile for Blue Gene/Q compute node. */
/* #undef MONITOR_BLUE_GENE_Q */

/* List of signals kept open for the client. */
#define MONITOR_CLIENT_SIGNALS_LIST 

/* Debug mode is always on. */
/* #undef MONITOR_DEBUG_DEFAULT_ON */

/* pthread_cleanup_push is a function, not a macro. */
/* #undef MONITOR_PTHREAD_CLEANUP_PUSH_IS_FCN */

/* pthread_equal is a function, not a macro. */
#define MONITOR_PTHREAD_EQUAL_IS_FCN 1

/* Libc start main type is ppc. */
/* #undef MONITOR_START_MAIN_PPC */

/* Include support for dlopen. */
#define MONITOR_USE_DLOPEN 1

/* Include support for fork and exec families. */
#define MONITOR_USE_FORK 1

/* Include support for memleak. */
#define MONITOR_USE_MEMLEAK 1

/* Include support for MPI. */
#define MONITOR_USE_MPI 1

/* Include support for pthreads. */
#define MONITOR_USE_PTHREADS 1

/* Include support for signals. */
#define MONITOR_USE_SIGNALS 1

/* Name of package */
#define PACKAGE "libmonitor"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "hpctoolkit-forum@rice.edu"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libmonitor"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libmonitor 0.x"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libmonitor"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.x"

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* SVN revision. */
#define SVN_REVISION 149

/* Version number of package */
#define VERSION "0.x"
