/*
 * Copyright 2012 Adam Murdoch
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/*
 * POSIX platform functions.
 */
#ifndef _WIN32

#include "native.h"
#include "generic.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

void mark_failed_with_errno(JNIEnv *env, const char* message, jobject result) {
    char* buffer = (char*)malloc(1024);
#if defined(__linux__) && _GNU_SOURCE
    // GNU semantics
    char* errno_message = strerror_r(errno, buffer, 1024);
#else
    strerror_r(errno, buffer, 1024);
    char* errno_message = buffer;
#endif
    mark_failed_with_code(env, message, errno, errno_message, result);
    free(buffer);
}

#endif
