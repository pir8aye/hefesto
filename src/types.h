/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef HEFESTO_TYPES_H
#define HEFESTO_TYPES_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "arch.h"
#include "dbg.h"
#include "version_no.h"

#define HEFESTO_VERSION_INFO         "hefesto-"HEFESTO_VERSION

#define HEFESTO_LINUX           0
#define HEFESTO_WINDOWS         1
#define HEFESTO_FREEBSD         2
#define HEFESTO_MINIX           3
#define HEFESTO_SUNOS           4
#define HEFESTO_NETBSD          5
#define HEFESTO_OPENBSD         6

#ifndef HEFESTO_TGT_OS

#if defined(__linux__)

#define HEFESTO_TGT_OS HEFESTO_LINUX

#elif defined(__FreeBSD__)

#define HEFESTO_TGT_OS HEFESTO_FREEBSD

#elif defined(__minix__)

#define HEFESTO_TGT_OS HEFESTO_MINIX

#elif defined(__sun__)

#define HEFESTO_TGT_OS HEFESTO_SUNOS

#elif defined(__NetBSD__)

#define HEFESTO_TGT_OS HEFESTO_NETBSD

#elif defined(__OpenBSD__)

#define HEFESTO_TGT_OS HEFESTO_OPENBSD

#elif defined(_WIN32)

#define HEFESTO_TGT_OS HEFESTO_WINDOWS

#endif  // defined(X)

#endif  // HEFESTO_TGT_OS

#define HEFESTO_NULL_EVAL       "(null)"
#define HEFESTO_NULL_EVAL_SZ    18

#define HVM_ALU_ENABLE_SHORT_CIRCUIT    1

#if HEFESTO_TGT_OS == HEFESTO_LINUX   ||\
    HEFESTO_TGT_OS == HEFESTO_FREEBSD ||\
    HEFESTO_TGT_OS == HEFESTO_MINIX   ||\
    HEFESTO_TGT_OS == HEFESTO_SUNOS   ||\
    HEFESTO_TGT_OS == HEFESTO_NETBSD  ||\
    HEFESTO_TGT_OS == HEFESTO_OPENBSD

#define HEFESTO_IX_ENV 1

#endif  // HEFESTO_TGT_OS == X

#ifdef HEFESTO_IX_ENV
#define HEFESTO_PATH_SEP        '/'
#else  // HEFESTO_IX_ENV
#define HEFESTO_PATH_SEP        '\\'
#endif  // HEFESTO_IX_ENV

#define HEFESTO_MAX_BUFFER_SIZE 8192

typedef unsigned long hefesto_type_t;

#ifdef HEFESTO_ARCH_X86
typedef int32_t hefesto_int_t;
typedef uint32_t hefesto_uint_t;
#else  // HEFESTO_ARCH_X86
#ifdef HEFESTO_ARCH_X64
typedef int64_t hefesto_int_t;
typedef uint64_t hefesto_uint_t;
#else  // HEFESTO_ARCH_X64
#error "ERROR: alien cpu architecture found... or at least the assumptions inside arch.h header file must be improved."
#endif  // HEFESTO_ARCH_X64
#endif  // HEFESTO_ARCH_X86

#define HEFESTO_VAR_TYPE_UNTYPED                                0x0
#define HEFESTO_VAR_TYPE_STRING                                 0x1
#define HEFESTO_VAR_TYPE_INT                                    0x2
#define HEFESTO_VAR_TYPE_FILE_DESCRIPTOR                        0x3
#define HEFESTO_VAR_TYPE_LIST                                   0x4
#define HEFESTO_VAR_TYPE_NONE                                   0x5

typedef struct _hefesto_common_stack_ctx {
  void *data;
  size_t dsize;
  hefesto_type_t dtype;
  struct _hefesto_common_stack_ctx *next;
}hefesto_common_stack_ctx;

typedef struct _hefesto_common_list_ctx {
  void *data;
  size_t dsize;
  unsigned char is_dummy_item;
  struct _hefesto_common_list_ctx *next;
}hefesto_common_list_ctx;

typedef struct _hefesto_var_list_ctx {
  char *name;
  hefesto_type_t type, subtype;
  hefesto_common_list_ctx *contents;
  struct _hefesto_var_list_ctx *next;
}hefesto_var_list_ctx;

typedef enum _hefesto_instruction_code_t {
  HEFESTO_SYS_CALL = 0,
  HEFESTO_ATTRIB,
  HEFESTO_IF,
  HEFESTO_ELSE,
  HEFESTO_WHILE,
  HEFESTO_CALL,
  HEFESTO_LIST_METHOD,
  HEFESTO_ARRAY_INDEXING,
  HEFESTO_ARRAY_INDEXING_ATTRIB,
  HEFESTO_LIST_ARRAY_INDEXING,
  HEFESTO_RET,
  HEFESTO_UNK,
  HEFESTO_TOOLSET_COMMAND_INVOKE,
  HEFESTO_BREAK,
  HEFESTO_CONTINUE,
  HEFESTO_STRING_METHOD,
  HEFESTO_PROJECT_COMMAND_INVOKE,
  HEFESTO_INSTRUCTION_NR,
  // WARN(Santiago): Under the hood stuff... ;)
  HEFESTO_SYS_CALL_REPLACE_IN_FILE = 0x01000000,
  HEFESTO_SYS_CALL_LS = 0x02000000,
  HEFESTO_SYS_CALL_CWD = 0x03000000,
  HEFESTO_SYS_CALL_CD = 0x04000000,
  HEFESTO_SYS_CALL_RM = 0x05000000,
  HEFESTO_SYS_CALL_FD = 0x06000000,
  HEFESTO_SYS_CALL_WRITE = 0x07000000,
  HEFESTO_SYS_CALL_READ = 0x08000000,
  HEFESTO_SYS_CALL_CLOSEFD = 0x09000000,
  HEFESTO_SYS_CALL_CP = 0x0a000000,
  HEFESTO_SYS_CALL_RUN = 0x0b000000,
  HEFESTO_SYS_CALL_MKDIR = 0x0c000000,
  HEFESTO_SYS_CALL_RMDIR = 0x0d000000,
  HEFESTO_SYS_CALL_ECHO = 0x0e000000,
  HEFESTO_SYS_CALL_ENV = 0x0f000000,
  HEFESTO_SYS_CALL_FDEND = 0x10000000,
  //HEFESTO_SYS_CALL_ARRAY_COUNT = 0x11000000,
  HEFESTO_SYS_PROMPT = 0x11000000,
  HEFESTO_SYS_FSEEK = 0x12000000,
  HEFESTO_SYS_FSEEK_TO_BEGIN = 0x13000000,
  HEFESTO_SYS_FSEEK_TO_END = 0x14000000,
  HEFESTO_SYS_FSIZE = 0x15000000,
  HEFESTO_SYS_FTELL = 0x16000000,
  HEFESTO_SYS_EXIT = 0x17000000,
  HEFESTO_SYS_OS_NAME = 0x18000000,
  HEFESTO_SYS_GET_OPTION = 0x19000000,
  HEFESTO_SYS_MAKE_PATH = 0x1a000000,
  HEFESTO_SYS_LAST_FORGE_RESULT = 0x1b000000,
  HEFESTO_SYS_FORGE = 0x1c000000,
  HEFESTO_SYS_BYREF = 0x1d000000,
  HEFESTO_SYS_TIME = 0x1e000000,
  HEFESTO_SYS_SETENV = 0x1f000000,
  HEFESTO_SYS_UNSETENV = 0x20000000,
  HEFESTO_SYS_LINES_FROM_FILE = 0x21000000,
  HEFESTO_SYS_CALL_FROM_MODULE = 0x22000000,
  HEFESTO_SYS_GET_FUNC_ADDR = 0x23000000,
  HEFESTO_SYS_CALL_FUNC_ADDR = 0x24000000
}hefesto_instruction_code_t;

typedef struct _hefesto_command_list_ctx {
  hefesto_instruction_code_t instruction;
  hefesto_var_list_ctx *working_var;
  hefesto_common_list_ctx *params;
  char *expr;
  hefesto_int_t expr_logical_result;
  struct _hefesto_command_list_ctx *sub;
  //struct _hefesto_func_list_ctx *last;
  struct _hefesto_func_list_ctx *func;
  struct _hefesto_command_list_ctx *last;
  struct _hefesto_command_list_ctx *next;
}hefesto_command_list_ctx;

typedef struct _hefesto_file_descriptors_ctx { // obsoleto
  char *var_name;
  hefesto_int_t is_array;
  FILE *fp;
  struct _hefesto_file_descriptors_ctx *array;
  struct _hefesto_file_descriptors_ctx *next;
}hefesto_file_descriptors_ctx;

typedef struct _hefesto_func_list_ctx {
  char *name;
  char *decl_at;
  hefesto_int_t is_local;
  hefesto_type_t result_type;
  hefesto_var_list_ctx *args;
  hefesto_var_list_ctx *vars;
  hefesto_file_descriptors_ctx *fds;
  hefesto_command_list_ctx *code;
  void *result;
  struct _hefesto_func_list_ctx *next;
}hefesto_func_list_ctx;

typedef struct _hefesto_toolset_command_ctx {
    char *name;
    char *command;
    hefesto_common_list_ctx *a_names, *a_values;
    struct _hefesto_toolset_command_ctx *next;
}hefesto_toolset_command_ctx;

typedef struct _hefesto_toolset_ctx {
    char *name;
    hefesto_func_list_ctx *forge;
    hefesto_toolset_command_ctx *commands;
    struct _hefesto_toolset_ctx *next;
}hefesto_toolset_ctx;

extern hefesto_toolset_ctx *HEFESTO_CURRENT_TOOLSET;

typedef struct _hefesto_file_handle {
    FILE *fp;
    char path[4096];
}hefesto_file_handle;

typedef struct _hefesto_options_ctx {
    char *option;
    hefesto_common_list_ctx *data;
    struct _hefesto_options_ctx *next;
}hefesto_options_ctx;

typedef struct _hefesto_dep_chain_ctx {
    char *file_path;
    hefesto_int_t dirty;
    hefesto_common_list_ctx *deps;
    struct _hefesto_dep_chain_ctx *next;
}hefesto_dep_chain_ctx;

typedef struct _hefesto_project_ctx {
    char *name;
    hefesto_toolset_ctx *toolset;
    char *dep_chain_expr;
    char *dep_chain_str;
    hefesto_dep_chain_ctx *dep_chain;
    hefesto_common_list_ctx *args;
    hefesto_func_list_ctx *prologue, *epilogue, *preloading;
    struct _hefesto_project_ctx *next;
}hefesto_project_ctx;

typedef struct _hefesto_sum_base_ctx {
    char *file_path;
    unsigned short chsum;
    unsigned char dirty;
    struct _hefesto_sum_base_ctx *deps;
    struct _hefesto_sum_base_ctx *next;
}hefesto_sum_base_ctx;

typedef struct _hefesto_base_refresh_ctx {
    char *path;
    size_t psize;
    hefesto_int_t refresh;
    struct _hefesto_base_refresh_ctx *next;
}hefesto_base_refresh_ctx;

extern hefesto_int_t HEFESTO_EXIT_CODE;
extern hefesto_int_t HEFESTO_EXIT;
extern hefesto_int_t HEFESTO_EXIT_FORGE;
extern hefesto_dep_chain_ctx *HEFESTO_CURRENT_DEP_CHAIN;
extern hefesto_options_ctx *HEFESTO_OPTIONS;
extern hefesto_int_t HEFESTO_LAST_FORGE_RESULT;
extern hefesto_project_ctx *HEFESTO_CURRENT_PROJECT;
extern char HEFESTO_CURRENT_FORGEFILE_NAME[HEFESTO_MAX_BUFFER_SIZE];
extern char HEFESTO_CURRENT_FORGEFILE_PATH[HEFESTO_MAX_BUFFER_SIZE];

#ifdef HVM_ASYNC_RQUEUE

#if HEFESTO_TGT_OS == HEFESTO_LINUX   ||\
    HEFESTO_TGT_OS == HEFESTO_FREEBSD ||\
    HEFESTO_TGT_OS == HEFESTO_SUNOS   ||\
    HEFESTO_TGT_OS == HEFESTO_NETBSD  ||\
    HEFESTO_TGT_OS == HEFESTO_OPENBSD
#include <pthread.h>
typedef void * hefesto_thread_routine_args_t;
typedef void * hefesto_thread_routine_t;
typedef pthread_t hefesto_thread_t;
#else  // HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD || HEFESTO_TGT_OS == HEFESTO_SUNOS || HEFESTO_TGT_OS == HEFESTO_NETBSD
#include <windows.h>
typedef void * hefesto_thread_routine_args_t;
typedef void * hefesto_thread_routine_t;
typedef HANDLE hefesto_thread_t;
#endif  // HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD || HEFESTO_TGT_OS == HEFESTO_SUNOS || HEFESTO_TGT_OS == HEFESTO_NETBSD

#else  // HVM_ASYNC_RQUEUE

typedef int hefesto_thread_t;

#endif  // HVM_ASYNC_RQUEUE

typedef struct _hefesto_rqueue_ctx {
    char *path_to_run;
    hefesto_int_t exit_code;
    hefesto_int_t idle;
    size_t max_path_size;
    hefesto_thread_t id;
    struct _hefesto_rqueue_ctx *next;
}hefesto_rqueue_ctx;

#endif  // HEFESTO_TYPES_H
