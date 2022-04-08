--TEST--
Short closures minimal capture: assign ref
--FILE--
<?php

$a = 1;
$x = 1;

$f = fn () {
    $b = 2;
    $x = &$b;
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
