--TEST--
Short closures minimal capture: unset
--FILE--
<?php

$a = 1;
$x = 1;

$f = fn () {
    $x = 2;
    unset($a);
    return $x + ($a ?? 0);
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "f():\n";
var_dump($f());
--EXPECT--
Captures:
array(0) {
}
f():
int(2)
