--TEST--
Aborted yield during object instantiation
--FILE--
<?php

class Foo {
    public function __construct() {}
}

function gen() {
    $x = new Foo(yield);
}

gen()->rewind();

?>
===DONE===
--EXPECT--
===DONE===
