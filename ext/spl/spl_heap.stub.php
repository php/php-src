<?php

/** @generate-function-entries */

class SplPriorityQueue implements Iterator, Countable
{
    /**
     * @param mixed $priority1
     * @param mixed $priority2
     * @return int
     */
    public function compare($priority1, $priority2) {}

    /**
     * @param mixed $value
     * @param mixed $priority
     * @return bool
     */
    public function insert($value, $priority) {}

    /** @return int */
    public function setExtractFlags(int $flags) {}

    /** @return mixed */
    public function top() {}

    /** @return mixed */
    public function extract() {}

    /**
     * @return int
     * @alias SplHeap::count
     */
    public function count() {}

    /**
     * @return bool
     * @alias SplHeap::isEmpty
     */
    public function isEmpty() {}

    /**
     * @return void
     * @alias SplHeap::rewind
     */
    public function rewind() {}

    /** @return mixed */
    public function current() {}

    /**
     * @return int
     * @alias SplHeap::key
     */
    public function key() {}

    /**
     * @return void
     * @alias SplHeap::next
     */
    public function next() {}

    /**
     * @return bool
     * @alias SplHeap::valid
     */
    public function valid() {}

    /**
     * @return bool
     * @alias SplHeap::recoverFromCorruption
     */
    public function recoverFromCorruption() {}

    /**
     * @return bool
     * @alias SplHeap::isCorrupted
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

    /**
     * @param mixed $value
     * @return bool
     */
    public function insert($value) {}

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

    /**
     * @param mixed $value1
     * @param mixed $value2
     * @return int
     */
    abstract protected function compare($value1, $value2);

    /** @return bool */
    public function isCorrupted() {}

    /** @return array */
    public function __debugInfo() {}
}

class SplMinHeap extends SplHeap
{
    /**
     * @param mixed $value1
     * @param mixed $value2
     * @return int
     */
    protected function compare($value1, $value2) {}
}

class SplMaxHeap extends SplHeap
{
    /**
     * @param mixed $value1
     * @param mixed $value2
     * @return int
     */
    protected function compare($value1, $value2) {}
}
