<?php

/** @generate-class-entries */

class ArrayObject implements IteratorAggregate, ArrayAccess, Serializable, Countable
{
    /**
     * @var int
     * @cvalue SPL_ARRAY_STD_PROP_LIST
     */
    const STD_PROP_LIST = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_ARRAY_ARRAY_AS_PROPS
     */
    const ARRAY_AS_PROPS = UNKNOWN;

    public function __construct(array|object $array = [], int $flags = 0, string $iteratorClass = ArrayIterator::class) {}

    /** @tentative-return-type */
    public function offsetExists(mixed $key): bool {}

    /** @tentative-return-type */
    public function offsetGet(mixed $key): mixed {}

    /** @tentative-return-type */
    public function offsetSet(mixed $key, mixed $value): void {}

    /** @tentative-return-type */
    public function offsetUnset(mixed $key): void {}

    /** @tentative-return-type */
    public function append(mixed $value): void {}

    /** @tentative-return-type */
    public function getArrayCopy(): array {}

    /** @tentative-return-type */
    public function count(): int {}

    /** @tentative-return-type */
    public function getFlags(): int {}

    /** @tentative-return-type */
    public function setFlags(int $flags): void {}

    /** @tentative-return-type */
    public function asort(int $flags = SORT_REGULAR): true {}

    /** @tentative-return-type */
    public function ksort(int $flags = SORT_REGULAR): true {}

    /** @tentative-return-type */
    public function uasort(callable $callback): true {}

    /** @tentative-return-type */
    public function uksort(callable $callback): true {}

    /** @tentative-return-type */
    public function natsort(): true {}

    /** @tentative-return-type */
    public function natcasesort(): true {}

    /** @tentative-return-type */
    public function unserialize(string $data): void {}

    /** @tentative-return-type */
    public function serialize(): string {}

    /** @tentative-return-type */
    public function __serialize(): array {}

    /** @tentative-return-type */
    public function __unserialize(array $data): void {}

    /** @tentative-return-type */
    public function getIterator(): Iterator {}

    /** @tentative-return-type */
    public function exchangeArray(array|object $array): array {}

    /** @tentative-return-type */
    public function setIteratorClass(string $iteratorClass): void {}

    /** @tentative-return-type */
    public function getIteratorClass(): string {}

    /** @tentative-return-type */
    public function __debugInfo(): array {}
}

class ArrayIterator implements SeekableIterator, ArrayAccess, Serializable, Countable
{
    /**
     * @var int
     * @cvalue SPL_ARRAY_STD_PROP_LIST
     */
    public const STD_PROP_LIST = UNKNOWN;
    /**
     * @var int
     * @cvalue SPL_ARRAY_ARRAY_AS_PROPS
     */
    public const ARRAY_AS_PROPS = UNKNOWN;

    public function __construct(array|object $array = [], int $flags = 0) {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::offsetExists
     */
    public function offsetExists(mixed $key): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::offsetGet
     */
    public function offsetGet(mixed $key): mixed {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::offsetSet
     */
    public function offsetSet(mixed $key, mixed $value): void {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::offsetUnset
     */
    public function offsetUnset(mixed $key): void {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::append
     */
    public function append(mixed $value): void {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::getArrayCopy
     */
    public function getArrayCopy(): array {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::count
     */
    public function count(): int {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::getFlags
     */
    public function getFlags(): int {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::setFlags
     */
    public function setFlags(int $flags): void {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::asort
     */
    public function asort(int $flags = SORT_REGULAR): true {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::ksort
     */
    public function ksort(int $flags = SORT_REGULAR): true {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::uasort
     */
    public function uasort(callable $callback): true {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::uksort
     */
    public function uksort(callable $callback): true {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::natsort
     */
    public function natsort(): true {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::natcasesort
     */
    public function natcasesort(): true {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::unserialize
     */
    public function unserialize(string $data): void {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::serialize
     */
    public function serialize(): string {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::__serialize
     */
    public function __serialize(): array {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::__unserialize
     */
    public function __unserialize(array $data): void {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function current(): mixed {}

    /** @tentative-return-type */
    public function key(): string|int|null {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function seek(int $offset): void {}

    /**
     * @tentative-return-type
     * @implementation-alias ArrayObject::__debugInfo
     */
    public function __debugInfo(): array {}
}

class RecursiveArrayIterator extends ArrayIterator implements RecursiveIterator
{
    /**
     * @var int
     * @cvalue SPL_ARRAY_CHILD_ARRAYS_ONLY
     */
    public const CHILD_ARRAYS_ONLY = UNKNOWN;

    /** @tentative-return-type */
    public function hasChildren(): bool {}

    /** @tentative-return-type */
    public function getChildren(): ?RecursiveArrayIterator {}
}
