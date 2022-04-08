--TEST--
Short closures
--FILE--
<?php

$a = 1;
$b = 2;

$f = fn ($x) {
    return [$a, $b, $x];
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "Result:\n";
var_dump($f(3));
--EXPECT--
Captures:
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
}
Result:
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
