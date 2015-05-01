--TEST--
Basic test cases for short Closure syntax
--FILE--
<?php

error_reporting(-1);

$foo = () ~> 1;
var_dump($foo());

var_dump(($a ~> $foo)()());

$n = 2;
var_dump(($a ~> { $foo = $a + $n; return $foo; })(3));

var_dump(((&$a) ~> $a++)($n), $n);

var_dump((($b, ...$a) ~> [$b] + $a)(1, 2, 3));

var_dump(($b ~> $a ~> $a + $b)(1)(2));

var_dump($a ~> $b);

?>
--EXPECTF--
int(1)

Warning: Missing argument 1 for {closure}(), called in %s on line %d and defined in %s on line %d
int(1)
int(5)
int(2)
int(3)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(3)
}
int(3)

Notice: Undefined variable: b in %s on line %d
object(Closure)#%d (2) {
  ["static"]=>
  array(1) {
    ["b"]=>
    NULL
  }
  ["parameter"]=>
  array(1) {
    ["$a"]=>
    string(10) "<required>"
  }
}

