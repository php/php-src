--TEST--
Short closures: nested functions
--FILE--
<?php

$a = 1;
$b = 2;
$c = 3;
$d = 4;
$e = 5;

$f = fn ($x) {
    $g = fn ($x) { return $a; };
    $h = fn () => $c;
    $i = function () use ($d, &$e) {};
};

echo "Captures:\n";
var_dump((new ReflectionFunction($f))->getStaticVariables());

--EXPECT--
Captures:
array(4) {
  ["a"]=>
  int(1)
  ["c"]=>
  int(3)
  ["d"]=>
  int(4)
  ["e"]=>
  int(5)
}
