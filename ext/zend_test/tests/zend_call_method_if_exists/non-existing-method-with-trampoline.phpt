--TEST--
zend_call_method_if_exists() with non existing method on class with a trampoline
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    public function foo() {
        return __METHOD__;
    }
    public function __call(string $name, array $arguments): string {
        return "In trampoline for $name!";
    }
}

$a = new A();

$r = zend_call_method_if_exists($a, 'bar');
var_dump($r);

?>
--EXPECT--
string(22) "In trampoline for bar!"
