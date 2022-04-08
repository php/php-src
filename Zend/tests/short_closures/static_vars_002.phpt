--TEST--
Short closures: static vs capture precedence
--FILE--
<?php

$a = 1;
$b = 2;

// static $a is initialized with explicitly used $a, as in long closures
// $b is not auto-captured because it is defined before any use
$f = fn () use ($a) {
    static $a;
    static $b;
    $ret = [$a, $b];
    $a = 1 + (int) $a;
    $b = 1 + (int) $b;
    return $ret;
};

echo "Captures or statics:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "Results:\n";
var_dump($f());
var_dump($f());
--EXPECT--
Captures or statics:
array(2) {
  ["a"]=>
  int(1)
  ["b"]=>
  NULL
}
Results:
array(2) {
  [0]=>
  int(1)
  [1]=>
  NULL
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(1)
}
