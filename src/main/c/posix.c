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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>

JNIEXPORT void JNICALL
Java_net_rubygrapefruit_platform_internal_jni_NativeLibraryFunctions_getSystemInfo(JNIEnv *env, jclass target, jobject info, jobject result) {
    jclass infoClass = (*env)->GetObjectClass(env, info);

    struct utsname machine_info;
    if (uname(&machine_info) != 0) {
        mark_failed_with_errno(env, "could not query machine details", result);
        return;
    }

    jfieldID osNameField = (*env)->GetFieldID(env, infoClass, "osName", "Ljava/lang/String;");
    (*env)->SetObjectField(env, info, osNameField, char_to_java(env, machine_info.sysname, result));
    jfieldID osVersionField = (*env)->GetFieldID(env, infoClass, "osVersion", "Ljava/lang/String;");
    (*env)->SetObjectField(env, info, osVersionField, char_to_java(env, machine_info.release, result));
    jfieldID machineArchitectureField = (*env)->GetFieldID(env, infoClass, "machineArchitecture", "Ljava/lang/String;");
    (*env)->SetObjectField(env, info, machineArchitectureField, char_to_java(env, machine_info.machine, result));
}

JNIEXPORT void JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixTypeFunctions_getNativeTypeInfo(JNIEnv *env, jclass target, jobject info) {
    jclass infoClass = (*env)->GetObjectClass(env, info);
    (*env)->SetIntField(env, info, (*env)->GetFieldID(env, infoClass, "int_bytes", "I"), sizeof(int));
    (*env)->SetIntField(env, info, (*env)->GetFieldID(env, infoClass, "u_long_bytes", "I"), sizeof(u_long));
    (*env)->SetIntField(env, info, (*env)->GetFieldID(env, infoClass, "size_t_bytes", "I"), sizeof(size_t));
    (*env)->SetIntField(env, info, (*env)->GetFieldID(env, infoClass, "uid_t_bytes", "I"), sizeof(uid_t));
    (*env)->SetIntField(env, info, (*env)->GetFieldID(env, infoClass, "gid_t_bytes", "I"), sizeof(gid_t));
    (*env)->SetIntField(env, info, (*env)->GetFieldID(env, infoClass, "off_t_bytes", "I"), sizeof(off_t));
}

/*
 * File functions
 */

JNIEXPORT void JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixFileFunctions_chmod(JNIEnv *env, jclass target, jstring path, jint mode, jobject result) {
    char* pathStr = java_to_char(env, path, result);
    if (pathStr == NULL) {
        return;
    }
    int retval = chmod(pathStr, mode);
    free(pathStr);
    if (retval != 0) {
        mark_failed_with_errno(env, "could not chmod file", result);
    }
}

jlong timestamp(struct timespec t) {
    return t.tv_sec * 1000 + t.tv_nsec / 1000;
}

JNIEXPORT void JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixFileFunctions_stat(JNIEnv *env, jclass target, jstring path, jobject dest, jobject result) {
    struct stat fileInfo;
    char* pathStr = java_to_char(env, path, result);
    if (pathStr == NULL) {
        return;
    }
    int retval = lstat(pathStr, &fileInfo);
    free(pathStr);
    if (retval != 0 && errno != ENOENT) {
        mark_failed_with_errno(env, "could not stat file", result);
        return;
    }

    jclass destClass = (*env)->GetObjectClass(env, dest);
    jfieldID modeField = (*env)->GetFieldID(env, destClass, "mode", "I");
    jfieldID typeField = (*env)->GetFieldID(env, destClass, "type", "I");
    jfieldID sizeField = (*env)->GetFieldID(env, destClass, "size", "J");
    jfieldID uidField = (*env)->GetFieldID(env, destClass, "uid", "I");
    jfieldID gidField = (*env)->GetFieldID(env, destClass, "gid", "I");
    jfieldID blockSizeField = (*env)->GetFieldID(env, destClass, "blockSize", "J");
    jfieldID accessTimeField = (*env)->GetFieldID(env, destClass, "accessTime", "J");
    jfieldID modificationTimeField = (*env)->GetFieldID(env, destClass, "modificationTime", "J");
    jfieldID statusChangeTime = (*env)->GetFieldID(env, destClass, "statusChangeTime", "J");

    if (retval != 0) {
        (*env)->SetIntField(env, dest, typeField, FILE_TYPE_MISSING);
    } else {
        (*env)->SetIntField(env, dest, modeField, 0777 & fileInfo.st_mode);
        int type;
        switch (fileInfo.st_mode & S_IFMT) {
            case S_IFREG:
                type = FILE_TYPE_FILE;
                break;
            case S_IFDIR:
                type = FILE_TYPE_DIRECTORY;
                break;
            case S_IFLNK:
                type = FILE_TYPE_SYMLINK;
                break;
            default:
                type= FILE_TYPE_OTHER;
        }
        (*env)->SetIntField(env, dest, typeField, type);
        (*env)->SetIntField(env, dest, uidField, fileInfo.st_uid);
        (*env)->SetIntField(env, dest, gidField, fileInfo.st_gid);
        (*env)->SetLongField(env, dest, sizeField, fileInfo.st_size);
        (*env)->SetLongField(env, dest, blockSizeField, fileInfo.st_blksize);
#ifdef __linux__
        (*env)->SetLongField(env, dest, accessTimeField, fileInfo.st_atime * 1000);
        (*env)->SetLongField(env, dest, modificationTimeField, fileInfo.st_mtime * 1000);
        (*env)->SetLongField(env, dest, statusChangeTime, fileInfo.st_ctime * 1000);
#else
        (*env)->SetLongField(env, dest, accessTimeField, timestamp(fileInfo.st_atimespec));
        (*env)->SetLongField(env, dest, modificationTimeField, timestamp(fileInfo.st_mtimespec));
        (*env)->SetLongField(env, dest, statusChangeTime, timestamp(fileInfo.st_ctimespec));
#endif
    }
}

JNIEXPORT void JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixFileFunctions_symlink(JNIEnv *env, jclass target, jstring path, jstring contents, jobject result) {
    char* pathStr = java_to_char(env, path, result);
    if (pathStr == NULL) {
        return;
    }
    char* contentStr = java_to_char(env, contents, result);
    if (contentStr == NULL) {
        free(pathStr);
        return;
    }
    int retval = symlink(contentStr, pathStr);
    free(contentStr);
    free(pathStr);
    if (retval != 0) {
        mark_failed_with_errno(env, "could not symlink", result);
    }
}

JNIEXPORT jstring JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixFileFunctions_readlink(JNIEnv *env, jclass target, jstring path, jobject result) {
    struct stat link_info;
    char* pathStr = java_to_char(env, path, result);
    if (pathStr == NULL) {
        return NULL;
    }
    int retval = lstat(pathStr, &link_info);
    if (retval != 0) {
        free(pathStr);
        mark_failed_with_errno(env, "could not lstat file", result);
        return NULL;
    }

    char* contents = (char*)malloc(link_info.st_size + 1);
    if (contents == NULL) {
        free(pathStr);
        mark_failed_with_message(env, "could not create array", result);
        return NULL;
    }

    retval = readlink(pathStr, contents, link_info.st_size);
    free(pathStr);
    if (retval < 0) {
        free(contents);
        mark_failed_with_errno(env, "could not readlink", result);
        return NULL;
    }
    contents[link_info.st_size] = 0;
    jstring contents_str = char_to_java(env, contents, result);
    free(contents);
    return contents_str;
}

/*
 * Process functions
 */

JNIEXPORT jint JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixProcessFunctions_getPid(JNIEnv *env, jclass target) {
    return getpid();
}

JNIEXPORT jstring JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixProcessFunctions_getWorkingDirectory(JNIEnv *env, jclass target, jobject result) {
    char* path = getcwd(NULL, 0);
    if (path == NULL) {
        mark_failed_with_errno(env, "could not getcwd()", result);
        return NULL;
    }
    jstring dir = char_to_java(env, path, result);
    free(path);
    return dir;
}

JNIEXPORT void JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixProcessFunctions_setWorkingDirectory(JNIEnv *env, jclass target, jstring dir, jobject result) {
    char* path = java_to_char(env, dir, result);
    if (path == NULL) {
        return;
    }
    if (chdir(path) != 0) {
        mark_failed_with_errno(env, "could not setcwd()", result);
    }
    free(path);
}

JNIEXPORT jstring JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixProcessFunctions_getEnvironmentVariable(JNIEnv *env, jclass target, jstring var, jobject result) {
    char* varStr = java_to_char(env, var, result);
    char* valueStr = getenv(varStr);
    free(varStr);
    if (valueStr == NULL) {
        return NULL;
    }
    return char_to_java(env, valueStr, result);
}

JNIEXPORT void JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixProcessFunctions_setEnvironmentVariable(JNIEnv *env, jclass target, jstring var, jstring value, jobject result) {
    char* varStr = java_to_char(env, var, result);
    if (value == NULL) {
        if (setenv(varStr, "", 1) != 0) {
            mark_failed_with_errno(env, "could not putenv()", result);
        }
    } else {
        char* valueStr = java_to_char(env, value, result);
        if (setenv(varStr, valueStr, 1) != 0) {
            mark_failed_with_errno(env, "could not putenv()", result);
        }
        free(valueStr);
    }
    free(varStr);
}

/*
 * Terminal functions
 */

JNIEXPORT jboolean JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixTerminalFunctions_isatty(JNIEnv *env, jclass target, jint output) {
    struct stat fileInfo;
    int result;
    switch (output) {
    case 0:
    case 1:
        return isatty(output+1) ? JNI_TRUE : JNI_FALSE;
    default:
        return JNI_FALSE;
    }
}

JNIEXPORT void JNICALL
Java_net_rubygrapefruit_platform_internal_jni_PosixTerminalFunctions_getTerminalSize(JNIEnv *env, jclass target, jint output, jobject dimension, jobject result) {
    struct winsize screen_size;
    int retval = ioctl(output+1, TIOCGWINSZ, &screen_size);
    if (retval != 0) {
        mark_failed_with_errno(env, "could not fetch terminal size", result);
        return;
    }
    jclass dimensionClass = (*env)->GetObjectClass(env, dimension);
    jfieldID widthField = (*env)->GetFieldID(env, dimensionClass, "cols", "I");
    (*env)->SetIntField(env, dimension, widthField, screen_size.ws_col);
    jfieldID heightField = (*env)->GetFieldID(env, dimensionClass, "rows", "I");
    (*env)->SetIntField(env, dimension, heightField, screen_size.ws_row);
}

#endif
