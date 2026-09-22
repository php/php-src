--TEST--
GH-20873 (Assertion failure in _zendi_try_convert_scalar_to_number with lazy proxy)
--FILE--
<?php
class A {
    public $_;
    public function __get($n) {
        global $obj;
        $obj->x =& $this->_;
        static $a = $a;
        $e =& $this->_ - $a;
    }
}
$rc = new ReflectionClass(A::class);
$obj = $rc->newLazyProxy(fn() => new A);
$rc->initializeLazyObject($obj);
var_dump($obj->p);
?>
--EXPECTF--
Deprecated: Creation of dynamic property A::$x is deprecated in %s on line %d

Warning: Undefined variable $a in %s on line %d

Notice: Indirect modification of overloaded property A::$x has no effect in %s on line %d

Fatal error: Uncaught Error: Cannot assign by reference to overloaded object in %s:%d
Stack trace:
#0 %s(%d): A->__get('p')
#1 {main}
  thrown in %s on line %d
