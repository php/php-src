--TEST--
SPL: EmptyIterator
--FILE--
<?php

class EmptyIteratorEx extends EmptyIterator
{
    function rewind(): void
    {
        echo __METHOD__ . "\n";
        parent::rewind();
    }
    function valid(): bool
    {
        echo __METHOD__ . "\n";
        return parent::valid();
    }
    function current(): mixed
    {
        echo __METHOD__ . "\n";
        return parent::current();
    }
    function key(): mixed
    {
        echo __METHOD__ . "\n";
        return parent::key();
    }
    function next(): void
    {
        echo __METHOD__ . "\n";
        parent::next();
    }
}

foreach (new EmptyIteratorEx() as $v) {
    var_dump($v);
}

?>
--EXPECT--
EmptyIteratorEx::rewind
EmptyIteratorEx::valid
