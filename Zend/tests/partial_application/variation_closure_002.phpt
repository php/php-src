--TEST--
PFA variation: Closure::__invoke()
--FILE--
<?php
$closure = function($a, $b) {
    var_dump($a, $b);
};

$function = $closure->__invoke(1, ?);

echo (string) new ReflectionFunction($function);

$function(10);
?>
--EXPECTF--
Closure [ <user> public method {closure:%s:%d} ] {
  @@ %svariation_closure_002.php 6 - 6

  - Bound Variables [1] {
      Variable #0 [ $a ]
  }

  - Parameters [1] {
    Parameter #0 [ <required> $b ]
  }
}
int(1)
int(10)
