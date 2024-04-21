//ilyas.cherifialaoui@gmail.com

#ifndef __GETOPT_H_
	#define __GETOPT_H_

	#ifdef _GETOPT_API
		#undef _GETOPT_API
	#endif

	#if defined(EXPORTS_GETOPT) && defined(STATIC_GETOPT)
		#error "The preprocessor definitions of EXPORTS_GETOPT and STATIC_GETOPT can only be used individually"
	#elif defined(STATIC_GETOPT)
		#pragma message("Warning static builds of getopt violate the Lesser GNU Public License")
		#define _GETOPT_API
	#elif defined(EXPORTS_GETOPT)
		#pragma message("Exporting getopt library")
		#define _GETOPT_API __declspec(dllexport)
	#else
		#pragma message("Importing getopt library")
		#define _GETOPT_API __declspec(dllimport)
	#endif

	// Change behavior for C\C++
	#ifdef __cplusplus
		#define _BEGIN_EXTERN_C extern "C" {
		#define _END_EXTERN_C }
		#define _GETOPT_THROW throw()
	#else
		#define _BEGIN_EXTERN_C
		#define _END_EXTERN_C
		#define _GETOPT_THROW
	#endif

	// Standard GNU options
	#define	null_argument		0	/*Argument Null*/
	#define	no_argument			0	/*Argument Switch Only*/
	#define required_argument	1	/*Argument Required*/
	#define optional_argument	2	/*Argument Optional*/	

	// Shorter Options
	#define ARG_NULL	0	/*Argument Null*/
	#define ARG_NONE	0	/*Argument Switch Only*/
	#define ARG_REQ		1	/*Argument Required*/
	#define ARG_OPT		2	/*Argument Optional*/

	#include <string.h>

_BEGIN_EXTERN_C

	extern _GETOPT_API int optind;
	extern _GETOPT_API int opterr;
	extern _GETOPT_API int optopt;

	struct option_a
	{
		const char* name;
		int has_arg;
		int *flag;
		int val;
	};
	extern _GETOPT_API char *optarg_a;
	extern _GETOPT_API int getopt_a(int argc, char *const *argv, const char *optstring) _GETOPT_THROW;
	extern _GETOPT_API int getopt_long_a(int argc, char *const *argv, const char *options, const struct option_a *long_options, int *opt_index) _GETOPT_THROW;
	extern _GETOPT_API int getopt_long_only_a(int argc, char *const *argv, const char *options, const struct option_a *long_options, int *opt_index) _GETOPT_THROW;

_END_EXTERN_C

	#undef _BEGIN_EXTERN_C
	#undef _END_EXTERN_C
	#undef _GETOPT_THROW
	#undef _GETOPT_API

	#define getopt getopt_a
	#define getopt_long getopt_long_a
	#define getopt_long_only getopt_long_only_a
	#define option option_a
	#define optarg optarg_a

#endif  // __GETOPT_H_
