<?php

/** @generate-class-entries */

interface SplObserver
{
    /** @tentative-return-type */
    public function update(SplSubject $subject): void;
}

interface SplSubject
{
    /** @tentative-return-type */
    public function attach(SplObserver $observer): void;

    /** @tentative-return-type */
    public function detach(SplObserver $observer): void;

    /** @tentative-return-type */
    public function notify(): void;
}

class SplObjectStorage implements Countable, Iterator, Serializable, ArrayAccess
{
    /** @tentative-return-type */
    public function attach(object $object, mixed $info = null): void {}

    /** @tentative-return-type */
    public function detach(object $object): void {}

    /** @tentative-return-type */
    public function contains(object $object): bool {}

    /** @tentative-return-type */
    public function addAll(SplObjectStorage $storage): int {}

    /** @tentative-return-type */
    public function removeAll(SplObjectStorage $storage): int {}

    /** @tentative-return-type */
    public function removeAllExcept(SplObjectStorage $storage): int {}

    /** @tentative-return-type */
    public function getInfo(): mixed {}

    /** @tentative-return-type */
    public function setInfo(mixed $info): void {}

    /** @tentative-return-type */
    public function count(int $mode = COUNT_NORMAL): int {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function key(): int {}

    /** @tentative-return-type */
    public function current(): object {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function unserialize(string $data): void {}

    /** @tentative-return-type */
    public function serialize(): string {}

    /**
     * @param object $object
     * @tentative-return-type
     * @implementation-alias SplObjectStorage::contains
     * @no-verify Cannot specify arg type because ArrayAccess does not
     */
    public function offsetExists($object): bool {}

    /**
     * @param object $object
     * @tentative-return-type
     */
    public function offsetGet($object): mixed {}

    /**
     * @param object $object
     * @tentative-return-type
     * @implementation-alias SplObjectStorage::attach
     * @no-verify Cannot specify arg type because ArrayAccess does not
     */
    public function offsetSet($object, mixed $info = null): void {}

    /**
     * @param object $object
     * @tentative-return-type
     * @implementation-alias SplObjectStorage::detach
     * @no-verify Cannot specify arg type because ArrayAccess does not
     */
    public function offsetUnset($object): void {}

    /** @tentative-return-type */
    public function getHash(object $object): string {}

    /** @tentative-return-type */
    public function __serialize(): array {}

    /** @tentative-return-type */
    public function __unserialize(array $data): void {}

    /** @tentative-return-type */
    public function __debugInfo(): array {}
}

class MultipleIterator implements Iterator
{
    /**
     * @var int
     * @cvalue MIT_NEED_ANY
     */
    public const MIT_NEED_ANY = UNKNOWN;
    /**
     * @var int
     * @cvalue MIT_NEED_ALL
     */
    public const MIT_NEED_ALL = UNKNOWN;
    /**
     * @var int
     * @cvalue MIT_KEYS_NUMERIC
     */
    public const MIT_KEYS_NUMERIC = UNKNOWN;
    /**
     * @var int
     * @cvalue MIT_KEYS_ASSOC
     */
    public const MIT_KEYS_ASSOC = UNKNOWN;

    public function __construct(int $flags = MultipleIterator::MIT_NEED_ALL|MultipleIterator::MIT_KEYS_NUMERIC) {}

    /** @tentative-return-type */
    public function getFlags(): int {}

    /** @tentative-return-type */
    public function setFlags(int $flags): void {}

    /** @tentative-return-type */
    public function attachIterator(Iterator $iterator, string|int|null $info = null): void {}

    /** @tentative-return-type */
    public function detachIterator(Iterator $iterator): void {}

    /** @tentative-return-type */
    public function containsIterator(Iterator $iterator): bool {}

    /** @tentative-return-type */
    public function countIterators(): int {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function key(): array {}

    /** @tentative-return-type */
    public function current(): array {}

    /** @tentative-return-type */
    public function next(): void {}

    /**
     * @tentative-return-type
     * @implementation-alias SplObjectStorage::__debugInfo
     */
    public function __debugInfo(): array {}
}
