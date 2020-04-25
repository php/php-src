<?php

/** @generate-function-entries */

class SplFixedArray implements Iterator, ArrayAccess, Countable
{
    public function __construct(int $size = 0) {}

    /** @return void */
    public function __wakeup() {}

    /** @return int */
    public function count() {}

    /** @return array */
    public function toArray() {}

    /** @return SplFixedArray */
    public static function fromArray(array $array, bool $save_indexes = true) {}

    /** @return int */
    public function getSize() {}

    /** @return bool */
    public function setSize(int $size) {}

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
     * @param int $index
     * @return void
     */
    public function offsetUnset($index) {}

    /** @return void */
    public function rewind() {}

    /** @return mixed */
    public function current() {}

    /** @return int */
    public function key() {}

    /** @return void */
    public function next() {}

    /** @return bool */
    public function valid() {}
}
