--TEST--
PFA inherits return type
--FILE--
<?php
function foo($a) : array {}

echo (string) new ReflectionFunction(foo(new stdClass, ...));
?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %s 4 - 4

  - Bound Variables [1] {
      Variable #0 [ $a ]
  }

  - Parameters [0] {
  }
  - Return [ array ]
}
