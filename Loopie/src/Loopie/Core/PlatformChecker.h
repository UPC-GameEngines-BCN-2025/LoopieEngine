#if defined(_WIN32)
	#define LOOPIE_WINDOWS
#endif

#if defined(__linux__)
	#define LOOPIE_LINUX
#endif

#if defined(__APPLE__)
	#include "TargetConditionals.h"
	#if TARGET_OS_MAC
		#define LOOPIE_MACOS
	#endif
#endif
