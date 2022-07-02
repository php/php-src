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
    function valid(): false
    {
        echo __METHOD__ . "\n";
        return parent::valid();
    }
    function current(): never
    {
        echo __METHOD__ . "\n";
        parent::current();
    }
    function key(): never
    {
        echo __METHOD__ . "\n";
        parent::key();
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
