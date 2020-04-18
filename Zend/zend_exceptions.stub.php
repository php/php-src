<?php

interface Throwable extends Stringable
{
    /** @return string */
    public function getMessage();

    /** @return int */
    public function getCode();

    /** @return string */
    public function getFile();

    /** @return int */
    public function getLine();

    /** @return array */
    public function getTrace();

    /** @return ?Throwable */
    public function getPrevious();

    /** @return string */
    public function getTraceAsString();
}

class Exception implements Throwable
{
    final private function __clone() {}

    public function __construct(string $message = UNKNOWN, int $code = 0, ?Throwable $previous = null) {}

    public function __wakeup() {}

    /** @return string */
    final public function getMessage() {}

    /** @return int */
    final public function getCode() {}

    /** @return string */
    final public function getFile() {}

    /** @return int */
    final public function getLine() {}

    /** @return array */
    final public function getTrace() {}

    /** @return ?Throwable */
    final public function getPrevious() {}

    /** @return string */
    final public function getTraceAsString() {}

    public function __toString(): string {}
}

class ErrorException extends Exception
{
    public function __construct(string $message = UNKNOWN, int $code = 0, int $severity = E_ERROR, string $filename = UNKNOWN, int $lineno = 0, ?Throwable $previous = null) {}

    /** @return int */
    final public function getSeverity() {}
}
