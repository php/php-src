--TEST--
Short closures minimal capture: foreach
--FILE--
<?php

$a = 1;
$x = 1;
$k = 1;
$v = 1;

$f = fn () {
    $ret = [];
    foreach (range(0, $a) as $k => $v) {
        $ret[] = [$k, $v];
    }
    return $ret;
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "f():\n";
var_dump($f());
--EXPECT--
Captures:
array(1) {
  ["a"]=>
  int(1)
}
f():
array(2) {
  [0]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(0)
  }
  [1]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}
