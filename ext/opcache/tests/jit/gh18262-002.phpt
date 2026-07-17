--TEST--
GH-18262 002 (Assertion failure Zend/zend_vm_execute.h JIT)
--FILE--
<?php
#[AllowDynamicProperties]
class B {
    public function __construct($init) {
        if ($init) {
            $this->b = $init;
        }
    }
}

$tests = [
    new B(1),
    new B(0),
];

set_error_handler(function ($_, $errstr) {
    throw new \Exception($errstr);
});

foreach ($tests as $obj) {
    var_dump($obj->b);
}
?>
--EXPECTF--
int(1)
NULL

Fatal error: Uncaught Exception: Undefined property: B::$b in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}(2, 'Undefined prope...', '%s', %d)
#1 %s(%d): var_dump(NULL)
#2 {main}
  thrown in %s on line %d
