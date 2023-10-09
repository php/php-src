<?php

/** @generate-class-entries */

namespace {
    /**
     * @var int
     * @cvalue FTPTYPE_ASCII
     */
    const FTP_ASCII = UNKNOWN;
    /**
     * @var int
     * @cvalue FTPTYPE_ASCII
     */
    const FTP_TEXT = UNKNOWN;
    /**
     * @var int
     * @cvalue FTPTYPE_IMAGE
     */
    const FTP_BINARY = UNKNOWN;
    /**
     * @var int
     * @cvalue FTPTYPE_IMAGE
     */
    const FTP_IMAGE = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_FTP_AUTORESUME
     */
    const FTP_AUTORESUME = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_FTP_OPT_TIMEOUT_SEC
     */
    const FTP_TIMEOUT_SEC = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_FTP_OPT_AUTOSEEK
     */
    const FTP_AUTOSEEK = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_FTP_OPT_USEPASVADDRESS
     */
    const FTP_USEPASVADDRESS = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_FTP_FAILED
     */
    const FTP_FAILED = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_FTP_FINISHED
     */
    const FTP_FINISHED = UNKNOWN;
    /**
     * @var int
     * @cvalue PHP_FTP_MOREDATA
     */
    const FTP_MOREDATA = UNKNOWN;

    function ftp_connect(string $hostname, int $port = 21, int $timeout = 90): FTP\Connection|false {}

    #ifdef HAVE_FTP_SSL
    function ftp_ssl_connect(string $hostname, int $port = 21, int $timeout = 90): FTP\Connection|false {}
    #endif

    function ftp_login(FTP\Connection $ftp, string $username, #[\SensitiveParameter] string $password): bool {}
    function ftp_pwd(FTP\Connection $ftp): string|false {}
    function ftp_cdup(FTP\Connection $ftp): bool {}
    function ftp_chdir(FTP\Connection $ftp, string $directory): bool {}
    function ftp_exec(FTP\Connection $ftp, string $command): bool {}

    /**
     * @return array<int, string>|null
     * @refcount 1
     */
    function ftp_raw(FTP\Connection $ftp, string $command): ?array {}
    function ftp_mkdir(FTP\Connection $ftp, string $directory): string|false {}
    function ftp_rmdir(FTP\Connection $ftp, string $directory): bool {}
    function ftp_chmod(FTP\Connection $ftp, int $permissions, string $filename): int|false {}

    /** @param string $response */
    function ftp_alloc(FTP\Connection $ftp, int $size, &$response = null): bool {}

    /**
     * @return array<int, string>|false
     * @refcount 1
     */
    function ftp_nlist(FTP\Connection $ftp, string $directory): array|false {}

    /**
     * @return array<int, string>|false
     * @refcount 1
     */
    function ftp_rawlist(FTP\Connection $ftp, string $directory, bool $recursive = false): array|false {}

    /**
     * @return array<int, array>|false
     * @refcount 1
     */
    function ftp_mlsd(FTP\Connection $ftp, string $directory): array|false {}

    function ftp_systype(FTP\Connection $ftp): string|false {}

    /** @param resource $stream */
    function ftp_fget(FTP\Connection $ftp, $stream, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}

    /** @param resource $stream */
    function ftp_nb_fget(FTP\Connection $ftp, $stream, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): int {}
    function ftp_pasv(FTP\Connection $ftp, bool $enable): bool {}
    function ftp_get(FTP\Connection $ftp, string $local_filename, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}
    function ftp_nb_get(FTP\Connection $ftp, string $local_filename, string $remote_filename, int $mode = FTP_BINARY, int $offset = 0): int|false {}
    function ftp_nb_continue(FTP\Connection $ftp): int {}

    /** @param resource $stream */
    function ftp_fput(FTP\Connection $ftp, string $remote_filename, $stream, int $mode = FTP_BINARY, int $offset = 0): bool {}

    /** @param resource $stream */
    function ftp_nb_fput(FTP\Connection $ftp, string $remote_filename, $stream, int $mode = FTP_BINARY, int $offset = 0): int {}
    function ftp_put(FTP\Connection $ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY, int $offset = 0): bool {}
    function ftp_append(FTP\Connection $ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY): bool {}
    function ftp_nb_put(FTP\Connection $ftp, string $remote_filename, string $local_filename, int $mode = FTP_BINARY, int $offset = 0): int|false {}
    function ftp_size(FTP\Connection $ftp, string $filename): int {}
    function ftp_mdtm(FTP\Connection $ftp, string $filename): int {}
    function ftp_rename(FTP\Connection $ftp, string $from, string $to): bool {}
    function ftp_delete(FTP\Connection $ftp, string $filename): bool {}
    function ftp_site(FTP\Connection $ftp, string $command): bool {}
    function ftp_close(FTP\Connection $ftp): bool {}

    /** @alias ftp_close */
    function ftp_quit(FTP\Connection $ftp): bool {}

    /** @param int|bool $value */
    function ftp_set_option(FTP\Connection $ftp, int $option, $value): bool {}
    function ftp_get_option(FTP\Connection $ftp, int $option): int|bool {}
}

namespace FTP {
    /**
     * @strict-properties
     * @not-serializable
     */
    final class Connection
    {
    }
}
