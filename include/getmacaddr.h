#ifndef GETMACADDR_H
#define GETMACADDR_H

#ifdef LINUX
void getmacaddr_lin(unsigned char addr[]);
#endif

#ifdef WIN32
void getmacaddr_win(unsigned char addr[]);
#endif

#endif 
