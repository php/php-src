<?php

interface Throwable
{
    /** @return string */
    function getMessage();

    /** @return int */
    function getCode();

    /** @return string */
    function getFile();

    /** @return int */
    function getLine();

    /** @return array */
    function getTrace();

    /** @return ?Throwable */
    function getPrevious();

    /** @return string */
    function getTraceAsString();

    /** @return string */
    function __toString();
}

class Exception implements Throwable
{
    final private function __clone() {}

    function __construct(string $message = UNKNOWN, int $code = 0, ?Throwable $previous = null) {}

    function __wakeup() {}

    /** @return string */
    final function getMessage() {}

    /** @return int */
    final function getCode() {}

    /** @return string */
    final function getFile() {}

    /** @return int */
    final function getLine() {}

    /** @return array */
    final function getTrace() {}

    /** @return ?Throwable */
    final function getPrevious() {}

    /** @return string */
    final function getTraceAsString() {}

    /** @return string */
    function __toString() {}
}

class ErrorException extends Exception
{
    function __construct(string $message = UNKNOWN, int $code = 0, int $severity = E_ERROR, string $filename = UNKNOWN, int $lineno = 0, ?Throwable $previous = null) {}

    final function getSeverity() {}
}
