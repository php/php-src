<?php

/** @generate-function-entries */

function ftp_connect(string $hostname, int $port = 21, int $timeout = 90): FTPConnection|false {}

#ifdef HAVE_FTP_SSL
function ftp_ssl_connect(string $hostname, int $port = 21, int $timeout = 90): FTPConnection|false {}
#endif

function ftp_login(FTPConnection $ftp, string $username, string $password): bool {}
function ftp_pwd(FTPConnection $ftp): string|false {}
function ftp_cdup(FTPConnection $ftp): bool {}
function ftp_chdir(FTPConnection $ftp, string $directory): bool {}
function ftp_exec(FTPConnection $ftp, string $command): bool {}
function ftp_raw(FTPConnection $ftp, string $command): array {}
function ftp_mkdir(FTPConnection $ftp, string $directory): string|false {}
function ftp_rmdir(FTPConnection $ftp, string $directory): bool {}
function ftp_chmod(FTPConnection $ftp, int $permissions, string $filename): int|false {}

/** @param string $response */
function ftp_alloc(FTPConnection $ftp, int $size, &$response = null): bool {}
function ftp_nlist(FTPConnection $ftp, string $directory): array|false {}
function ftp_rawlist(FTPConnection $ftp, string $directory, bool $recursive = false): array|false {}
function ftp_mlsd(FTPConnection $ftp, string $directory): array|false {}
function ftp_systype(FTPConnection $ftp): string|false {}

/** @param resource $stream */
function ftp_fget(FTPConnection $ftp, $stream, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}

/** @param resource $stream */
function ftp_nb_fget(FTPConnection $ftp, $stream, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): int {}
function ftp_pasv(FTPConnection $ftp, bool $enable): bool {}
function ftp_get(FTPConnection $ftp, string $local_filename, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}
function ftp_nb_get(FTPConnection $ftp, string $local_filename, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): int {}
function ftp_nb_continue(FTPConnection $ftp): int {}

/** @param resource $stream */
function ftp_fput(FTPConnection $ftp, string $remote_filename, $stream, int $mode = FTP_BINARY, int $offset = 0): bool {}

/** @param resource $stream */
function ftp_nb_fput(FTPConnection $ftp, string $remote_filename, $stream, int $mode = FTP_BINARY, int $offset = 0): int {}
function ftp_put(FTPConnection $ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}
function ftp_append(FTPConnection $ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY): bool {}
function ftp_nb_put(FTPConnection $ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY, int $offset = 0): int|false {}
function ftp_size(FTPConnection $ftp, string $filename): int {}
function ftp_mdtm(FTPConnection $ftp, string $filename): int {}
function ftp_rename(FTPConnection $ftp, string $from, string $to): bool {}
function ftp_delete(FTPConnection $ftp, string $filename): bool {}
function ftp_site(FTPConnection $ftp, string $command): bool {}
function ftp_close(FTPConnection $ftp): bool {}

/** @alias ftp_close */
function ftp_quit(FTPConnection $ftp): bool {}

/** @param int|bool $value */
function ftp_set_option(FTPConnection $ftp, int $option, $value): bool {}
function ftp_get_option(FTPConnection $ftp, int $option): int|bool {}
