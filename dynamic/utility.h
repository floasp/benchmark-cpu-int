#ifndef UTILITY_H
#define UTILITY_H

#ifdef _WIN32
#include <windows.h>
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif

int getNumberOfCores();
void my_sleep();


#endif /* UTILITY_H */