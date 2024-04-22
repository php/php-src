/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_FS_H
#define LEXBOR_FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


typedef lexbor_action_t (*lexbor_fs_dir_file_f)(const lxb_char_t *fullpath,
                                                size_t fullpath_len,
                                                const lxb_char_t *filename,
                                                size_t filename_len, void *ctx);

typedef int lexbor_fs_dir_opt_t;

enum lexbor_fs_dir_opt {
    LEXBOR_FS_DIR_OPT_UNDEF          = 0x00,
    LEXBOR_FS_DIR_OPT_WITHOUT_DIR    = 0x01,
    LEXBOR_FS_DIR_OPT_WITHOUT_FILE   = 0x02,
    LEXBOR_FS_DIR_OPT_WITHOUT_HIDDEN = 0x04,
};

typedef enum {
    LEXBOR_FS_FILE_TYPE_UNDEF            = 0x00,
    LEXBOR_FS_FILE_TYPE_FILE             = 0x01,
    LEXBOR_FS_FILE_TYPE_DIRECTORY        = 0x02,
    LEXBOR_FS_FILE_TYPE_BLOCK_DEVICE     = 0x03,
    LEXBOR_FS_FILE_TYPE_CHARACTER_DEVICE = 0x04,
    LEXBOR_FS_FILE_TYPE_PIPE             = 0x05,
    LEXBOR_FS_FILE_TYPE_SYMLINK          = 0x06,
    LEXBOR_FS_FILE_TYPE_SOCKET           = 0x07
}
lexbor_fs_file_type_t;


LXB_API lxb_status_t
lexbor_fs_dir_read(const lxb_char_t *dirpath, lexbor_fs_dir_opt_t opt,
                   lexbor_fs_dir_file_f callback, void *ctx);

LXB_API lexbor_fs_file_type_t
lexbor_fs_file_type(const lxb_char_t *full_path);

LXB_API lxb_char_t *
lexbor_fs_file_easy_read(const lxb_char_t *full_path, size_t *len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_FS_H */

