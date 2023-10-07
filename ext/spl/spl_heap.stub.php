<?php

/** @generate-class-entries */

class SplPriorityQueue implements Iterator, Countable
{
    /** @cvalue SPL_PQUEUE_EXTR_BOTH */
    public const int EXTR_BOTH = UNKNOWN;
    /** @cvalue SPL_PQUEUE_EXTR_PRIORITY */
    public const int EXTR_PRIORITY = UNKNOWN;
    /** @cvalue SPL_PQUEUE_EXTR_DATA */
    public const int EXTR_DATA = UNKNOWN;

    /** @tentative-return-type */
    public function compare(mixed $priority1, mixed $priority2): int {}

    /** @return true */
    public function insert(mixed $value, mixed $priority) {} // TODO make return type void

    /** @tentative-return-type */
    public function setExtractFlags(int $flags): int {}

    /** @tentative-return-type */
    public function top(): mixed {}

    /** @tentative-return-type */
    public function extract(): mixed {}

    /**
     * @tentative-return-type
     * @implementation-alias SplHeap::count
     */
    public function count(): int {}

    /**
     * @tentative-return-type
     * @implementation-alias SplHeap::isEmpty
     */
    public function isEmpty(): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias SplHeap::rewind
     */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /**
     * @tentative-return-type
     * @implementation-alias SplHeap::key
     */
    public function key(): int {}

    /**
     * @tentative-return-type
     * @implementation-alias SplHeap::next
     */
    public function next(): void {}

    /**
     * @tentative-return-type
     * @implementation-alias SplHeap::valid
     */
    public function valid(): bool {}

    /**
     * @return bool
     * @implementation-alias SplHeap::recoverFromCorruption
     */
    public function recoverFromCorruption() {} // TODO make return type void

    /**
     * @tentative-return-type
     * @implementation-alias SplHeap::isCorrupted
     */
    public function isCorrupted(): bool {}

    /** @tentative-return-type */
    public function getExtractFlags(): int {}

    /** @tentative-return-type */
    public function __debugInfo(): array {}
}

abstract class SplHeap implements Iterator, Countable
{
    /** @tentative-return-type */
    public function extract(): mixed {}

    /** @tentative-return-type */
    public function insert(mixed $value): bool {}

    /** @tentative-return-type */
    public function top(): mixed {}

    /** @tentative-return-type */
    public function count(): int {}

    /** @tentative-return-type */
    public function isEmpty(): bool {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /** @tentative-return-type */
    public function key(): int {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function recoverFromCorruption(): bool {}

    /** @tentative-return-type */
    abstract protected function compare(mixed $value1, mixed $value2): int;

    /** @tentative-return-type */
    public function isCorrupted(): bool {}

    /** @tentative-return-type */
    public function __debugInfo(): array {}
}

class SplMinHeap extends SplHeap
{
    /** @tentative-return-type */
    protected function compare(mixed $value1, mixed $value2): int {}
}

class SplMaxHeap extends SplHeap
{
    /** @tentative-return-type */
    protected function compare(mixed $value1, mixed $value2): int {}
}
