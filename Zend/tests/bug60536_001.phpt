--TEST--
Bug #60536 (Traits Segfault)
--FILE--
<?php
trait T { private $x = 0; }
class X {
    use T;
}
class Y extends X {
      use T;
      function __construct() {
          return ++$this->x;
      }
}
class Z extends Y {
      function __construct() {
          return ++$this->x;
      }
}
$a = new Z();
$a->__construct();
echo "DONE";
?>
--EXPECTF--
Deprecated: Creation of dynamic property Z::$x is deprecated in %s on line %d

Warning: Undefined property: Z::$x in %s on line %d
DONE
