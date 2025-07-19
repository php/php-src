<?php

/** @generate-class-entries */

require "zend_constants.stub.php";

class CancellationException implements Throwable
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