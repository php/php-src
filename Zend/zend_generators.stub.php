<?php

/** @generate-class-entries */

/**
 * @strict-properties
 * @not-serializable
 */
final class Generator implements Iterator
{
    #[\NonpublicConstructor("the \"Generator\" class is reserved for internal use and cannot be manually instantiated")]
    private function __construct() {}

    public function rewind(): void {}

    public function valid(): bool {}

    public function current(): mixed {}

    public function key(): mixed {}

    public function next(): void {}

    public function send(mixed $value): mixed {}

    public function throw(Throwable $exception): mixed {}

    public function getReturn(): mixed {}

    public function __debugInfo(): array {}
}

class ClosedGeneratorException extends Exception
{
}
