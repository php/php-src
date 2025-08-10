--TEST--
zend_call_method_if_exists() with non existing method on extended class with a trampoline
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    public function __call(string $name, array $arguments): string {
        return "In A trampoline for $name!";
    }
}

class B extends A {}

$b = new B();

$r = zend_call_method_if_exists($b, 'bar');
var_dump($r);

?>
--EXPECT--
string(24) "In A trampoline for bar!"
