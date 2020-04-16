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

      function another() {
          self::__construct();
      }
}
$a = new Z();
$a->another();
echo "DONE";
?>
--EXPECTF--
Warning: Undefined property: Z::$x in %s on line %d
DONE
