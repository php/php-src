--TEST--
Short closures minimal capture: simple case
--FILE--
<?php

$a = 1;
$x = 1;

$f = fn () {
    $x = 2;
    return $x + $a;
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "f():\n";
var_dump($f());
--EXPECT--
Captures:
array(1) {
  ["a"]=>
  int(1)
}
f():
int(3)
