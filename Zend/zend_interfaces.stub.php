<?php

/** @generate-function-entries */

interface Traversable {}

interface IteratorAggregate extends Traversable
{
    /** @return Traversable */
    public function getIterator();
}

interface Iterator extends Traversable
{
    /** @return mixed */
    public function current();

    /** @return void */
    public function next();

    /** @return mixed */
    public function key();

    /** @return bool */
    public function valid();

    /** @return void */
    public function rewind();
}

interface ArrayAccess
{
    /** @return bool */
    public function offsetExists($offset);

    /* actually this should be return by ref but atm cannot be */
    /** @return mixed */
    public function offsetGet($offset);

    /** @return void */
    public function offsetSet($offset, $value);

    /** @return void */
    public function offsetUnset($offset);
}

interface Serializable
{
    /** @return string */
    public function serialize();

    /** @return void */
    public function unserialize(string $serialized);
}

interface Countable
{
    /** @return int */
    public function count();
}

interface Stringable
{
    public function __toString(): string;
}
