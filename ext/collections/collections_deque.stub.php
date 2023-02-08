<?php

/** @generate-class-entries */

namespace Collections;

/**
 * A double-ended queue (Typically abbreviated as Deque, pronounced "deck", like "cheque")
 * represented internally as a circular buffer.
 *
 * This has much lower memory usage than SplDoublyLinkedList or its subclasses (SplStack, SplStack),
 * and operations are significantly faster than SplDoublyLinkedList.
 *
 * See https://en.wikipedia.org/wiki/Double-ended_queue
 *
 * This supports amortized constant time pushing and popping onto the front or back of the Deque.
 *
 * Naming is based on https://www.php.net/spldoublylinkedlist
 * and on array_push/pop/pushFront/popFront and array_key_first.
 */
final class Deque implements \IteratorAggregate, \Countable, \JsonSerializable, \ArrayAccess
{
    /** Construct the Deque from the values of the Traversable/array, ignoring keys */
    public function __construct(iterable $iterator = []) {}
    /**
     * Returns an iterator that accounts for calls to popFront/pushFront tracking the position of the start of the Deque.
     * Calls to popFront/pushFront will do the following:
     * - Increase/Decrease the value returned by the iterator's key()
     *   by the number of elements added/removed to/from the start of the Deque.
     *   (`$deque[$iteratorKey] === $iteratorValue` at the time the key and value are returned).
     * - Repeated calls to popFront will cause valid() to return false if the iterator's
     *   position ends up before the start of the Deque at the time iteration resumes.
     * - They will not cause the remaining values to be iterated over more than once or skipped.
     */
    public function getIterator(): \InternalIterator {}
    /** Returns the number of elements in the Deque. */
    public function count(): int {}
    /** Returns true if there are 0 elements in the Deque. */
    public function isEmpty(): bool {}
    /** Removes all elements from the Deque. */
    public function clear(): void {}

    /** @implementation-alias Collections\Deque::toArray */
    public function __serialize(): array {}
    public function __unserialize(array $data): void {}
    /** Construct the Deque from the values of the array, ignoring keys */
    public static function __set_state(array $array): Deque {}

    /** Appends value(s) to the end of the Deque, like array_push. */
    public function push(mixed ...$values): void {}
    /** Prepends value(s) to the start of the Deque, like array_pushFront. */
    public function pushFront(mixed ...$values): void {}
    /**
     * Pops a value from the end of the Deque.
     * @throws \UnderflowException if the Deque is empty
     */
    public function pop(): mixed {}
    /**
     * Pops a value from the start of the Deque.
     * @throws \UnderflowException if the Deque is empty
     */
    public function popFront(): mixed {}

    /**
     * Peeks at the value at the start of the Deque.
     * @throws \UnderflowException if the Deque is empty
     */
    public function first(): mixed {}
    /**
     * Peeks at the value at the end of the Deque.
     * @throws \UnderflowException if the Deque is empty
     */
    public function last(): mixed {}

    /**
     * Returns a list of the elements from the start to the end.
     */
    public function toArray(): array {}

    /**
     * Insert 0 or more values at the given offset of the Deque.
     * @throws \OutOfBoundsException if the value of $offset is not within the bounds of this Deque.
     */
    public function insert(int $offset, mixed ...$values): void {}
    // Must be mixed for compatibility with ArrayAccess
    /**
     * Returns the value at offset (int)$offset (relative to the start of the Deque)
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this Deque.
     */
    public function offsetGet(mixed $offset): mixed {}
    /**
     * Returns true if `0 <= (int)$offset && (int)$offset < $this->count()
     * AND the value at that offset is non-null.
     */
    public function offsetExists(mixed $offset): bool {}
    /**
     * Sets the value at offset $offset (relative to the start of the Deque) to $value
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this Deque.
     */
    public function offsetSet(mixed $offset, mixed $value): void {}
    /**
     * Removes the value at (int)$offset from the deque.
     * @throws \OutOfBoundsException if the value of (int)$offset is not within the bounds of this Deque.
     */
    public function offsetUnset(mixed $offset): void {}

    /**
     * This is JSON serialized as a JSON array with elements from the start to the end.
     * @implementation-alias Collections\Deque::toArray
     */
    public function jsonSerialize(): array {}
}
