<?php

/** @generate-function-entries */

function ftp_connect(string $host, int $port = 21, int $timeout = 90): FTPConnection|false {}

#ifdef HAVE_FTP_SSL
function ftp_ssl_connect(string $host, int $port = 21, int $timeout = 90): FTPConnection|false {}
#endif

function ftp_login(FTPConnection $ftp, string $username, string $password): bool {}
function ftp_pwd(FTPConnection $ftp): string|false {}
function ftp_cdup(FTPConnection $ftp): bool {}
function ftp_chdir(FTPConnection $ftp, string $directory): bool {}
function ftp_exec(FTPConnection $ftp, string $command): bool {}
function ftp_raw(FTPConnection $ftp, string $command): array {}
function ftp_mkdir(FTPConnection $ftp, string $directory): string|false {}
function ftp_rmdir(FTPConnection $ftp, string $directory): bool {}
function ftp_chmod(FTPConnection $ftp, int $mode, string $filename): int|false {}

/** @param string $response */
function ftp_alloc(FTPConnection $ftp, int $size, &$response = null): bool {}
function ftp_nlist(FTPConnection $ftp, string $directory): array|false {}
function ftp_rawlist(FTPConnection $ftp, string $directory, bool $recurse = false): array|false {}
function ftp_mlsd(FTPConnection $ftp, string $directory): array|false {}
function ftp_systype(FTPConnection $ftp): string|false {}

/** @param resource $fp */
function ftp_fget(FTPConnection $ftp, $fp, string $remote_file, int $mode = FTP_BINARY, int $resumepos = 0): bool {}

/** @param resource $fp */
function ftp_nb_fget(FTPConnection $ftp, $fp, string $remote_file, int $mode = FTP_BINARY, int $resumpos = 0): int|false {}

function ftp_pasv(FTPConnection $ftp, bool $pasv): bool {}
function ftp_get(FTPConnection $ftp, string $local_file, string $remote_file, int $mode = FTP_BINARY, int $resumepos = 0): bool {}
function ftp_nb_get(FTPConnection $ftp, string $local_file, string $remote_file, int $mode = FTP_BINARY, int $resume_pos = 0): int {}
function ftp_nb_continue(FTPConnection $ftp): int {}

/** @param resource $fp */
function ftp_fput(FTPConnection $ftp, string $remote_file, $fp, int $mode = FTP_BINARY, int $startpos = 0): bool {}

/** @param resource $fp */
function ftp_nb_fput(FTPConnection $ftp, string $remote_file, $fp, int $mode = FTP_BINARY, int $startpos = 0): int|false {}

function ftp_put(FTPConnection $ftp, string $remote_file, string $local_file, int $mode = FTP_BINARY, int $startpos = 0): bool {}
function ftp_append(FTPConnection $ftp, string $remove_file, string $local_file, int $mode = FTP_BINARY): bool {}
function ftp_nb_put(FTPConnection $ftp, string $remote_file, string $local_file, int $mode = FTP_BINARY, int $startpos = 0): int|false {}
function ftp_size(FTPConnection $ftp, string $filename): int {}
function ftp_mdtm(FTPConnection $ftp, string $filename): int {}
function ftp_rename(FTPConnection $ftp, string $src, string $dest): bool {}
function ftp_delete(FTPConnection $ftp, string $file): bool {}
function ftp_site(FTPConnection $ftp, string $cmd): bool {}
function ftp_close(FTPConnection $ftp): bool {}

/** @alias ftp_close */
function ftp_quit(FTPConnection $ftp): bool {}

function ftp_set_option(FTPConnection $ftp, int $option, int|bool $value): bool {}
function ftp_get_option(FTPConnection $ftp, int $option): int|bool {}
