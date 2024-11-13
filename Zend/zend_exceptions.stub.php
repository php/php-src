<?php

/** @generate-class-entries */

require "zend_constants.stub.php";

interface Throwable extends Stringable
{
    public function getMessage(): string;

    /** @return int */
    public function getCode(); // TODO add proper type (i.e. int|string)

    public function getFile(): string;

    public function getLine(): int;

    public function getTrace(): array;

    public function getPrevious(): ?Throwable;

    public function getTraceAsString(): string;
}

class Exception implements Throwable
{
    /**
     * Intentionally left untyped for BC reasons
     * @var string
     */
    protected $message = "";
    private string $string = "";
    /**
     * Intentionally left untyped for BC reasons
     * @var int
     */
    protected $code = 0;  // TODO add proper type (i.e. int|string)
    protected string $file = "";
    protected int $line = 0;
    private array $trace = [];
    private ?Throwable $previous = null;

    private function __clone(): void {}

    public function __construct(string $message = "", int $code = 0, ?Throwable $previous = null) {}

    /** @tentative-return-type */
    public function __wakeup(): void {}

    final public function getMessage(): string {}

    /** @return int */
    final public function getCode() {} // TODO add proper type (i.e. int|string)

    final public function getFile(): string {}

    final public function getLine(): int {}

    final public function getTrace(): array {}

    final public function getPrevious(): ?Throwable {}

    final public function getTraceAsString(): string {}

    public function __toString(): string {}
}

class ErrorException extends Exception
{
    protected int $severity = E_ERROR;

    public function __construct(
        string $message = "",
        int $code = 0,
        int $severity = E_ERROR,
        ?string $filename = null,
        ?int $line = null,
        ?Throwable $previous = null
    ) {}

    final public function getSeverity(): int {}
}

class Error implements Throwable
{
    /**
     * Intentionally left untyped for BC reasons
     * @var string
     */
    protected $message = "";
    private string $string = "";
    /**
     * Intentionally left untyped for BC reasons
     * @var int
     */
    protected $code = 0; // TODO add proper type (i.e. int|string)
    protected string $file = "";
    protected int $line;
    private array $trace = [];
    private ?Throwable $previous = null;

    /** @implementation-alias Exception::__clone */
    private function __clone(): void {}

    /** @implementation-alias Exception::__construct */
    public function __construct(string $message = "", int $code = 0, ?Throwable $previous = null) {}

    /**
     * @tentative-return-type
     * @implementation-alias Exception::__wakeup
     */
    public function __wakeup(): void {}

    /** @implementation-alias Exception::getMessage */
    final public function getMessage(): string {}

    /**
     * @return int
     * @implementation-alias Exception::getCode
     */
    final public function getCode() {}

    /** @implementation-alias Exception::getFile */
    final public function getFile(): string {}

    /** @implementation-alias Exception::getLine */
    final public function getLine(): int {}

    /** @implementation-alias Exception::getTrace */
    final public function getTrace(): array {}

    /** @implementation-alias Exception::getPrevious */
    final public function getPrevious(): ?Throwable {}

    /** @implementation-alias Exception::getTraceAsString */
    final public function getTraceAsString(): string {}

    /** @implementation-alias Exception::__toString */
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

class UnhandledMatchError extends Error
{
}

class RequestParseBodyException extends Exception
{
}
