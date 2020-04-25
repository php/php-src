<?php

/** @generate-function-entries */

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

    final public function getMessage(): string {}

    /** @return int */
    final public function getCode() {}

    final public function getFile(): string {}

    final public function getLine(): int {}

    final public function getTrace(): array {}

    final public function getPrevious(): ?Throwable {}

    final public function getTraceAsString(): string {}

    public function __toString(): string {}
}

class ErrorException extends Exception
{
    public function __construct(string $message = UNKNOWN, int $code = 0, int $severity = E_ERROR, string $filename = UNKNOWN, int $lineno = 0, ?Throwable $previous = null) {}

    final public function getSeverity(): int {}
}

class Error implements Throwable
{
    /** @alias Exception::__clone */
    final private function __clone() {}

    /** @alias Exception::__construct */
    public function __construct(string $message = UNKNOWN, int $code = 0, ?Throwable $previous = null) {}

    /** @alias Exception::__wakeup */
    public function __wakeup() {}

    /** @alias Exception::getMessage */
    final public function getMessage(): string {}

    /**
     * @return int
     * @alias Exception::getCode
     */
    final public function getCode() {}

    /** @alias Exception::getFile */
    final public function getFile(): string {}

    /** @alias Exception::getLine */
    final public function getLine(): int {}

    /** @alias Exception::getTrace */
    final public function getTrace(): array {}

    /** @alias Exception::getPrevious */
    final public function getPrevious(): ?Throwable {}

    /** @alias Exception::getTraceAsString */
    final public function getTraceAsString(): string {}

    /** @alias Exception::__toString */
    public function __toString(): string {}
}

class CompileError extends Error
{
}

class ParseError extends CompileError
{
}

class TypeError extends Error
{
}

class ArgumentCountError extends TypeError
{
}

class ValueError extends Error
{
}

class ArithmeticError extends Error
{
}

class DivisionByZeroError extends ArithmeticError
{
}
