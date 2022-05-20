--TEST--
Short closures minimal capture: foreach variation
--FILE--
<?php

$a = 1;
$k = 2;
$v = 3;

$f = fn () {
    $ret = [];
    foreach (range(0, $a) as $k => $v) {
        $ret[] = [$k, $v];
    }
    return $k + $v;
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

echo "f():\n";
var_dump($f());
--EXPECT--
Captures:
array(3) {
  ["a"]=>
  int(1)
  ["v"]=>
  int(3)
  ["k"]=>
  int(2)
}
f():
int(2)
