--TEST--
Lazy proxy with __get creating references and arithmetic
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

Warning: Undefined property: A::$x in %s on line %d

Fatal error: Uncaught Error: Cannot assign by reference to overloaded object in %s:%d
Stack trace:
#0 %s(%d): A->__get('x')
#1 %s(%d): A->__get('p')
#2 {main}
  thrown in %s on line %d


