--TEST--
zend_call_method_if_exists() with non existing method on class with a static trampoline
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    public function foo() {
        return __METHOD__;
    }
    public static function __callStatic(string $name, array $arguments): string {
        return "In static trampoline for $name!";
    }
}

$a = new A();

$r = zend_call_method_if_exists($a, 'bar');
var_dump($r);

?>
--EXPECT--
NULL
