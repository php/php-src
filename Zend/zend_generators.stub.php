<?php

/** @generate-function-entries */

final class Generator implements Iterator
{
    public function rewind(): void {}

    public function valid(): bool {}

    public function current(): mixed {}

    public function key(): mixed {}

    public function next(): void {}

    public function send(mixed $value): mixed {}

    public function throw(Throwable $exception): mixed {}

    public function getReturn(): mixed {}
}

class ClosedGeneratorException extends Exception
{
}
