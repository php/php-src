--TEST--
Bug #63336 (invalid E_NOTICE error occur)
--XFAIL--
Bug is not fixed yet
--FILE--
<?php
error_reporting(E_ALL | E_NOTICE );
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
--EXPECT--
Strict Standards: Declaration of Child::foo() should be compatible with Base::foo($var = '123', $more = NULL) in %sbug63336.php on line %d

Strict Standards: Declaration of Child::bar() should be compatible with Base::bar($var, $more = 'XXX') in %sbug63336.php on line %d
