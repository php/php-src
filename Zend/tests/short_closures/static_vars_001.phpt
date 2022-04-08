--TEST--
Short closures: static vars
--FILE--
<?php

$a = 1;
$b = 2;

$f = fn ($x) {
    static $a;
    static $y;
    return [$a, $b, $x, $y];
};

echo "Captures or statics:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "Result:\n";
var_dump($f(3));
--EXPECT--
Captures or statics:
array(3) {
  ["a"]=>
  NULL
  ["y"]=>
  NULL
  ["b"]=>
  int(2)
}
Result:
array(4) {
  [0]=>
  NULL
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  NULL
}
