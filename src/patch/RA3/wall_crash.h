/* 
From: https://github.com/lanyizi/BegoneCrashers
All credits to lanyizi
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern "C" {
	void WallCrash_patch_retail();
	void WallCrash_patch_digital();
	void WallCrash_patch_common();
}