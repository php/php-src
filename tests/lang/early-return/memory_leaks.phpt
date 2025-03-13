--TEST--
Memory leaks
--FILE--
<?php
class Foo {}

function test() {
    new Foo() + return;
}

test();

?>
--EXPECT--

