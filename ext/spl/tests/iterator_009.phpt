--TEST--
SPL: EmptyIterator
--FILE--
<?php

class EmptyIteratorEx extends EmptyIterator
{
    function rewind()
    {
        echo __METHOD__ . "\n";
        parent::rewind();
    }
    function valid()
    {
        echo __METHOD__ . "\n";
        return parent::valid();
    }
    function current()
    {
        echo __METHOD__ . "\n";
        return parent::current();
    }
    function key()
    {
        echo __METHOD__ . "\n";
        return parent::key();
    }
    function next()
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
