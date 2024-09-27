--TEST--
GH-15851: Access on NULL when printing backtrace with freed generator
--FILE--
<?php

class Foo {
    public function __destruct() {
        debug_print_backtrace();
    }
}

function bar() {
    yield from foo();
}

function foo() {
    $foo = new Foo();
    yield;
}

$gen = bar();
foreach ($gen as $dummy);

?>
--EXPECTF--
#0 %s(%d): Foo->__destruct()
#1 %s(%d): bar()
