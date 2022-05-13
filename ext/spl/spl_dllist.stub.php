<?php

/** @generate-class-entries */

class SplDoublyLinkedList implements Iterator, Countable, ArrayAccess, Serializable
{
    /** @tentative-return-type */
    public function add(int $index, mixed $value): void {}

    /** @tentative-return-type */
    public function pop(): mixed {}

    /** @tentative-return-type */
    public function shift(): mixed {}

    /** @tentative-return-type */
    public function push(mixed $value): void {}

    /** @tentative-return-type */
    public function unshift(mixed $value): void {}

    /** @tentative-return-type */
    public function top(): mixed {}

    /** @tentative-return-type */
    public function bottom(): mixed {}

    /** @tentative-return-type */
    public function __debugInfo(): array {}

    /** @tentative-return-type */
    public function count(): int {}

    /** @tentative-return-type */
    public function isEmpty(): bool {}

    /** @tentative-return-type */
    public function setIteratorMode(int $mode): int {}

    /** @tentative-return-type */
    public function getIteratorMode(): int {}

    /**
     * @param int $index
     * @tentative-return-type
     */
    public function offsetExists($index): bool {}

    /**
     * @param int $index
     * @tentative-return-type
     */
    public function offsetGet($index): mixed {}

    /**
     * @param int|null $index
     * @tentative-return-type
     */
    public function offsetSet($index, mixed $value): void {}

    /**
     * @param int $index
     * @tentative-return-type
     */
    public function offsetUnset($index): void {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /** @tentative-return-type */
    public function key(): int {}

    /** @tentative-return-type */
    public function prev(): void {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function unserialize(string $data): void {}

    /** @tentative-return-type */
    public function serialize(): string {}

    /** @tentative-return-type */
    public function __serialize(): array {}

    /** @tentative-return-type */
    public function __unserialize(array $data): void {}
}

class SplQueue extends SplDoublyLinkedList
{
    /**
     * @tentative-return-type
     * @implementation-alias SplDoublyLinkedList::push
     */
    public function enqueue(mixed $value): void {}

    /**
     * @tentative-return-type
     * @implementation-alias SplDoublyLinkedList::shift
     */
    public function dequeue(): mixed {}
}

class SplStack extends SplDoublyLinkedList
{
}
