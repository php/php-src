<?php

/** @generate-function-entries */

interface Traversable {}

interface IteratorAggregate extends Traversable
{
    /** @return Traversable */
    public function getIterator();
}

interface Iterator extends Traversable
{
    /** @return mixed */
    public function current();

    /** @return void */
    public function next();

    /** @return mixed */
    public function key();

    /** @return bool */
    public function valid();

    /** @return void */
    public function rewind();
}

interface ArrayAccess
{
    /** @return bool */
    public function offsetExists(mixed $offset);

    /**
     * Actually this should be return by ref but atm cannot be.
     * @return mixed
     */
    public function offsetGet(mixed $offset);

    /** @return void */
    public function offsetSet(mixed $offset, mixed $value);

    /** @return void */
    public function offsetUnset(mixed $offset);
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
    /** @return int */
    public function count();
}

interface Stringable
{
    public function __toString(): string;
}

final class InternalIterator implements Iterator
{
    private function __construct();

    /** @return mixed */
    public function current();

    /** @return mixed */
    public function key();

    public function next(): void;

    public function valid(): bool;

    public function rewind(): void;
}
