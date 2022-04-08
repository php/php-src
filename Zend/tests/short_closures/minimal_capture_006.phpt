--TEST--
Short closures minimal capture: global
--FILE--
<?php

$a = 1;

$f = fn () {
    global $a;
    return $a;
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
int(1)
