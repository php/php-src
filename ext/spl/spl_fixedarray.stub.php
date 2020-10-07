<?php

/** @generate-function-entries */

class SplFixedArray implements IteratorAggregate, ArrayAccess, Countable
{
    public function __construct(int $size = 0) {}

    /** @return void */
    public function __wakeup() {}

    /** @return int */
    public function count() {}

    /** @return array */
    public function toArray() {}

    /** @return SplFixedArray */
    public static function fromArray(array $array, bool $preserveKeys = true) {}

    /** @return int */
    public function getSize() {}

    /** @return bool */
    public function setSize(int $size) {}

    /**
     * @param int $index
     * @return bool
     */
    public function offsetExists($index) {}

    /**
     * @param int $index
     * @return mixed
     */
    public function offsetGet($index) {}

    /**
     * @param int $index
     * @return void
     */
    public function offsetSet($index, mixed $value) {}

    /**
     * @param int $index
     * @return void
     */
    public function offsetUnset($index) {}

    public function getIterator(): Iterator {}
}
