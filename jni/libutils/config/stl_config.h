#ifndef __STL_CONFIG_H__
#define __STL_CONFIG_H__

#if defined(PLATFORM_ANDROID)
#include <ustring.h>
#include <ulist.h>
#include <umap.h>
#include <uctralgo.h>
#include <ctype.h>
using namespace ustl;

//#elif defined (PLATFORM_MIPS)
#elif defined (__GNUC__)
#include <string>
#include <deque>
#include <list>
#include <map>
using namespace std;
#else
//other unknown platform.
#include <string>
#include <deque>
#include <list>
#include <map>
using namespace std;
#endif

#endif /// __UTILS_H
