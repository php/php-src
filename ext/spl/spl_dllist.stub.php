<?php

/** @generate-function-entries */

class SplDoublyLinkedList implements Iterator, Countable, ArrayAccess, Serializable
{
    /** @return void */
    public function add(int $index, mixed $value) {}

    /** @return mixed */
    public function pop() {}

    /** @return mixed */
    public function shift() {}

    /** @return void */
    public function push(mixed $value) {}

    /** @return void */
    public function unshift(mixed $value) {}

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
     * @param int $index
     * @return mixed
     */
    public function offsetGet($index) {}

    /**
     * @param int|null $index
     * @return void
     */
    public function offsetSet($index, mixed $value) {}

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
    public function prev() {}

    /** @return void */
    public function next() {}

    /** @return bool */
    public function valid() {}

    /** @return void */
    public function unserialize(string $data) {}

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
     * @return void
     * @implementation-alias SplDoublyLinkedList::push
     */
    public function enqueue(mixed $value) {}

    /**
     * @return mixed
     * @implementation-alias SplDoublyLinkedList::shift
     */
    public function dequeue() {}
}

class SplStack extends SplDoublyLinkedList
{
}
