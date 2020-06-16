<?php

/** @generate-function-entries */

class SplDoublyLinkedList implements Iterator, Countable, ArrayAccess, Serializable
{
    /**
     * @param mixed $index
     * @param mixed $value
     * @return void
     */
    public function add($index, $value) {}

    /** @return mixed */
    public function pop() {}

    /** @return mixed */
    public function shift() {}

    /**
     * @param mixed $value
     * @return void
     */
    public function push($value) {}

    /**
     * @param mixed $value
     * @return void
     */
    public function unshift($value) {}

    /** @return mixed */
    public function top() {}

    /** @return mixed */
    public function bottom() {}

    /** @return array */
    public function __debugInfo() {}

    /** @return int */
    public function count() {}

    /** @return bool */
    public function isEmpty() {}

    /** @return int */
    public function setIteratorMode(int $mode) {}

    /** @return int */
    public function getIteratorMode() {}

    /**
     * @param int $index
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

    /** @return void */
    public function rewind() {}

    /** @return mixed */
    public function current() {}

    /** @return int */
    public function key() {}

    /** @return void */
    public function prev() {}

    /** @return void */
    public function next() {}

    /** @return bool */
    public function valid() {}

    /** @return void */
    public function unserialize(string $serialized) {}

    /** @return string */
    public function serialize() {}

    /** @return array */
    public function __serialize() {}

    /** @return void */
    public function __unserialize(array $data) {}
}

class SplQueue extends SplDoublyLinkedList
{
    /**
     * @param mixed $value
     * @return void
     * @alias SplDoublyLinkedList::push
     */
    public function enqueue($value) {}

    /**
     * @return mixed
     * @alias SplDoublyLinkedList::shift
     */
    public function dequeue() {}
}

class SplStack extends SplDoublyLinkedList
{
}
