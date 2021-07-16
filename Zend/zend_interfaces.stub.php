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
    /** @return mixed */
    public function current(); // TODO Can't add tentative return tyoe due to DirectoryIterator::current()

    /** @tentative-return-type */
    public function next(): void;

    /** @return mixed */
    public function key(); // TODO Can't add tentative return tyoe due to DirectoryIterator::key()

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

    /** @return void */
    public function offsetUnset(mixed $offset); // TODO Can't add tentative return type due to Phar::offsetUnset
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
    public function count(); // TODO Can't add tentatie return type due to DOMNodeList::count() and DOMNamedNodeMap::count()
}

interface Stringable
{
    public function __toString(): string;
}

final class InternalIterator implements Iterator
{
    private function __construct() {}

    public function current(): mixed {}

    public function key(): mixed {}

    public function next(): void {}

    public function valid(): bool {}

    public function rewind(): void {}
}
