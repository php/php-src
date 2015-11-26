--TEST--
Bug #63336 (invalid E_NOTICE error occur)
--FILE--
<?php
error_reporting(E_ALL & ~E_WARNING);
define("TEST", "123");
class Base {
    const DUMMY = "XXX";
    public function foo($var=TEST, $more=null) { return true; }
    public function bar($more=self::DUMMY) { return true; }
}

class Child extends Base {
    const DUMMY = "DDD";
    public function foo($var=TEST, array $more = array()) { return true; }
    public function bar($var, $more=self::DUMMY) { return true; }
}
?>
--EXPECTF--
Warning: Declaration of Child::foo($var = TEST, array $more = Array) should be compatible with Base::foo($var = TEST, $more = NULL) in %sbug63336.php on line %d

Warning: Declaration of Child::bar($var, $more = self::DUMMY) should be compatible with Base::bar($more = self::DUMMY) in %sbug63336.php on line %d
