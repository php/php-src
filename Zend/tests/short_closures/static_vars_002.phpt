--TEST--
Short closures: static vs capture precedence
--FILE--
<?php

$a = 1;
$b = 2;

// $a and $b are not auto-captured because they are defined before any use
$f = fn () {
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
  NULL
  ["b"]=>
  NULL
}
Results:
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(1)
}
