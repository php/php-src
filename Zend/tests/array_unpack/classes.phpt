--TEST--
Array unpacking with classes
--FILE--
<?php

class C {
    public const FOO = [0, ...self::ARR, 4];
    public const ARR = [1, 2, 3];
    public static $bar = [...self::ARR];
}

class D {
    public const A = [...self::B];
    public const B = [...self::A];
}

var_dump(C::FOO);
var_dump(C::$bar);

try {
    var_dump(D::A);
} catch (Error $ex) {
    echo "Exception: " . $ex->getMessage() . "\n";
}
?>
--EXPECT--
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
}
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
Exception: Cannot declare self-referencing constant self::B
