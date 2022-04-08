--TEST--
Short closures minimal capture: static
--FILE--
<?php

$a = 1;
$x = 1;

$f = fn () {
    static $a;
    $x = 2;
    return $x + (int) $a;
};

echo "Captures or statics:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "f():\n";
var_dump($f(3));
--EXPECT--
Captures or statics:
array(1) {
  ["a"]=>
  NULL
}
f():
int(2)
