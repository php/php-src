<?php

/** @generate-class-entries */

interface ArrayAccess
{
    /** @tentative-return-type */
    public function offsetExists(mixed $offset): bool;

    /**
     * Actually this should be return by ref but atm cannot be.
     * @tentative-return-type
     */
    public function offsetGet(mixed $offset): mixed;

    /** @tentative-return-type */
    public function offsetSet(mixed $offset, mixed $value): void;

    /** @tentative-return-type */
    public function offsetUnset(mixed $offset): void;
}

interface DimensionReadable
{
    public function offsetGet(mixed $offset): mixed;

    public function offsetExists(mixed $offset): bool;
}

interface DimensionFetchable extends DimensionReadable
{
    public function &offsetFetch(mixed $offset): mixed;
}

interface DimensionWritable
{
    public function offsetSet(mixed $offset, mixed $value): void;
}

interface DimensionUnsetable
{
    public function offsetUnset(mixed $offset): void;
}

interface Appendable
{
    public function append(mixed $value): void;
}

interface FetchAppendable extends Appendable
{
    public function &fetchAppend(): mixed;
}
