<?php

/** @generate-class-entries */

interface Throwable extends Stringable
{
    public function getMessage(): string;

    /** @return int */
    public function getCode();

    public function getFile(): string;

    public function getLine(): int;

    public function getTrace(): array;

    public function getPrevious(): ?Throwable;

    public function getTraceAsString(): string;
}

class Exception implements Throwable
{
    /** @var string */
    protected $message = "";
    /** @var string */
    private $string = "";
    /** @var int */
    protected $code = 0;
    /** @var string|null */
    protected $file = null;
    /** @var int|null */
    protected $line = null;
    private array $trace = [];
    private ?Throwable $previous = null;

    final private function __clone(): void {}

    public function __construct(string $message = "", int $code = 0, ?Throwable $previous = null) {}

    /** @return void */
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
    /** @var int */
    protected $severity = E_ERROR;

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
    /** @var string */
    protected $message = "";
    /** @var string */
    private $string = "";
    /** @var int */
    protected $code = 0;
    /** @var string|null */
    protected $file = null;
    /** @var int|null */
    protected $line = null;
    private array $trace = [];
    private ?Throwable $previous = null;

    /** @implementation-alias Exception::__clone */
    final private function __clone(): void {}

    /** @implementation-alias Exception::__construct */
    public function __construct(string $message = "", int $code = 0, ?Throwable $previous = null) {}

    /**
     * @return void
     * @implementation-alias Exception::__wakeup
     */
    public function __wakeup() {}

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
