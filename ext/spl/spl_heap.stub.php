<?php

/** @generate-function-entries */

class SplPriorityQueue implements Iterator, Countable
{
    /** @return int */
    public function compare(mixed $priority1, mixed $priority2) {}

    /** @return bool */
    public function insert(mixed $value, mixed $priority) {}

    /** @return int */
    public function setExtractFlags(int $flags) {}

    /** @return mixed */
    public function top() {}

    /** @return mixed */
    public function extract() {}

    /**
     * @return int
     * @implementation-alias SplHeap::count
     */
    public function count() {}

    /**
     * @return bool
     * @implementation-alias SplHeap::isEmpty
     */
    public function isEmpty() {}

    /**
     * @return void
     * @implementation-alias SplHeap::rewind
     */
    public function rewind() {}

    /** @return mixed */
    public function current() {}

    /**
     * @return int
     * @implementation-alias SplHeap::key
     */
    public function key() {}

    /**
     * @return void
     * @implementation-alias SplHeap::next
     */
    public function next() {}

    /**
     * @return bool
     * @implementation-alias SplHeap::valid
     */
    public function valid() {}

    /**
     * @return bool
     * @implementation-alias SplHeap::recoverFromCorruption
     */
    public function recoverFromCorruption() {}

    /**
     * @return bool
     * @implementation-alias SplHeap::isCorrupted
     */
    public function isCorrupted() {}

    /** @return int */
    public function getExtractFlags() {}

    /** @return array */
    public function __debugInfo() {}
}

abstract class SplHeap implements Iterator, Countable
{
    /** @return mixed */
    public function extract() {}

    /** @return bool */
    public function insert(mixed $value) {}

    /** @return mixed */
    public function top() {}

    /** @return int */
    public function count() {}

    /** @return bool */
    public function isEmpty() {}

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

    /** @return bool */
    public function recoverFromCorruption() {}

    /** @return int */
    abstract protected function compare(mixed $value1, mixed $value2);

    /** @return bool */
    public function isCorrupted() {}

    /** @return array */
    public function __debugInfo() {}
}

class SplMinHeap extends SplHeap
{
    /** @return int */
    protected function compare(mixed $value1, mixed $value2) {}
}

class SplMaxHeap extends SplHeap
{
    /** @return int */
    protected function compare(mixed $value1, mixed $value2) {}
}
