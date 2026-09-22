--TEST--
GH-18262 003 (Assertion failure Zend/zend_vm_execute.h JIT)
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
    new B('str'), // slow deoptimization, create linked side trace
    new B(0),     // jump to side trace with fast deoptimization
];

set_error_handler(function ($_, $errstr) {
    throw new \Exception($errstr);
});

foreach ($tests as $obj) {
    try {
        var_dump($obj->b);
    } catch (Exception $e) {
        printf("%s: %s\n", $e::class, $e->getMessage());
    }
}
?>
--EXPECT--
int(1)
string(3) "str"
Exception: Undefined property: B::$b
