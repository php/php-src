--TEST--
ReflectionFunction::__toString() with bound variables
--FILE--
<?php

$closure_without_bounds = fn () => 0;

$rf = new ReflectionFunction($closure_without_bounds);
echo (string) $rf;

$global = "";
$closure_with_bounds = function() use($global) {
    static $counter = 0;
    return $counter++;
};

$rf = new ReflectionFunction($closure_with_bounds);
echo (string) $rf;

?>
--EXPECTF--
Closure [ <user> function {closure:%s:%d} ] {
  @@ %sReflectionFunction__toString_bound_variables.php 3 - 3
}
Closure [ <user> function {closure:%s:%d} ] {
  @@ %sReflectionFunction__toString_bound_variables.php 9 - 12

  - Bound Variables [2] {
      Variable #0 [ $global ]
      Variable #1 [ $counter ]
  }
}
