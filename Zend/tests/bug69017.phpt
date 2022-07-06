--TEST--
#69017 (Fail to push to the empty array with the constant value defined in class scope)
--FILE--
<?php

class c1
{
    const ZERO = 0;
    const ONE = 1;
    const MAX = PHP_INT_MAX;
    public static $a1 = array(self::ONE => 'one');
    public static $a2 = array(self::ZERO => 'zero');
    public static $a3 = array(self::MAX => 'zero');
}


c1::$a1[] = 1;
c1::$a2[] = 1;
try {
    c1::$a3[] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

var_dump(c1::$a1);
var_dump(c1::$a2);
var_dump(c1::$a3);
?>
--EXPECTF--
Cannot add element to the array as the next element is already occupied
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  int(1)
}
array(2) {
  [0]=>
  string(4) "zero"
  [1]=>
  int(1)
}
array(1) {
  [%d]=>
  string(4) "zero"
}
