<?php

interface Traversable {}

interface IteratorAggregate extends Traversable
{
    /** @return Traversable */
    function getIterator();
}

interface Iterator extends Traversable
{
    function current();

    /** @return void */
    function next();

    function key();

    /** @return bool */
    function valid();

    /** @return void */
    function rewind();
}

interface ArrayAccess
{
    function offsetExists($offset);

    /* actually this should be return by ref but atm cannot be */
    function offsetGet($offset);

    function offsetSet($offset, $value);

    function offsetUnset($offset);
}

interface Serializable
{
    /** @return string */
    function serialize();

    function unserialize(string $serialized);
}

interface Countable
{
    /** @return int */
    function count();
}
