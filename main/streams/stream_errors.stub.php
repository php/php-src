<?php

/** @generate-class-entries static */

enum StreamErrorCode: int
{
    // Error code cases registered in code

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
    case NonTerminal;
    case Terminal;
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
    public ?StreamError $next;

    public function hasCode(StreamErrorCode $code): bool {}

    public function count(): int {}
}

class StreamException extends Exception
{
    private ?StreamError $error = null;

    public function getError(): ?StreamError {}
}
