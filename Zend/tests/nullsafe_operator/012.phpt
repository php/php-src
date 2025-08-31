--TEST--
Test nullsafe property on reference
--FILE--
<?php

class Foo {
    public $bar;
}

$foo = new Foo();
$foo->bar = 'bar';

$fooRef = &$foo;
var_dump($fooRef?->bar);

?>
--EXPECT--
string(3) "bar"
