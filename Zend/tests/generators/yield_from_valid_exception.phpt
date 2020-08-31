--TEST--
Exception from valid() during yield from
--FILE--
<?php

class FooBar implements Iterator {
    function rewind() {}
    function current() {}
    function key() {}
    function next() {}
    function valid() {
        throw new Exception("Exception from valid()");
    }
}

function gen() {
    try {
        yield from new FooBar;
    } catch (Exception $e) {
        echo $e->getMessage(), "\n";
    }
}

$x = gen();
$x->current();

?>
--EXPECT--
Exception from valid()
