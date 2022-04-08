--TEST--
Short closures minimal capture: args
--FILE--
<?php

$a = 1;
$x = 1;

$f = fn ($a) {
    $x = 2;
    return $x + $a;
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "f(2):\n";
var_dump($f(2));
--EXPECT--
Captures:
array(0) {
}
f(2):
int(4)
