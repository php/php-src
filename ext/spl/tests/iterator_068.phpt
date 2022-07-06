--TEST--
SPL: Iterator: Overloaded object and destruction
--FILE--
<?php

class Test implements Iterator {
    function foo() {
        echo __METHOD__ . "()\n";
    }
    function rewind() {}
    function valid() {}
    function current() {}
    function key() {}
    function next() {}
}

class TestIteratorIterator extends IteratorIterator {
    function __destruct() {
        echo __METHOD__ . "()\n";
        $this->foo();
    }
}

$obj = new TestIteratorIterator(new Test);
$obj->foo();
unset($obj);

?>
--EXPECT--
Test::foo()
TestIteratorIterator::__destruct()
Test::foo()
