<?php

/** @generate-function-entries */

class ArrayObject implements IteratorAggregate, ArrayAccess, Serializable, Countable
{
    public function __construct(array|object $array = [], int $flags = 0, string $iteratorClass = ArrayIterator::class) {}

    /** @return bool */
    public function offsetExists(mixed $key) {}

    /** @return mixed */
    public function offsetGet(mixed $key) {}

    /** @return void */
    public function offsetSet(mixed $key, mixed $value) {}

    /** @return void */
    public function offsetUnset(mixed $key) {}

    /** @return void */
    public function append(mixed $value) {}

    /** @return array */
    public function getArrayCopy() {}

    /** @return int */
    public function count() {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}

    /** @return bool */
    public function asort(int $flags = SORT_REGULAR) {}

    /** @return bool */
    public function ksort(int $flags = SORT_REGULAR) {}

    /** @return bool */
    public function uasort(callable $callback) {}

    /** @return bool */
    public function uksort(callable $callback) {}

    /** @return bool */
    public function natsort() {}

    /** @return bool */
    public function natcasesort() {}

    /** @return void */
    public function unserialize(string $data) {}

    /** @return string */
    public function serialize() {}

    /** @return array */
    public function __serialize() {}

    /** @return void */
    public function __unserialize(array $data) {}

    /** @return Iterator */
    public function getIterator() {}

    /** @return array */
    public function exchangeArray(array|object $array) {}

    /** @return void */
    public function setIteratorClass(string $iteratorClass) {}

    /** @return string */
    public function getIteratorClass() {}

    /** @return array */
    public function __debugInfo() {}
}

class ArrayIterator implements SeekableIterator, ArrayAccess, Serializable, Countable
{
    public function __construct(array|object $array = [], int $flags = 0) {}

    /**
     * @return bool
     * @implementation-alias ArrayObject::offsetExists
     */
    public function offsetExists(mixed $key) {}

    /**
     * @return mixed
     * @implementation-alias ArrayObject::offsetGet
     */
    public function offsetGet(mixed $key) {}

    /**
     * @return void
     * @implementation-alias ArrayObject::offsetSet
     */
    public function offsetSet(mixed $key, mixed $value) {}

    /**
     * @return void
     * @implementation-alias ArrayObject::offsetUnset
     */
    public function offsetUnset(mixed $key) {}

    /**
     * @return void
     * @implementation-alias ArrayObject::append
     */
    public function append(mixed $value) {}

    /**
     * @return array
     * @implementation-alias ArrayObject::getArrayCopy
     */
    public function getArrayCopy() {}

    /**
     * @return int
     * @implementation-alias ArrayObject::count
     */
    public function count() {}

    /**
     * @return int
     * @implementation-alias ArrayObject::getFlags
     */
    public function getFlags() {}

    /**
     * @return void
     * @implementation-alias ArrayObject::setFlags
     */
    public function setFlags(int $flags) {}

    /**
     * @return bool
     * @implementation-alias ArrayObject::asort
     */
    public function asort(int $flags = SORT_REGULAR) {}

    /**
     * @return bool
     * @implementation-alias ArrayObject::ksort
     */
    public function ksort(int $flags = SORT_REGULAR) {}

    /**
     * @return bool
     * @implementation-alias ArrayObject::uasort
     */
    public function uasort(callable $callback) {}

    /**
     * @return bool
     * @implementation-alias ArrayObject::uksort
     */
    public function uksort(callable $callback) {}

    /**
     * @return bool
     * @implementation-alias ArrayObject::natsort
     */
    public function natsort() {}

    /**
     * @return bool
     * @implementation-alias ArrayObject::natcasesort
     */
    public function natcasesort() {}

    /**
     * @return void
     * @implementation-alias ArrayObject::unserialize
     */
    public function unserialize(string $data) {}

    /**
     * @return string
     * @implementation-alias ArrayObject::serialize
     */
    public function serialize() {}

    /**
     * @return array
     * @implementation-alias ArrayObject::__serialize
     */
    public function __serialize() {}

    /**
     * @return void
     * @implementation-alias ArrayObject::__unserialize
     */
    public function __unserialize(array $data) {}

    /** @return void */
    public function rewind() {}

    /** @return mixed */
    public function current() {}

    /** @return mixed */
    public function key() {}

    /** @return void */
    public function next() {}

    /** @return bool */
    public function valid() {}

    /** @return void */
    public function seek(int $offset) {}

    /**
     * @return array
     * @implementation-alias ArrayObject::__debugInfo
     */
    public function __debugInfo() {}
}

class RecursiveArrayIterator extends ArrayIterator implements RecursiveIterator
{
    /** @return bool */
    public function hasChildren() {}

    /** @return RecursiveArrayIterator|null */
    public function getChildren() {}
}
