<?php

class ArrayObject implements IteratorAggregate, ArrayAccess, Countable
{
    /** @param array|object $input */
    public function __construct($input = [], int $flags = 0, string $iterator_class = ArrayIterator::class) {}

    /**
     * @param mixed $index
     * @return bool
     */
    public function offsetExists($index) {}

    /**
     * @param mixed $index
     * @return mixed
     */
    public function offsetGet($index) {}

    /**
     * @param mixed $index
     * @param mixed $value
     * @return void
     */
    public function offsetSet($index, $value) {}

    /**
     * @param mixed $index
     * @return void
     */
    public function offsetUnset($index) {}

    /**
     * @param mixed $value
     * @return void
     */
    public function append($value) {}

    /** @return array */
    public function getArrayCopy() {}

    /** @return int */
    public function count() {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}

    /**
     * @param int $sort_flags
     * @return bool
     */
    public function asort($sort_flags = SORT_REGULAR) {}

    /**
     * @param int $sort_flags
     * @return bool
     */
    public function ksort($sort_flags = SORT_REGULAR) {}

    /**
     * @param callback $cmp_function
     * @return bool
     */
    public function uasort($cmp_function) {}

    /**
     * @param callback $cmp_function
     * @return bool
     */
    public function uksort($cmp_function) {}

    /** @return bool */
    public function natsort() {}

    /** @return bool */
    public function natcasesort() {}

    /** @return array */
    public function __serialize() {}

    /** @return void */
    public function __unserialize(array $data) {}

    /** @return Iterator */
    public function getIterator() {}

    /**
     * @param array|object $input
     * @return array|null
     */
    public function exchangeArray($input) {}

    /** @return void */
    public function setIteratorClass(string $iteratorClass) {}

    /** @return string */
    public function getIteratorClass() {}

    /** @return array */
    public function __debugInfo() {}
}

class ArrayIterator implements SeekableIterator, ArrayAccess, Countable
{
    /** @param array|object $array */
    public function __construct($array = [], int $flags = 0) {}

    /**
     * @param mixed $index
     * @return bool
     */
    public function offsetExists($index) {}

    /**
     * @param mixed $index
     * @return mixed
     */
    public function offsetGet($index) {}

    /**
     * @param mixed $index
     * @param mixed $value
     * @return void
     */
    public function offsetSet($index, $value) {}

    /**
     * @param mixed $index
     * @return void
     */
    public function offsetUnset($index) {}

    /**
     * @param mixed $value
     * @return void
     */
    public function append($value) {}

    /** @return array */
    public function getArrayCopy() {}

    /** @return int */
    public function count() {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}

    /**
     * @param int $sort_flags
     * @return bool
     */
    public function asort($sort_flags = SORT_REGULAR) {}

    /**
     * @param int $sort_flags
     * @return bool
     */
    public function ksort($sort_flags = SORT_REGULAR) {}

    /**
     * @param callback $cmp_function
     * @return bool
     */
    public function uasort($cmp_function) {}

    /**
     * @param callback $cmp_function
     * @return bool
     */
    public function uksort($cmp_function) {}

    /** @return bool */
    public function natsort() {}

    /** @return bool */
    public function natcasesort() {}

    /** @return array */
    public function __serialize() {}

    /** @return void */
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
    public function seek(int $position) {}

    /** @return array */
    public function __debugInfo() {}
}

class RecursiveArrayIterator extends ArrayIterator implements RecursiveIterator
{
    /** @return bool */
    public function hasChildren() {}

    /** @return RecursiveArrayIterator|null */
    public function getChildren() {}
}
