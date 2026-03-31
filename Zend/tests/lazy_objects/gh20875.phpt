--TEST--
GH-20875 (Assertion failure in _get_zval_ptr_tmp with lazy proxy)
--FILE--
<?php
class A {
    public $_;
    public function __get($name) {
        global $obj;
        $obj->f =& $this->b - $x > $y = new StdClass;
        static $a = $a;
        $t = 'x';
        foreach (get_defined_vars() as $key => $e) {}
        if ($v ==!1) $x = $a ?: $t = "ok";
    }
}
$rc = new ReflectionClass(A::class);
$obj = $rc->newLazyProxy(function () { return new A; });
$real = $rc->initializeLazyObject($obj);
var_dump($real->prop);
?>
--EXPECTF--
Deprecated: Creation of dynamic property A::$b is deprecated in %s on line %d

Deprecated: Creation of dynamic property A::$f is deprecated in %s on line %d

Warning: Undefined variable $x in %s on line %d

Notice: Object of class stdClass could not be converted to int in %s on line %d

Warning: Undefined variable $a in %s on line %d

Warning: Undefined variable $v in %s on line %d

Notice: Indirect modification of overloaded property A::$b has no effect in %s on line %d

Warning: Undefined variable $x in %s on line %d

Notice: Object of class stdClass could not be converted to int in %s on line %d

Warning: Undefined variable $v in %s on line %d

Notice: Indirect modification of overloaded property A::$f has no effect in %s on line %d

Fatal error: Uncaught Error: Cannot assign by reference to overloaded object in %s:%d
Stack trace:
#0 %s(%d): A->__get('b')
#1 %s(%d): A->__get('prop')
#2 {main}
  thrown in %s on line %d
