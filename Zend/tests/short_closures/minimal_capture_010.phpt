--TEST--
Short closures minimal capture: var args
--FILE--
<?php

$a = [1];
$x = 1;

$f = fn (...$a) {
    $x = 2;
    return $x + count($a);
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "f(0,1):\n";
var_dump($f(0,1));
--EXPECT--
Captures:
array(0) {
}
f(0,1):
int(4)
