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
Fatal error: Can't use nullsafe operator in write context in %s.php on line 12
