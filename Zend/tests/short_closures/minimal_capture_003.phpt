--TEST--
Short closures minimal capture: goto
--FILE--
<?php

$a = -1;
$b = 2;

// Does not capture $a because it's always defined
$f = fn ($x) {
    goto end;

    ret:
    return $a + $x;

    end:
    $a = $b;
    goto ret;
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
  ["b"]=>
  int(2)
}
f(3):
int(5)
f(0):
int(2)
