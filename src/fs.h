#pragma once

// #include <unistd.h> // for chdir
// #include <libgen.h> // for dirname
// #include <mach-o/dyld.h> // for _NSGetExecutablePath
// #include <limits.h> // for PATH_MAX?

#include <string.h>
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// char directory_separator[] = "/";
// #ifdef WIN32
// directory_separator[0] = '\\';
// #endif

#ifdef __APPLE__

// this will fill the input variable with a path, something like
// /Users/Maya/Applications/My-App.app/Contents/Resources
// (with no trailing /)
void getMacBundleResourcesPath(char *resourcePath);

#endif

// filename should be something like "simple.frag"
// or, "shaders/simple.frag" if you intend there to be a "shaders"
// directory inside of the bundle

#ifdef __cplusplus
}
#endif
