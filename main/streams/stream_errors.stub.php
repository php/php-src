<?php

/**
 * @generate-class-entries static
 * @generate-c-enums
 */

/** @c-name-table */
enum StreamErrorCode
{
    /* General errors */
    case None;
    case Generic;

    /* I/O operation errors */
    case ReadFailed;
    case WriteFailed;
    case SeekFailed;
    case SeekNotSupported;
    case FlushFailed;
    case TruncateFailed;
    case ConnectFailed;
    case BindFailed;
    case ListenFailed;
    case NotWritable;
    case NotReadable;

    /* File system operations */
    case Disabled;
    case NotFound;
    case PermissionDenied;
    case AlreadyExists;
    case InvalidPath;
    case PathTooLong;
    case OpenFailed;
    case CreateFailed;
    case DupFailed;
    case UnlinkFailed;
    case RenameFailed;
    case MkdirFailed;
    case RmdirFailed;
    case StatFailed;
    case MetaFailed;
    case ChmodFailed;
    case ChownFailed;
    case CopyFailed;
    case TouchFailed;
    case InvalidMode;
    case InvalidMeta;
    case ModeNotSupported;
    case Readonly;
    case RecursionDetected;

    /* Wrapper/protocol operations */
    case NotImplemented;
    case NoOpener;
    case PersistentNotSupported;
    case WrapperNotFound;
    case WrapperDisabled;
    case ProtocolUnsupported;
    case WrapperRegistrationFailed;
    case WrapperUnregistrationFailed;
    case WrapperRestorationFailed;

    /* Filter operations */
    case FilterNotFound;
    case FilterFailed;

    /* Cast/conversion operations */
    case CastFailed;
    case CastNotSupported;
    case MakeSeekableFailed;
    case BufferedDataLost;

    /* Network/socket operations */
    case NetworkSendFailed;
    case NetworkRecvFailed;
    case SslNotSupported;
    case ResumptionFailed;
    case SocketPathTooLong;
    case OobNotSupported;
    case ProtocolError;
    case InvalidUrl;
    case InvalidResponse;
    case InvalidHeader;
    case InvalidParam;
    case RedirectLimit;
    case AuthFailed;

    /* Encoding/decoding/archiving operations */
    case ArchivingFailed;
    case EncodingFailed;
    case DecodingFailed;
    case InvalidFormat;

    /* Resource/allocation operations */
    case AllocationFailed;
    case TemporaryFileFailed;

    /* Locking operations */
    case LockFailed;
    case LockNotSupported;

    /* Userspace stream operations */
    case UserspaceNotImplemented;
    case UserspaceInvalidReturn;
    case UserspaceCallFailed;

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
}

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
    public StreamErrorCode $code;
    public string $message;
    public string $wrapperName;
    public int $severity;
    public bool $terminating;
    public ?string $param;
}

class StreamException extends Exception
{
    /** @var list<StreamError> */
    private array $errors = [];

    /** @return list<StreamError> */
    public function getErrors(): array {}
}
