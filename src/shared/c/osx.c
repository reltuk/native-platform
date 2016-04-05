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
#ifdef __APPLE__

#include "native.h"
#include "generic.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

char* java_to_char(JNIEnv *env, jstring string, jobject result) {
    size_t len = (*env)->GetStringLength(env, string);
    size_t bytes = (*env)->GetStringUTFLength(env, string);
    char* chars = (char*)malloc(bytes + 1);
    (*env)->GetStringUTFRegion(env, string, 0, len, chars);
    chars[bytes] = 0;
    return chars;
}

jstring char_to_java(JNIEnv* env, const char* chars, jobject result) {
    return (*env)->NewStringUTF(env, chars);
}

#endif
