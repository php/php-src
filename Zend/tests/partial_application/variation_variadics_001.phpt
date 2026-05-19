--TEST--
PFA variation: variadics, user function
--FILE--
<?php
function foo($a, ...$b) {
    var_dump($a, ...$b);
}

$foo = foo(10, 100, ...);

echo (string) new ReflectionFunction($foo);

$foo(1000, 10000);
?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %s 6 - 6

  - Bound Variables [2] {
      Variable #0 [ $a ]
      Variable #1 [ $b0 ]
  }

  - Parameters [1] {
    Parameter #0 [ <optional> ...$b ]
  }
}
int(10)
int(100)
int(1000)
int(10000)
