--TEST--
Partial application variation closure
--FILE--
<?php
$closure = function($a, $b) {

};

echo (string) new ReflectionFunction($closure(1, ?));
?>
--EXPECTF--
Function [ <user> partial function {closure} ] {
  @@ %s 6 - 6

  - Parameters [1] {
    Parameter #0 [ <required> $b ]
  }
}

