/* timer.h  -  Cross-platform timer library  -  Public Domain  -  2011 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform interface to measure
 * elapsed time with (at least) millisecond accuracy.
 *
 * This library is put in the public domain; you can redistribute
 * it and/or modify it without any restrictions.
 *
 */

#include "timer.h"

#define TIMER_PLATFORM_WINDOWS 0
#define TIMER_PLATFORM_APPLE   0
#define TIMER_PLATFORM_POSIX   0

#if defined( _WIN32 ) || defined( _WIN64 )
#  undef  TIMER_PLATFORM_WINDOWS
#  define TIMER_PLATFORM_WINDOWS 1
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#elif defined( __APPLE__ )
#  undef  TIMER_PLATFORM_APPLE
#  define TIMER_PLATFORM_APPLE 1
#  include <mach/mach_time.h>
#  include <string.h>
static mach_timebase_info_data_t _timerlib_info;
static void absolutetime_to_nanoseconds (uint64_t mach_time, uint64_t* clock ) { *clock = mach_time * _timerlib_info.numer / _timerlib_info.denom; }
#else
#  undef  TIMER_PLATFORM_POSIX
#  define TIMER_PLATFORM_POSIX 1
#  include <unistd.h>
#  include <time.h>
#  include <string.h>
#endif

static tick_t _timerlib_freq    = 0;
static double _timerlib_oofreq  = 0;


int timer_lib_initialize( void )
{
#if TIMER_PLATFORM_WINDOWS
	tick_t unused;
	if( !QueryPerformanceFrequency( (LARGE_INTEGER*)&_timerlib_freq ) ||
	    !QueryPerformanceCounter( (LARGE_INTEGER*)&unused ) )
		return -1;
#elif TIMER_PLATFORM_APPLE
	if( mach_timebase_info( &_timerlib_info ) )
		return -1;
	_timerlib_freq = 1000000000ULL;
#elif TIMER_PLATFORM_POSIX
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };
	if( clock_gettime( CLOCK_MONOTONIC, &ts ) )
		return -1;
	_timerlib_freq = 1000000000ULL;
#endif

	_timerlib_oofreq = 1.0 / (double)_timerlib_freq;

	return 0;
}


void timer_lib_shutdown( void )
{
}


tick_t timer_current( void )
{
#if TIMER_PLATFORM_WINDOWS

	tick_t curclock;
	QueryPerformanceCounter( (LARGE_INTEGER*)&curclock );
	return curclock;

#elif TIMER_PLATFORM_APPLE

	tick_t curclock = 0;
	absolutetime_to_nanoseconds( mach_absolute_time(), &curclock );
	return curclock;

#elif TIMER_PLATFORM_POSIX

	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };
	clock_gettime( CLOCK_MONOTONIC, &ts );
	return ( (uint64_t)ts.tv_sec * 1000000000ULL ) + ts.tv_nsec;

#endif
}


tick_t timer_ticks_per_second( void )
{
	return _timerlib_freq;
}


deltatime_t timer_elapsed( const tick_t t )
{
	return (deltatime_t)( (double)timer_elapsed_ticks( t ) * _timerlib_oofreq );
}


tick_t timer_elapsed_ticks( const tick_t t )
{
	tick_t dt = 0;

#if TIMER_PLATFORM_WINDOWS

	tick_t curclock = t;
	QueryPerformanceCounter( (LARGE_INTEGER*)&curclock );
	dt = curclock - t;

#elif TIMER_PLATFORM_APPLE

	tick_t curclock = t;
	absolutetime_to_nanoseconds( mach_absolute_time(), &curclock );
	dt = curclock - t;

#elif TIMER_PLATFORM_POSIX

	tick_t curclock;
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };
	clock_gettime( CLOCK_MONOTONIC, &ts );

	curclock = ( (tick_t)ts.tv_sec * 1000000000ULL ) + ts.tv_nsec;
	dt = curclock - t;

#endif

	return dt;
}


deltatime_t timer_ticks_to_seconds( const tick_t dt )
{
	return (deltatime_t)( (double)dt * _timerlib_oofreq );
}


#if TIMER_PLATFORM_WINDOWS
struct __timeb64 {
	__time64_t time;
	unsigned short millitm;
	short timezone;
	short dstflag;
	};
_CRTIMP errno_t __cdecl _ftime64_s(_Out_ struct __timeb64 * _Time);
#endif

tick_t timer_system( void )
{
#if TIMER_PLATFORM_WINDOWS

	struct __timeb64 tb;
	_ftime64_s( &tb );
	return ( (tick_t)tb.time * 1000ULL ) + (tick_t)tb.millitm;

#elif TIMER_PLATFORM_APPLE

	tick_t curclock = 0;
	absolutetime_to_nanoseconds( mach_absolute_time(), &curclock );
	return ( curclock / 1000000ULL );

#elif TIMER_PLATFORM_POSIX

	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };
	clock_gettime( CLOCK_REALTIME, &ts );
	return ( (uint64_t)ts.tv_sec * 1000ULL ) + ( ts.tv_nsec / 1000000ULL );

#endif
}
