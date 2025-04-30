--TEST--
GH-18262 001 (Assertion failure Zend/zend_vm_execute.h JIT)
--CREDITS--
YuanchengJiang
--FILE--
<?php
#[AllowDynamicProperties]
class B {
    public int $fusion;
}
class C extends B {
}
class D extends C {
    public function __destruct() {
    }
}
$tests = [
    [C::class, new C()],
    [C::class, new B()],
    [D::class, new B()],
];
foreach ($tests as [$class, $instance]) {
    $obj = (new ReflectionClass($class))->newLazyProxy(function ($obj) use ($instance) {
        $instance->b = 1;
        return $instance;
    });
    var_dump($obj->b);
}
?>
--EXPECTF--
int(1)
int(1)

Fatal error: Uncaught TypeError: %s in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
