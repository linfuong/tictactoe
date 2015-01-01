// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
 	

#include<windowsx.h>
// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
#include <time.h>
#include "brain.h"
// матрица симметрий
#define IN_PROGRESS 0
#define COMPUTER_WIN 1
#define USER_WIN 2
#define PARITY 3

#define RES_WIN 1
#define RES_PARITY 0
#define RES_LOOSE 2
