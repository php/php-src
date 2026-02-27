<?php

/** @generate-class-entries static */

enum StreamErrorMode
{
    case Error;
    case Exception;
    case Silent;
}

enum StreamErrorStore
{
    case Auto;
    case None;
    case NonTerminating;
    case Terminating;
    case All;
}

final readonly class StreamError
{
    /* General errors */
    /** @cvalue STREAM_ERROR_CODE_NONE */
    public const int CODE_NONE = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_GENERIC */
    public const int CODE_GENERIC = UNKNOWN;

    /* I/O operation errors (10-29) */
    /** @cvalue STREAM_ERROR_CODE_READ_FAILED */
    public const int CODE_READ_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_WRITE_FAILED */
    public const int CODE_WRITE_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_SEEK_FAILED */
    public const int CODE_SEEK_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_SEEK_NOT_SUPPORTED */
    public const int CODE_SEEK_NOT_SUPPORTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_FLUSH_FAILED */
    public const int CODE_FLUSH_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_TRUNCATE_FAILED */
    public const int CODE_TRUNCATE_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_CONNECT_FAILED */
    public const int CODE_CONNECT_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_BIND_FAILED */
    public const int CODE_BIND_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_LISTEN_FAILED */
    public const int CODE_LISTEN_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_NOT_WRITABLE */
    public const int CODE_NOT_WRITABLE = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_NOT_READABLE */
    public const int CODE_NOT_READABLE = UNKNOWN;

    /* File system operations (30-69) */
    /** @cvalue STREAM_ERROR_CODE_DISABLED */
    public const int CODE_DISABLED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_NOT_FOUND */
    public const int CODE_NOT_FOUND = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_PERMISSION_DENIED */
    public const int CODE_PERMISSION_DENIED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_ALREADY_EXISTS */
    public const int CODE_ALREADY_EXISTS = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_INVALID_PATH */
    public const int CODE_INVALID_PATH = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_PATH_TOO_LONG */
    public const int CODE_PATH_TOO_LONG = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_OPEN_FAILED */
    public const int CODE_OPEN_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_CREATE_FAILED */
    public const int CODE_CREATE_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_DUP_FAILED */
    public const int CODE_DUP_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_UNLINK_FAILED */
    public const int CODE_UNLINK_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_RENAME_FAILED */
    public const int CODE_RENAME_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_MKDIR_FAILED */
    public const int CODE_MKDIR_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_RMDIR_FAILED */
    public const int CODE_RMDIR_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_STAT_FAILED */
    public const int CODE_STAT_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_META_FAILED */
    public const int CODE_META_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_CHMOD_FAILED */
    public const int CODE_CHMOD_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_CHOWN_FAILED */
    public const int CODE_CHOWN_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_COPY_FAILED */
    public const int CODE_COPY_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_TOUCH_FAILED */
    public const int CODE_TOUCH_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_INVALID_MODE */
    public const int CODE_INVALID_MODE = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_INVALID_META */
    public const int CODE_INVALID_META = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_MODE_NOT_SUPPORTED */
    public const int CODE_MODE_NOT_SUPPORTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_READONLY */
    public const int CODE_READONLY = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_RECURSION_DETECTED */
    public const int CODE_RECURSION_DETECTED = UNKNOWN;

    /* Wrapper/protocol operations (70-89) */
    /** @cvalue STREAM_ERROR_CODE_NOT_IMPLEMENTED */
    public const int CODE_NOT_IMPLEMENTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_NO_OPENER */
    public const int CODE_NO_OPENER = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_PERSISTENT_NOT_SUPPORTED */
    public const int CODE_PERSISTENT_NOT_SUPPORTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_WRAPPER_NOT_FOUND */
    public const int CODE_WRAPPER_NOT_FOUND = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_WRAPPER_DISABLED */
    public const int CODE_WRAPPER_DISABLED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_PROTOCOL_UNSUPPORTED */
    public const int CODE_PROTOCOL_UNSUPPORTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_WRAPPER_REGISTRATION_FAILED */
    public const int CODE_WRAPPER_REGISTRATION_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_WRAPPER_UNREGISTRATION_FAILED */
    public const int CODE_WRAPPER_UNREGISTRATION_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_WRAPPER_RESTORATION_FAILED */
    public const int CODE_WRAPPER_RESTORATION_FAILED = UNKNOWN;

    /* Filter operations (90-99) */
    /** @cvalue STREAM_ERROR_CODE_FILTER_NOT_FOUND */
    public const int CODE_FILTER_NOT_FOUND = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_FILTER_FAILED */
    public const int CODE_FILTER_FAILED = UNKNOWN;

    /* Cast/conversion operations (100-109) */
    /** @cvalue STREAM_ERROR_CODE_CAST_FAILED */
    public const int CODE_CAST_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_CAST_NOT_SUPPORTED */
    public const int CODE_CAST_NOT_SUPPORTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_MAKE_SEEKABLE_FAILED */
    public const int CODE_MAKE_SEEKABLE_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_BUFFERED_DATA_LOST */
    public const int CODE_BUFFERED_DATA_LOST = UNKNOWN;

    /* Network/socket operations (110-129) */
    /** @cvalue STREAM_ERROR_CODE_NETWORK_SEND_FAILED */
    public const int CODE_NETWORK_SEND_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_NETWORK_RECV_FAILED */
    public const int CODE_NETWORK_RECV_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_SSL_NOT_SUPPORTED */
    public const int CODE_SSL_NOT_SUPPORTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_RESUMPTION_FAILED */
    public const int CODE_RESUMPTION_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_SOCKET_PATH_TOO_LONG */
    public const int CODE_SOCKET_PATH_TOO_LONG = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_OOB_NOT_SUPPORTED */
    public const int CODE_OOB_NOT_SUPPORTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_PROTOCOL_ERROR */
    public const int CODE_PROTOCOL_ERROR = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_INVALID_URL */
    public const int CODE_INVALID_URL = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_INVALID_RESPONSE */
    public const int CODE_INVALID_RESPONSE = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_INVALID_HEADER */
    public const int CODE_INVALID_HEADER = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_INVALID_PARAM */
    public const int CODE_INVALID_PARAM = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_REDIRECT_LIMIT */
    public const int CODE_REDIRECT_LIMIT = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_AUTH_FAILED */
    public const int CODE_AUTH_FAILED = UNKNOWN;

    /* Encoding/decoding/archiving operations (130-139) */
    /** @cvalue STREAM_ERROR_CODE_ARCHIVING_FAILED */
    public const int CODE_ARCHIVING_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_ENCODING_FAILED */
    public const int CODE_ENCODING_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_DECODING_FAILED */
    public const int CODE_DECODING_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_INVALID_FORMAT */
    public const int CODE_INVALID_FORMAT = UNKNOWN;

    /* Resource/allocation operations (140-149) */
    /** @cvalue STREAM_ERROR_CODE_ALLOCATION_FAILED */
    public const int CODE_ALLOCATION_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_TEMPORARY_FILE_FAILED */
    public const int CODE_TEMPORARY_FILE_FAILED = UNKNOWN;

    /* Locking operations (150-159) */
    /** @cvalue STREAM_ERROR_CODE_LOCK_FAILED */
    public const int CODE_LOCK_FAILED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_LOCK_NOT_SUPPORTED */
    public const int CODE_LOCK_NOT_SUPPORTED = UNKNOWN;

    /* Userspace stream operations (160-169) */
    /** @cvalue STREAM_ERROR_CODE_USERSPACE_NOT_IMPLEMENTED */
    public const int CODE_USERSPACE_NOT_IMPLEMENTED = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_USERSPACE_INVALID_RETURN */
    public const int CODE_USERSPACE_INVALID_RETURN = UNKNOWN;
    /** @cvalue STREAM_ERROR_CODE_USERSPACE_CALL_FAILED */
    public const int CODE_USERSPACE_CALL_FAILED = UNKNOWN;

    public int $code;
    public string $message;
    public string $wrapperName;
    public int $severity;
    public bool $terminating;
    public ?string $param;
    public ?StreamError $next;

    public function isIoError(): bool {}

    public function isFileSystemError(): bool {}

    public function isWrapperError(): bool {}

    public function isFilterError(): bool {}

    public function isCastError(): bool {}

    public function isNetworkError(): bool {}

    public function isEncodingError(): bool {}

    public function isResourceError(): bool {}

    public function isLockError(): bool {}

    public function isUserspaceError(): bool {}

    public function hasCode(int $code): bool {}

    public function count(): int {}
}

class StreamException extends Exception
{
    private ?StreamError $error = null;

    public function getError(): ?StreamError {}
}
