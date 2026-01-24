#pragma once

#include "targetver.h"

#define NOMINMAX
#define _USE_MATH_DEFINES

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// TODO: reference additional headers your program requires here
#include <sys/types.h>
#include<sys/timeb.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

#include <atlcomtime.h>
#include <atlcomcli.h>

#include <iostream>
#include <locale>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <functional>
#include <deque>
#include <stdexcept>
#include <limits>
#include <cmath>
#include <cfloat>
#include <cstring>

#include "matrix.h"
#include "matrix2.h"
#include "nullable.h"
