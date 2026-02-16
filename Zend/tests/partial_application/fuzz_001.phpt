--TEST--
Closure application fuzz 001
--FILE--
<?php
$closure = function($a, $b) {};

echo (string) new ReflectionFunction($closure(1, ?));
?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %s 4 - 4

  - Bound Variables [2] {
      Variable #0 [ $fn ]
      Variable #1 [ $a ]
  }

  - Parameters [1] {
    Parameter #0 [ <required> $b ]
  }
}
