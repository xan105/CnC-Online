/* 
From: https://github.com/lanyizi/BegoneCrashers
All credits to lanyizi
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern "C" {
	void WallCrash_patch0();
	void WallCrash_patch0b();
	void WallCrash_patch1();
}