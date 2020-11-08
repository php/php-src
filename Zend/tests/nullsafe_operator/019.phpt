--TEST--
Test nullsafe in new
--FILE--
<?php

class Bar {}

class Foo {
    public $bar;
}

$foo = new Foo();
$foo->bar = 'bar';
var_dump(new $foo?->bar);

$foo = null;
var_dump(new $foo?->bar);

?>
--EXPECTF--
object(Bar)#2 (0) {
}

Fatal error: Uncaught Error: Class name must be a valid object or a string in %s.php:14
Stack trace:
#0 {main}
  thrown in %s.php on line 14
