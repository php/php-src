<?php

/** @generate-function-entries */

/** @return resource|false */
function ftp_connect(string $hostname, int $port = 21, int $timeout = 90) {}

#ifdef HAVE_FTP_SSL
/** @return resource|false */
function ftp_ssl_connect(string $hostname, int $port = 21, int $timeout = 90) {}
#endif

/** @param resource $ftp */
function ftp_login($ftp, string $username, string $password): bool {}

/** @param resource $ftp */
function ftp_pwd($ftp): string|false {}

/** @param resource $ftp */
function ftp_cdup($ftp): bool {}

/** @param resource $ftp */
function ftp_chdir($ftp, string $directory): bool {}

/** @param resource $ftp */
function ftp_exec($ftp, string $command): bool {}

/** @param resource $ftp */
function ftp_raw($ftp, string $command): ?array {}

/** @param resource $ftp */
function ftp_mkdir($ftp, string $directory): string|false {}

/** @param resource $ftp */
function ftp_rmdir($ftp, string $directory): bool {}

/** @param resource $ftp */
function ftp_chmod($ftp, int $permissions, string $filename): int|false {}

/**
 * @param resource $ftp
 * @param string $response
 */
function ftp_alloc($ftp, int $size, &$response = null): bool {}

/** @param resource $ftp */
function ftp_nlist($ftp, string $directory): array|false {}

/** @param resource $ftp */
function ftp_rawlist($ftp, string $directory, bool $recursive = false): array|false {}

/** @param resource $ftp */
function ftp_mlsd($ftp, string $directory): array|false {}

/** @param resource $ftp */
function ftp_systype($ftp): string|false {}

/**
 * @param resource $ftp
 * @param resource $stream
 */
function ftp_fget($ftp, $stream, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}

/**
 * @param resource $ftp
 * @param resource $stream
 */
function ftp_nb_fget($ftp, $stream, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): int {}

/** @param resource $ftp */
function ftp_pasv($ftp, bool $enable): bool {}

/** @param resource $ftp */
function ftp_get($ftp, string $local_filename, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}

/** @param resource $ftp */
function ftp_nb_get($ftp, string $local_filename, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): int {}

/** @param resource $ftp */
function ftp_nb_continue($ftp): int {}

/**
 * @param resource $ftp
 * @param resource $stream
 */
function ftp_fput($ftp, string $remote_filename, $stream, int $mode = FTP_BINARY, int $offset = 0): bool {}

/**
 * @param resource $ftp
 * @param resource $stream
 */
function ftp_nb_fput($ftp, string $remote_filename, $stream, int $mode = FTP_BINARY, int $offset = 0): int {}

/** @param resource $ftp */
function ftp_put($ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}

/** @param resource $ftp */
function ftp_append($ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY): bool {}

/** @param resource $ftp */
function ftp_nb_put($ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY, int $offset = 0): int|false {}

/** @param resource $ftp */
function ftp_size($ftp, string $filename): int {}

/** @param resource $ftp */
function ftp_mdtm($ftp, string $filename): int {}

/** @param resource $ftp */
function ftp_rename($ftp, string $from, string $to): bool {}

/** @param resource $ftp */
function ftp_delete($ftp, string $filename): bool {}

/** @param resource $ftp */
function ftp_site($ftp, string $command): bool {}

/** @param resource $ftp */
function ftp_close($ftp): bool {}

/**
 * @param resource $ftp
 * @alias ftp_close
 */
function ftp_quit($ftp): bool {}

/**
 * @param resource $ftp
 * @param int|bool $value
 */
function ftp_set_option($ftp, int $option, $value): bool {}

/** @param resource $ftp */
function ftp_get_option($ftp, int $option): int|bool {}
