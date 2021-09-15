<?php

/** @generate-class-entries */

final class Vector implements IteratorAggregate, Countable, JsonSerializable, ArrayAccess
{
    /**
     * Construct a Vector from an iterable.
     *
     * When $preserveKeys is false, the values will be reindexed without gaps starting from 0
     * When $preserveKeys is true, any gaps in the keys of the iterable will be filled in with null,
     * and negative indices or non-integer indices will be rejected and cause an Exception.
     */
    public function __construct(iterable $iterator = [], bool $preserveKeys = true) {}
    public function getIterator(): InternalIterator {}
    public function count(): int {}
    public function capacity(): int {}
    public function shrinkToFit(): void {}
    public function clear(): void {}
    public function setSize(int $size): void {}

    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    public static function __set_state(array $array): Vector {}

    public function push(mixed $value): void {}
    public function pop(): mixed {}

    public function toArray(): array {}
    // Strictly typed, unlike offsetGet/offsetSet
    public function valueAt(int $offset): mixed {}
    public function setValueAt(int $offset, mixed $value): void {}

    public function offsetGet(mixed $offset): mixed {}
    public function offsetExists(mixed $offset): bool {}
    public function offsetSet(mixed $offset, mixed $value): void {}
    // Throws because unset and null are different things, unlike SplFixedArray
    public function offsetUnset(mixed $offset): void {}

    public function indexOf(mixed $value): int|false {}
    public function contains(mixed $value): bool {}

    public function map(callable $callback): Vector {}
    /**
     * Returns the subset of elements of the Vector satisfying the predicate.
     *
     * If the value returned by the callback is truthy
     * (e.g. true, non-zero number, non-empty array, truthy object, etc.),
     * this is treated as satisfying the predicate.
     *
     * (at)param null|callable(mixed):mixed $callback
     */
    public function filter(?callable $callback = null): Vector {}

    public function jsonSerialize(): array {}
}
