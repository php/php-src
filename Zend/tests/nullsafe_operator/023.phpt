--TEST--
Foreach by reference on nullsafe
--FILE--
<?php

class Foo {
    public $bar;
}

$foo = new Foo();

foreach ($foo?->bar as &$value) {
    var_dump($value);
}

$foo->bar = [42];
foreach ($foo?->bar as &$value) {
    var_dump($value);
    $value++;
}
var_dump($foo->bar);

?>
--EXPECTF--
Warning: foreach() argument must be of type array|object, null given in %s on line %d
int(42)
array(1) {
  [0]=>
  &int(43)
}
