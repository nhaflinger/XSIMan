// zlib_timeops.C - time calculation utility, use to expire applications

// The type time_t
//     include: <time.h>
//  Is exactly identical to "int" in behaviour. You may assign int values
//  to a time_t variable, and vice-versa, and print time_t values using
//  the %d format, and everything.
//  In the future, either time_t will cease to be identical to int, or
//  the meaning of "int" will change from 4-byte-integer to 8-byte-integer,
//  otherwise unix will have its own "Y2K" problem one second after 10:14:07pm,
//  Monday 18th January 2038, which will appear to be 3:45:52pm on Friday
//  13th December 1901.
//        time_t values are produced from the clock by time.
//        time_t values are produced from y,m,d,h,m,s parts by mktime and timegm.
//        time_t values are analysed into y,m,d,h,m,s by localtime and gmtime.
//        time_t values are converted to readable strings by ctime.

#ifdef WIN32
# include "stdafx.h"
#endif

#include <time.h>
#include <iostream>

void usage()
{
  std::cerr << 
      "timeops curtime_n | curtime_s | future_n n | future_s n | whattime_n\n";
  std::cerr << "_n gives numeric time in seconds since 1970\n";
  std::cerr << "_s gives string (readable) time\n";
  exit(1);
}


int main(int ac, char *av[])
{
  if (ac < 2) usage();

  if (strcmp(av[1], "curtime_n")==0) {
    time_t curtime;
    time(&curtime);
    std::cout << curtime << std::endl;
  }

  else if (strcmp(av[1], "curtime_s")==0) {
    time_t curtime;
    time(&curtime);
    std::cout << ctime(&curtime);
  }

  else if (strcmp(av[1], "future_n")==0) {
    time_t curtime;
    time_t add = atoi(av[2]);
    time(&curtime);
    curtime += add;
    std::cout << curtime << std::endl;
  }

  else if (strcmp(av[1], "future_s")==0) {
    time_t curtime;
    time_t add = atoi(av[2]);
    time(&curtime);
    curtime += add;
    std::cout << ctime(&curtime);
  }

  else if (strcmp(av[1], "whattime_s")==0) {
    time_t curtime = atoi(av[2]);
    std::cout << ctime(&curtime);
  }

  else {
    std::cout << "unrecognized command: " << av[1] << std::endl;
    usage();
  }

  exit(0);
} //main

