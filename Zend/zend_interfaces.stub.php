<?php

interface Traversable {}

interface IteratorAggregate extends Traversable
{
    /** @return Traversable */
    function getIterator();
}

interface Iterator extends Traversable
{
    /** @return mixed */
    function current();

    /** @return void */
    function next();

    /** @return mixed */
    function key();

    /** @return bool */
    function valid();

    /** @return void */
    function rewind();
}

interface ArrayAccess
{
    /** @return bool */
    function offsetExists($offset);

    /* actually this should be return by ref but atm cannot be */
    /** @return mixed */
    function offsetGet($offset);

    /** @return void */
    function offsetSet($offset, $value);

    /** @return void */
    function offsetUnset($offset);
}

interface Serializable
{
    /** @return string */
    function serialize();

    /** @return void */
    function unserialize(string $serialized);
}

interface Countable
{
    /** @return int */
    function count();
}

interface Stringable
{
    function __toString(): string;
}
