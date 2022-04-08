--TEST--
Short closures minimal capture: loop
--FILE--
<?php

$a = -1;
$b = 2;

// Captures $a because it may be used before definition
$f = fn ($x) {
    for ($i = 0; $i < $x; $i++) {
        $a = $i;
    }
    return $a;
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "f(3):\n";
var_dump($f(3));

echo "f(0):\n";
var_dump($f(0));
--EXPECT--
Captures:
array(1) {
  ["a"]=>
  int(-1)
}
f(3):
int(2)
f(0):
int(-1)
