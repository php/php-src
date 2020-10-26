--TEST--
Test nullsafe lhs of assignment to nested property chain
--FILE--
<?php

class Foo {
    public $bar;
}

function bar() {
    var_dump('called');
}

$foo = null;
$foo?->bar->baz = bar();

?>
--EXPECTF--
Fatal error: Cannot use the null-safe operator in write context in %s on line %d
