--TEST--
Lazy proxy with __get creating references and arithmetic
--FILE--
<?php
class A {
    public $_;

    public function __get($n) {
        global $obj;

        // Create a reference to an uninitialized property
        $obj->x =& $this->_;

        // Static self-reference (edge case)
        static $a = $a;

        // Arithmetic on reference
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
#0 %s(9): A->__get('x')
#1 %s(23): A->__get('p')
#2 {main}
  thrown in %s on line %d


