<?php

/** @generate-function-entries */

interface SplObserver
{
    /** @return void */
    public function update(SplSubject $subject);
}

interface SplSubject
{
    /** @return void */
    public function attach(SplObserver $observer);

    /** @return void */
    public function detach(SplObserver $observer);

    /** @return void */
    public function notify();
}

class SplObjectStorage implements Countable, Iterator, Serializable, ArrayAccess
{
    /** @return void */
    public function attach(object $object, mixed $info = null) {}

    /** @return void */
    public function detach(object $object) {}

    /** @return bool */
    public function contains(object $object) {}

    /** @return int */
    public function addAll(SplObjectStorage $storage) {}

    /** @return int */
    public function removeAll(SplObjectStorage $storage) {}

    /** @return int */
    public function removeAllExcept(SplObjectStorage $storage) {}

    /** @return mixed */
    public function getInfo() {}

    /** @return void */
    public function setInfo(mixed $info) {}

    /** @return int */
    public function count(int $mode = COUNT_NORMAL) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return int */
    public function key() {}

    /** @return object|null */
    public function current() {}

    /** @return void */
    public function next() {}

    /** @return void */
    public function unserialize(string $data) {}

    /** @return string */
    public function serialize() {}

    /**
     * @param object $object
     * @return bool
     * @implementation-alias SplObjectStorage::contains
     */
    public function offsetExists($object) {}

    /**
     * @param object $object
     * @return mixed
     */
    public function offsetGet($object) {}

    /**
     * @param object $object
     * @return void
     * @implementation-alias SplObjectStorage::attach
     */
    public function offsetSet($object, mixed $info = null) {}

    /**
     * @param object $object
     * @return void
     * @implementation-alias SplObjectStorage::detach
     */
    public function offsetUnset($object) {}

    /** @return string */
    public function getHash(object $object) {}

    /** @return array */
    public function __serialize() {}

    /** @return void */
    public function __unserialize(array $data) {}

    /** @return array */
    public function __debugInfo() {}
}

class MultipleIterator implements Iterator
{
    public function __construct(int $flags = MultipleIterator::MIT_NEED_ALL|MultipleIterator::MIT_KEYS_NUMERIC) {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}

    /** @return void */
    public function attachIterator(Iterator $iterator, string|int|null $info = null) {}

    /** @return void */
    public function detachIterator(Iterator $iterator) {}

    /** @return bool */
    public function containsIterator(Iterator $iterator) {}

    /** @return int */
    public function countIterators() {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return array|false */
    public function key() {}

    /** @return array|false */
    public function current() {}

    /** @return void */
    public function next() {}

    /**
     * @return array
     * @implementation-alias SplObjectStorage::__debugInfo
     */
    public function __debugInfo() {}
}
