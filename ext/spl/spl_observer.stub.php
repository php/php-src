<?php

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
    /**
     * @param mixed $info
     * @return void
     */
    public function attach(object $object, $info = null) {}

    /** @return void */
    public function detach(object $object) {}

    /** @return bool */
    public function contains(object $object) {}

    /** @return int */
    public function addAll(SplObjectStorage $storage) {}

    /** @return void */
    public function removeAll(SplObjectStorage $storage) {}

    /** @return void */
    public function removeAllExcept(SplObjectStorage $storage) {}

    /** @return mixed */
    public function getInfo() {}

    /**
     * @param mixed $info
     * @return void
     */
    public function setInfo($info) {}

    /** @return int */
    public function count(int $mode = COUNT_NORMAL) {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return int */
    public function key() {}

    /** @return object */
    public function current() {}

    /** @return void */
    public function next() {}

    /** @return void */
    public function unserialize(string $serialized) {}

    /** @return string|null */
    public function serialize() {}

    /**
     * @param object $object
     * @return bool
     */
    public function offsetExists($object) {}

    /**
     * @param object $object
     * @return string
     */
    public function offsetGet($object) {}

    /**
     * @param object $object
     * @param mixed $info
     * @return void
     */
    public function offsetSet($object, $info = null) {}

    /**
     * @param object $object
     * @return void
     */
    public function offsetUnset($object) {}

    /** @return string */
    public function getHash(object $object) {}

    /** @return array */
    public function __serialize() {}

    /** @return void */
    public function __unserialize(array $data) {}
}

class MultipleIterator implements Iterator
{
    public function __construct(int $flags = MultipleIterator::MIT_NEED_ALL|MultipleIterator::MIT_KEYS_NUMERIC) {}

    /** @return int */
    public function getFlags() {}

    /** @return void */
    public function setFlags(int $flags) {}

    /**
    * @param Iterator $iterator
    * @param int|string|null $info
    * @return void
    */
    public function attachIterator(Iterator $iterator, $info = null) {}

    /** @return void */
    public function detachIterator(object $iterator) {}

    /** @return bool */
    public function containsIterator(object $iterator) {}

    /** @return int */
    public function countIterators() {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return array */
    public function key() {}

    /** @return array */
    public function current() {}

    /** @return void */
    public function next() {}
}
