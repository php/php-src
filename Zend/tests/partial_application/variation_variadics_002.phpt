--TEST--
PFA variation: variadics, internal function
--FILE--
<?php
$sprintf = sprintf("%d %d %d", 100, ...);

echo (string) new ReflectionFunction($sprintf);

echo $sprintf(1000, 10000);
?>
--EXPECTF--
Closure [ <user> static function {closure:%s:%d} ] {
  @@ %svariation_variadics_002.php 2 - 2

  - Bound Variables [2] {
      Variable #0 [ $format ]
      Variable #1 [ $values0 ]
  }

  - Parameters [1] {
    Parameter #0 [ <optional> mixed ...$values ]
  }
  - Return [ string ]
}
100 1000 10000
