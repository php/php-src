<?php

final class Generator implements Iterator
{
    function rewind(): void {}

    function valid(): bool {}

    /** @return mixed */
    function current() {}

    /** @return mixed */
    function key() {}

    function next(): void {}

    /** @return mixed */
    function send($value) {}

    /** @return mixed */
    function throw(Throwable $exception) {}

    /** @return mixed */
    function getReturn() {}
}
