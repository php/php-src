<?php

/** @return resource|false */
function ftp_connect(string $host, int $port = 21, int $timeout = 90) {}

#ifdef HAVE_FTP_SSL
/** @return resource|false */
function ftp_ssl_connect(string $host, int $port = 21, int $timeout = 90) {}
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
function ftp_raw($ftp, string $command): array {}

/** @param resource $ftp */
function ftp_mkdir($ftp, string $directory): string|false {}

/** @param resource $ftp */
function ftp_rmdir($ftp, string $directory): bool {}

/** @param resource $ftp */
function ftp_chmod($ftp, int $mode, string $filename): int|false {}

/** @param resource $ftp */
function ftp_alloc($ftp, int $size, &$response = UNKNOWN): bool {}

/** @param resource $ftp */
function ftp_nlist($ftp, string $directory): array|false {}

/** @param resource $ftp */
function ftp_rawlist($ftp, string $directory, bool $recurse = false): array|false {}

/** @param resource $ftp */
function ftp_mlsd($ftp, string $directory): array|false {}

/** @param resource $ftp */
function ftp_systype($ftp): string|false {}

/**
 * @param resource $ftp
 * @param resource $fp
 */
function ftp_fget($ftp, $fp, string $remote_file, int $mode = FTP_BINARY, int $resumepos = 0): bool {}

/**
 * @param resource $ftp
 * @param resource $fp
 */
function ftp_nb_fget($ftp, $fp, string $remote_file, int $mode = FTP_BINARY, int $resumpos = 0): int|false {}

/** @param resource $ftp */
function ftp_pasv($ftp, bool $pasv): bool {}

/** @param resource $ftp */
function ftp_get($ftp, string $local_file, string $remote_file, int $mode = FTP_BINARY, int $resumepos = 0): bool {}

/** @param resource $ftp */
function ftp_nb_get($ftp, string $local_file, string $remote_file, int $mode = FTP_BINARY, int $resume_pos = 0): int {}

/** @param resource $ftp */
function ftp_nb_continue($ftp): int {}

/**
 * @param resource $ftp
 * @param resource $fp
 */
function ftp_fput($ftp, string $remote_file, $fp, int $mode = FTP_BINARY, int $startpos = 0): bool {}

/**
 * @param resource $ftp
 * @param resource $fp
 */
function ftp_nb_fput($ftp, string $remote_file, $fp, $mode = FTP_BINARY, $startpos = 0): int|false {}

/** @param resource $ftp */
function ftp_put($ftp, string $remote_file, string $local_file, int $mode = FTP_BINARY, int $startpos = 0): bool {}

/** @param resource $ftp */
function ftp_append($ftp, string $remove_file, string $local_file, int $mode = FTP_BINARY): bool {}

/** @param resource $ftp */
function ftp_nb_put($ftp, string $remote_file, string $local_file, int $mode = FTP_BINARY, int $startpos = 0): int|false {}

/** @param resource $ftp */
function ftp_size($ftp, string $filename): int {}

/** @param resource $ftp */
function ftp_mdtm($ftp, string $filename): int {}

/** @param resource $ftp */
function ftp_rename($ftp, string $src, string $dest): bool {}

/** @param resource $ftp */
function ftp_delete($ftp, string $file): bool {}

/** @param resource $ftp */
function ftp_site($ftp, string $cmd): bool {}

/** @param resource $ftp */
function ftp_close($ftp): bool {}

/** @param resource $ftp */
function ftp_set_option($ftp, int $option, $value): bool {}

/** @param resource $ftp */
function ftp_get_option($ftp, int $option): int|bool {}
