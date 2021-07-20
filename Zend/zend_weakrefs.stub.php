<?php

/** @generate-class-entries */

/**
 * @strict-properties
 * @not-serializable
 */
final class WeakReference
{
    public function __construct() {}

    public static function create(object $object): WeakReference {}

    public function get(): ?object {}
}

/**
 * @strict-properties
 * @not-serializable
 */
final class WeakMap implements ArrayAccess, Countable, IteratorAggregate
{
    /** @param object $object */
    public function offsetGet($object): mixed {}

    /** @param object $object */
    public function offsetSet($object, mixed $value): void {}

    /** @param object $object */
    public function offsetExists($object): bool {}

    /** @param object $object */
    public function offsetUnset($object): void {}

    public function count(): int {}

    public function getIterator(): Iterator {}
}
