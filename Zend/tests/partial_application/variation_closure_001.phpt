--TEST--
PFA variation: Closure
--FILE--
<?php
$closure = function($a, $b) {
};

echo (string) new ReflectionFunction($closure(1, ?));

$closure2 = function($a, $fn) {
};

echo (string) new ReflectionFunction($closure2(1, ?));

?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %s 5 - 5

  - Bound Variables [2] {
      Variable #0 [ $fn ]
      Variable #1 [ $a ]
  }

  - Parameters [1] {
    Parameter #0 [ <required> $b ]
  }
}
Closure [ <user> static function {closure:pfa:%s:%d} ] {
  @@ %s 10 - 10

  - Bound Variables [2] {
      Variable #0 [ $fn2 ]
      Variable #1 [ $a ]
  }

  - Parameters [1] {
    Parameter #0 [ <required> $fn ]
  }
}
