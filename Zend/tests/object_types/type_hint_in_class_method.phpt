--TEST--
Adding a class method object type hint
--FILE--
<?php

class One {
    public function a(object $obj) {}
}

$one = new One();
$one->a(new One());
$one->a(123);
?>
--EXPECTF--
Fatal error: Uncaught TypeError: One::a(): Argument #1 ($obj) must be of type object, int given, called in %s:%d
Stack trace:
#0 %s(9): One->a(123)
#1 {main}
  thrown in %s on line 4
