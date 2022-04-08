--TEST--
Short closures: explicit use
--FILE--
<?php

$a = 1;
$b = 2;

$f = fn ($x) use ($a, &$b) {
    $b = 4;
    return [$a, $b, $x];
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "Result:\n";
var_dump($f(3));

echo "\$b:\n";
var_dump($b);
--EXPECT--
Captures:
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  &int(2)
}
Result:
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  int(3)
}
$b:
int(4)
