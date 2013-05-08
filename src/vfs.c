/*
 *                              Copyright (C) 2013 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "vfs.h"
#include "mem.h"
#include "regex.h"
#include "structs_io.h"
#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#if HEFESTO_TGT_OS == HEFESTO_WINDOWS

#include <windows.h>

#endif

struct hefesto_file_ptr_pool {
    FILE *p;
    struct hefesto_file_ptr_pool *next;
};

#define new_hefesto_file_ptr_pool(f) ( (f) = (struct hefesto_file_ptr_pool *)\
                    hefesto_mloc(sizeof(struct hefesto_file_ptr_pool)),\
                    (f)->p = NULL, (f)->next = NULL )

static struct hefesto_file_ptr_pool *HEFESTO_FILE_PTR_POOL = NULL;

static int hefesto_is_dir(const char *path);

static int copy_single_file(const char *src, const char *dest);

static void add_file_ptr_to_hefesto_file_ptr_pool(FILE *p);

static void del_file_ptr_from_hefesto_file_ptr_pool(FILE *p);

static struct hefesto_file_ptr_pool *get_hefesto_file_ptr_pool_tail();

char HEFESTO_FS_CWD[HEFESTO_MAX_BUFFER_SIZE];

char HEFESTO_APP_PATH[HEFESTO_MAX_BUFFER_SIZE];

int hefesto_fs_cd(const char *dir) {

    DIR *d = opendir(dir);
    char *cwd;
    char tmp[HEFESTO_MAX_BUFFER_SIZE];

    if (d) {
        closedir(d);
        memset(tmp, 0, sizeof(tmp));
        strncpy(tmp, dir, sizeof(tmp)-1);
        if (tmp[strlen(tmp)-1] != HEFESTO_PATH_SEP) {
            tmp[strlen(tmp)] = HEFESTO_PATH_SEP;
        }
        chdir(tmp);
        cwd = hefesto_pwd();
        strncpy(HEFESTO_FS_CWD, cwd, HEFESTO_MAX_BUFFER_SIZE-1);
        free(cwd);
        if (HEFESTO_FS_CWD[strlen(HEFESTO_FS_CWD)-1] == HEFESTO_PATH_SEP)
            HEFESTO_FS_CWD[strlen(HEFESTO_FS_CWD)-1] = 0;
        return 1;
    }

    return 0;

}

char *hefesto_pwd(void) {

    char *result = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);

    result = getcwd(result, HEFESTO_MAX_BUFFER_SIZE);

    return result;

}

char *hefesto_make_path(const char *root, const char *path, size_t max_sz) {

    char *result, *r;
    const char *p;

    result = (char *) hefesto_mloc(max_sz+1);

#if HEFESTO_TGT_OS != HEFESTO_WINDOWS

    for (p = root, r = result; *p != 0; p++, r++)
        *r = *p;

    if (*(r-1) != HEFESTO_PATH_SEP) {
        *r = HEFESTO_PATH_SEP;
        r++;
    }

    p = path;

    while (*p == HEFESTO_PATH_SEP) p++;

    for (; *p != 0; p++, r++)
        *r = *p;

    *r = 0;

#else

    for (p = root, r = result; *p != 0; p++, r++)
        *r = *p;

    if (*root != 0) {
        if (*(r-1) != HEFESTO_PATH_SEP && *(r-1) != '/') {
            *r = HEFESTO_PATH_SEP;
            r++;
        }
    }

    p = path;

    if (strstr(p, ":") != NULL && (root && *root != 0)) {
       while (*p != ':') p++;
       p++;
    }

    while (*p == HEFESTO_PATH_SEP || *p == '/') p++;

    for (; *p != 0; p++, r++)
        *r = *p;

    *r = 0;

#endif

    return result;

}

static int copy_single_file(const char *src, const char *dest) {

    unsigned char buf[HEFESTO_MAX_BUFFER_SIZE];
    size_t rd;
    FILE *fs, *fd;
    struct stat f_st;

#if HFESTO_TGT_OS != HEFESTO_WINDOWS

    if (!(fs = fopen(src, "rb"))) return 0;

    if (!(fd = fopen(dest, "wb"))) {
        fclose(fs);
        return 0;
    }

    do
    {
        rd = fread(buf, 1, HEFESTO_MAX_BUFFER_SIZE, fs);
        if (rd > 0)
            fwrite(buf, 1, rd, fd);
    } while (!feof(fs));

    fclose(fs);
    fclose(fd);

    if (stat(src, &f_st) == 0) {
        chmod(dest, f_st.st_mode);
    }

#else

    CopyFile(src, dest, FALSE);

#endif

    return 1;

}

int hefesto_cp(const char *src, const char *dest) {

    char *full_src, *full_dest;
    char *file_src, *file_dest;
    DIR *dir;
    struct dirent *de;
    int result = 0;
    char *regex;

    full_src = hefesto_make_path(hefesto_is_relative_path(src) ?
                                    HEFESTO_FS_CWD : "", src,
                                 HEFESTO_MAX_BUFFER_SIZE);
    full_dest = hefesto_make_path(hefesto_is_relative_path(dest) ?
                                        HEFESTO_FS_CWD : "", dest,
                                  HEFESTO_MAX_BUFFER_SIZE);

    if (hefesto_is_file(full_src)) {
        if (strcmp(full_src, full_dest) == 0) return 1;

        result = copy_single_file(full_src, full_dest);

    } else if (hefesto_is_dir(full_src) && hefesto_is_dir(full_dest)) {
        if ((dir = opendir(full_src))) {

            result = 1;

            while ((de = readdir(dir)) && result) {

                file_src = hefesto_make_path(full_src, de->d_name,
                                          HEFESTO_MAX_BUFFER_SIZE);

                if (hefesto_is_file(file_src)) {
                    file_dest = hefesto_make_path(full_dest, de->d_name,
                                                HEFESTO_MAX_BUFFER_SIZE);
                    result = copy_single_file(file_src, file_dest);
                    free(file_dest);
                } else if (strcmp(".", de->d_name) != 0 &&
                           strcmp("..", de->d_name) != 0) {
                    file_dest = hefesto_make_path(full_dest,
                                                  de->d_name,
                                                  HEFESTO_MAX_BUFFER_SIZE);
                    if (!hefesto_is_dir(file_dest)) {
                        result = (hefesto_mkdir(file_dest +
                                          strlen(HEFESTO_FS_CWD)) == 0);
                    }
                    if (result) {
                        result = hefesto_cp(file_src, file_dest);
                    }
                    free(file_dest);
                }

                free(file_src);

            }

            closedir(dir);

        }
    } else if (hefesto_is_dir(full_dest)) {
        regex = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
        preprocess_usr_regex((unsigned char *)regex, (unsigned char *) src,
                              HEFESTO_MAX_BUFFER_SIZE, strlen(src));

        if ((dir = opendir(HEFESTO_FS_CWD))) {

            result = 1;

            while ((de = readdir(dir)) && result) {

                file_src = hefesto_make_path(HEFESTO_FS_CWD, 
                                   de->d_name, HEFESTO_MAX_BUFFER_SIZE);

                if (bool_match_regex((unsigned char *)file_src,
                                     (unsigned char *)file_src,
                                     (unsigned char *)file_src +
                                        strlen(file_src),
                                     (unsigned char *)regex,
                                     (unsigned char *)regex,
                                     (unsigned char *)regex +
                                        strlen(regex), 1)) {
                    if (hefesto_is_file(file_src)) {
                        file_dest = hefesto_make_path(full_dest, de->d_name,
                                                      HEFESTO_MAX_BUFFER_SIZE);
                        result = copy_single_file(file_src, file_dest);
                        free(file_dest);
                    } else if (strcmp(".", de->d_name) != 0 &&
                               strcmp("..", de->d_name) != 0) {
                        file_dest = hefesto_make_path(full_dest,
                                                      de->d_name,
                                                      HEFESTO_MAX_BUFFER_SIZE);
                        if (!hefesto_is_dir(file_dest))
                            result = (hefesto_mkdir(file_dest +
                                                  strlen(HEFESTO_FS_CWD)) == 0);
                        if (result) {
                            result = hefesto_cp(file_src, file_dest);
                        }
                        free(file_dest);
                    }

                }

                free(file_src);

            }

        }

        free(regex);

    }

    free(full_src);
    free(full_dest);

    return result;

}

int hefesto_rm(const char *src) {

    char *full_src;
    int result = 0;

    if (src == NULL) return 0;

    full_src = hefesto_make_path(hefesto_is_relative_path(src) ?
                                   HEFESTO_FS_CWD : "", src,
                                 HEFESTO_MAX_BUFFER_SIZE);
    result = remove(full_src);

    free(full_src);

    return result;

}

int hefesto_mkdir(const char *dir) {

    char *full_src;
    int result = 0;

    if (dir == NULL) return 0;

    full_src = hefesto_make_path(HEFESTO_FS_CWD, dir, HEFESTO_MAX_BUFFER_SIZE);

#if HEFESTO_TGT_OS !=  HEFESTO_WINDOWS
    result = mkdir(full_src, 0);
#else
    result = mkdir(full_src);
#endif

    free(full_src);

    return result;

}

int hefesto_rmdir(const char *dir) {

    char *full_src;
    int result = 0;

    if (dir == NULL) return 0;

    full_src = hefesto_make_path(hefesto_is_relative_path(dir) ?
                                    HEFESTO_FS_CWD : "",
                                 dir, HEFESTO_MAX_BUFFER_SIZE);
    result = rmdir(full_src);

    free(full_src);

    return result;

}

int hefesto_is_file(const char *path) {

    DIR *dir = opendir(path);
    FILE *fp;

    if (dir != NULL) {
        closedir(dir);
        return 0;
    }

    fp = fopen(path, "rb");

    if (fp != NULL) {
        fclose(fp);
        return 1;
    }

    return 0;

}

static int hefesto_is_dir(const char *path) {

    DIR *dir = opendir(path);

    if (dir) {
        closedir(dir);
        return 1;
    }

    return 0;

}

int hefesto_ls(const char *mask) {

    DIR *dir;
    char *cwd = hefesto_pwd();
    struct dirent *de;
    int total = 0;
    char regex[HEFESTO_MAX_BUFFER_SIZE];
    char *full_path;

    if (!(dir = opendir(cwd))) {
        free(cwd);
        return 0;
    }

    preprocess_usr_regex((unsigned char *)regex, (unsigned char *) mask,
                         HEFESTO_MAX_BUFFER_SIZE, strlen(mask) + 1);

    while ((de = readdir(dir))) {
        if (strcmp(de->d_name, ".") == 0 ||
            strcmp(de->d_name, "..") == 0) continue;
        full_path = hefesto_make_path(cwd, de->d_name, HEFESTO_MAX_BUFFER_SIZE);
        if (bool_match_regex((unsigned char *)full_path,
                             (unsigned char *)full_path,
                             (unsigned char *)full_path + strlen(full_path),
                             (unsigned char *)regex,
                             (unsigned char *)regex,
                             (unsigned char *)regex + strlen(regex), 1)) {
            total++;
        }
        free(full_path);
    }

    free(cwd);

    closedir(dir);

    return total;

}

hefesto_file_handle *hefesto_fopen(const char *file_path, const char *mode) {

    hefesto_file_handle *fp_handle;
    char temp_mode[20];
    const char *m;
    size_t t;

    strncpy(temp_mode, mode, sizeof(temp_mode)-1);

    if (strstr(mode, "+") == NULL) {
        if ((temp_mode[0] == 'w' || temp_mode[0] == 'r') && temp_mode[1] == 0) {
            temp_mode[1] = 'b';
            temp_mode[2] = 0;
        }
    } else {
        t = 0;
        for (m = mode; *m != 0 && t < sizeof(temp_mode)-1; m++) {
            if (*m != 'b' && *m != '+') {
                temp_mode[t++] = *m;
            } else {
                temp_mode[t++] = 'b';
                if (*m == '+') temp_mode[t++] = *m;
            }
        }
    }

    HEFESTO_DEBUG_INFO(0, "vfs/temp_mode: %s\n", temp_mode);
    new_hefesto_file_handle(fp_handle, file_path);
    fp_handle->fp = fopen(fp_handle->path, temp_mode);

    if (fp_handle->fp != NULL) {
        add_file_ptr_to_hefesto_file_ptr_pool(fp_handle->fp);
    }

    return fp_handle;

}

void hefesto_fclose(hefesto_file_handle **fp_handle) {

    if (fp_handle != NULL && (*fp_handle) != NULL && (*fp_handle)->fp != NULL) {
        del_file_ptr_from_hefesto_file_ptr_pool((*fp_handle)->fp);
        fclose((*fp_handle)->fp);
        (*fp_handle)->fp = NULL;
    }

}

int hefesto_fwrite(const char *buffer, const size_t size,
                   hefesto_file_handle *fp_handle) {
    HEFESTO_DEBUG_INFO(0,
        "vfs/buffer: %s, size: %d, fp_handle: %x\n", buffer,
            size, fp_handle->fp);

    if (fp_handle == NULL || fp_handle->fp == NULL) return -1;

    return fwrite(buffer, 1, size, fp_handle->fp);

}

int hefesto_fread(char *buffer, const size_t size,
                  hefesto_file_handle *fp_handle) {

    HEFESTO_DEBUG_INFO(0,
        "vfs/buffer: size: %d, fp_handle: %x\n",
            size, fp_handle->fp);

    memset(buffer, 0, size);

    if (fp_handle == NULL || fp_handle->fp == NULL) return -1;

    return fread(buffer, 1, size, fp_handle->fp);

}

hefesto_file_handle *get_file_descriptor_by_var_name(const char *var_name,
                                            hefesto_var_list_ctx *lo_vars,
                                            hefesto_var_list_ctx *gl_vars,
                                            hefesto_func_list_ctx *functions) {
    hefesto_var_list_ctx *vp;
    char *vname, *v, *vn, *expr;

    if (strstr(var_name, "[") == NULL) {
        vname = (char *) var_name;
    } else {
        vname = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
        for (vn = (char *) vname, v = vname; *vn != 0 && *vn != '['; vn++, v++)
            *vn = *v;
        *vn = 0;
        for (; *v != 0 && is_hefesto_blank(*v); v++);
        expr = (char *) hefesto_mloc(HEFESTO_MAX_BUFFER_SIZE);
        for (vn = expr; *v != 0; v++, vn++)
            *vn = *v;
        *vn = 0;
    }
    vp = get_hefesto_var_list_ctx_name(vname, lo_vars);
    if (!vp) vp = get_hefesto_var_list_ctx_name(vname, gl_vars);
    HEFESTO_DEBUG_INFO(0, "vfs/vp = %x\n", vp);
    if (vname != var_name) free(vname);
    if (vp && vp->type == HEFESTO_VAR_TYPE_FILE_DESCRIPTOR)
        return (hefesto_file_handle *) vp->contents->data;
    HEFESTO_DEBUG_INFO(0, "vfs/retornei nulo!\n");

    return NULL;

}

int hefesto_feof(hefesto_file_handle *fp_handle) {

    if (fp_handle == NULL || fp_handle->fp == NULL) return 1;

    return (feof(fp_handle->fp) != 0);

}

int hefesto_fseek(hefesto_file_handle *fp_handle, const long offset) {

    if (fp_handle == NULL || fp_handle->fp == NULL) return -1;

    return fseek(fp_handle->fp, offset, SEEK_SET);

}

int hefesto_fseek_to_begin(hefesto_file_handle *fp_handle) {

    if (fp_handle == NULL || fp_handle->fp == NULL) return -1;

    return fseek(fp_handle->fp, 0, SEEK_SET);

}

int hefesto_fseek_to_end(hefesto_file_handle *fp_handle) {

    if (fp_handle == NULL || fp_handle->fp == NULL) return -1;

    return fseek(fp_handle->fp, 0, SEEK_END);

}

int hefesto_fsize(hefesto_file_handle *fp_handle) {

    long temp;
    int fsz;

    if (fp_handle == NULL || fp_handle->fp == NULL) return -1;

    temp = ftell(fp_handle->fp);
    fseek(fp_handle->fp, 0, SEEK_END);
    fsz = ftell(fp_handle->fp);
    fseek(fp_handle->fp, temp, SEEK_SET);

    return fsz;

}

int hefesto_ftell(hefesto_file_handle *fp_handle) {

    if (fp_handle == NULL || fp_handle->fp == NULL) return -1;

    return ftell(fp_handle->fp);

}

void set_hefesto_app_directory(const char *directory) {

    memset(HEFESTO_APP_PATH, 0, sizeof(HEFESTO_APP_PATH));

    strncpy(HEFESTO_APP_PATH, directory, sizeof(HEFESTO_APP_PATH)-1);
}

int hefesto_is_relative_path(const char *path) {

    int is_relative = 0;
#if HEFESTO_TGT_OS == HEFESTO_LINUX || HEFESTO_TGT_OS == HEFESTO_FREEBSD
    char *cwd;
    char *p, temp[HEFESTO_MAX_BUFFER_SIZE * 2];

    if (path && *path == HEFESTO_PATH_SEP) return 0;

    cwd = hefesto_pwd();

    for (p = (char *) path; *p != 0; p++);
    for (p--; *p != HEFESTO_PATH_SEP && p != path; p--);
    memset(temp, 0, sizeof(temp));
    memcpy(temp, path, (p != path) ? (p - path) : strlen(path));
    for (p = cwd; *p != 0; p++);
    p--;
    if (*p == HEFESTO_PATH_SEP) *p = 0;
    is_relative = (strcmp(temp, cwd) != 0);
    free(cwd);
#elif HEFESTO_TGT_OS == HEFESTO_WINDOWS
    char *cwd;
    char *p, temp[HEFESTO_MAX_BUFFER_SIZE * 2];

    cwd = hefesto_pwd();

    for (p = cwd; *p != 0; p++) {
        if (isalpha(*p)) *p = toupper(*p);
    }

    if (strstr(path, ":") != NULL || strstr(path, "\\\\") != NULL) return 0;

    for (p = (char *) path; *p != 0; p++);
    for (p--; *p != HEFESTO_PATH_SEP && *p != '/' && p != path; p--);
    memset(temp, 0, sizeof(temp));
    memcpy(temp, path, (p != path) ? (p - path) : strlen(path));
    for (p = cwd; *p != 0; p++);
    p--;
    if (*p == HEFESTO_PATH_SEP || *p == '/') *p = 0;
    for (p = cwd; *p != 0; p++) {
        if (isalpha(*p)) *p = toupper(*p);
    }
    is_relative = (strcmp(temp, cwd) != 0);
    free(cwd);
#endif

    return is_relative;

}

static void add_file_ptr_to_hefesto_file_ptr_pool(FILE *p) {
    struct hefesto_file_ptr_pool *np;
    if (HEFESTO_FILE_PTR_POOL == NULL) {
        new_hefesto_file_ptr_pool(HEFESTO_FILE_PTR_POOL);
        np = HEFESTO_FILE_PTR_POOL;
    } else {
        np = get_hefesto_file_ptr_pool_tail();
        new_hefesto_file_ptr_pool(np->next);
        np = np->next;
    }
    np->p = p;
}

static void del_file_ptr_from_hefesto_file_ptr_pool(FILE *p) {
    struct hefesto_file_ptr_pool *np, *l_np;
    for (np = l_np = HEFESTO_FILE_PTR_POOL; np; l_np = np, np = np->next) {
        if (np->p == p) {
            if (np == HEFESTO_FILE_PTR_POOL) {
                HEFESTO_FILE_PTR_POOL = HEFESTO_FILE_PTR_POOL->next;
            } if (np->next == NULL) {
                l_np->next = NULL;
            } else {
                l_np->next = np->next;
            }
            free(np);
            break;
        }
    }
}

static struct hefesto_file_ptr_pool *get_hefesto_file_ptr_pool_tail() {
    struct hefesto_file_ptr_pool *np;
    for (np = HEFESTO_FILE_PTR_POOL; np->next; np = np->next);
    return np;
}

void clear_hefesto_file_ptr_pool() {
    struct hefesto_file_ptr_pool *np, *t;
    for (np = t = HEFESTO_FILE_PTR_POOL; t; np = t) {
        t = np->next;
        if (np->p != NULL) fclose(np->p);
        free(np);
    }
    HEFESTO_FILE_PTR_POOL = NULL;
}