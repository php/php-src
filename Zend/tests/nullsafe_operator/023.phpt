--TEST--
Foreach by reference on nullsafe
--FILE--
<?php

class Foo {
    public $bar;
}

$foo = new Foo();

try {
    foreach ($foo?->bar as &$value) {
        var_dump($value);
    }
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

// Don't convert $foo->bar into a reference.
$foo->bar = [42];
foreach ($foo?->bar as &$value) {
    var_dump($value);
    $value++;
}
var_dump($foo->bar);

// But respect interior references.
$ref =& $foo->bar[0];
foreach ($foo?->bar as &$value) {
    var_dump($value);
    $value++;
}
var_dump($foo->bar);

?>
--EXPECT--
foreach() argument must be of type array|object, null given
int(42)
array(1) {
  [0]=>
  int(42)
}
int(42)
array(1) {
  [0]=>
  &int(43)
}
