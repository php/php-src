<?php

/** @generate-class-entries */

interface Traversable {}

interface IteratorAggregate extends Traversable
{
    /** @tentative-return-type */
    public function getIterator(): Traversable;
}

interface Iterator extends Traversable
{
    /** @tentative-return-type */
    public function current(): mixed;

    /** @tentative-return-type */
    public function next(): void;

    /** @tentative-return-type */
    public function key(): mixed;

    /** @tentative-return-type */
    public function valid(): bool;

    /** @tentative-return-type */
    public function rewind(): void;
}

interface ArrayAccess
{
    /** @tentative-return-type */
    public function offsetExists(mixed $offset): bool;

    /**
     * Actually this should be return by ref but atm cannot be.
     * @tentative-return-type
     */
    public function offsetGet(mixed $offset): mixed;

    /** @tentative-return-type */
    public function offsetSet(mixed $offset, mixed $value): void;

    /** @tentative-return-type */
    public function offsetUnset(mixed $offset): void;
}

interface Serializable
{
    /** @return string|null */
    public function serialize();

    /** @return void */
    public function unserialize(string $data);
}

interface Countable
{
    /** @tentative-return-type */
    public function count(): int;
}

interface Stringable
{
    public function __toString(): string;
}

/**
 * @not-serializable
 */
final class InternalIterator implements Iterator
{
    private function __construct() {}

    public function current(): mixed {}

    public function key(): mixed {}

    public function next(): void {}

    public function valid(): bool {}

    public function rewind(): void {}
}
