/**
 * @file Timer.cpp
 * @author John Huston
 * @date 2013-03-28
 * @brief Implementation for global Timer object.
 * @details Timer provides a class for measuring the time
 * delta between frame renders, and provides fractional
 * scalar values for use with animation speeds to
 * accelerate or slow animations in accordance with
 * the key frame rate.
 */

#include <time.h>
#include <sys/time.h>
#include "Timer.hpp"

/* Globally provided Timer. */
Timer tick;

#define SecToNSec (1000000000)
#define SecTouSec (1000000)
#define SecToMSec (1000)

Timer::Timer( void ) {
#ifdef _RT
  clock_gettime( CLOCK_REALTIME, &_t1 );
  clock_gettime( CLOCK_REALTIME, &_t2 );
#else
  gettimeofday( &_t1, NULL );
  gettimeofday( &_t2, NULL );
#endif
  
  _keyFrameRate = (DEFAULT_KEYFRAME_RATE);
  
}

/**
 tick is an alias for tock.
 Ha, Ha, Ha.
 @return An unsigned long corresponding to how much
 time has passed since the last tick. Microseconds normally,
 Nanoseconds if _RT was enabled.
 **/
unsigned long Timer::tick( void ) {
  return this->tock();
}

/**
 tock returns the time elapsed since the last tock.
 @return An unsigned long corresponding to how much time
 has passed since the last tock. Microseconds normally,
 Nanoseconds if _RT was enabled.
 **/
unsigned long Timer::tock( void ) {
#ifdef _RT
  clock_gettime( CLOCK_REALTIME, &_t2 );
  _delta = (_t2.tv_nsec - _t1.tv_nsec) + (SecToNSec * (_t2.tv_sec - _t1.tv_sec));
  _scale = (_delta / (keyFrameRate() * 1000))
#else
  gettimeofday( &_t2, NULL );
  _delta = (_t2.tv_usec - _t1.tv_usec)
           + (SecTouSec * (_t2.tv_sec - _t1.tv_sec));
  _scale = (_delta / keyFrameRate());
#endif
  _t1 = _t2;
  return _delta;
}

/**
 delta returns the time elapsed between the last tick and the last tock.
 Does not start a new timer.
 @return Time elapsed in Microseconds, or Nanoseconds if _RT was enabled.
 **/
unsigned long Timer::delta( void ) const {
  return _delta;
}

/**
 scale returns the relative lateness or eagerness of the Timer,
 Relative to a benchmark or Key Frame Rate (The default is 60FPS,
 or 16667 msec.)
 @return A non-zero float that ranges from (0,1) indicating that
 the program is rendering faster than 60FPS, or from the range
 [1,+inf) indicating that the program is rendering slower than
 60FPS.
 **/
double Timer::scale( void ) const {
  return _scale;
}

unsigned long Timer::keyFrameRate() const {
  return _keyFrameRate;
}

unsigned long Timer::keyFrameRate( unsigned long newFrameRate ) {
  _keyFrameRate = newFrameRate;
}
