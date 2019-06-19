// int t = zlib_timeops::expire(EXPTIME);
// if (t < 0) expired;
// if (t < (60*60*24*30)) 
//    std::cout << "warning: will expire on zlib_timeops::ctime(EXPTIME)

#ifndef ZLIB_TIMEOPS
#define ZLIB_TIMEOPS

#include <time.h>

class zlib_timeops
{
 public:

  static int expire(time_t exptime)
  {
    time_t curtime;
    time(&curtime);
    if (curtime > exptime) return -1;
    return (int)((long)exptime - (long)curtime);
  }

  static char *ctime(time_t t) 
  {
    return ::ctime(&t);
  }

};

#endif

