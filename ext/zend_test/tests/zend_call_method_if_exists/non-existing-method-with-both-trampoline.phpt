--TEST--
zend_call_method_if_exists() with non existing method on class with a trampoline and static trampoline
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    public function foo() {
        return __METHOD__;
    }
    public function __call(string $name, array $arguments): string {
        return "In A trampoline for $name!";
    }
    public static function __callStatic(string $name, array $arguments): string {
        return "In A static trampoline for $name!";
    }
}

class B {
    public function foo() {
        return __METHOD__;
    }
    public static function __callStatic(string $name, array $arguments): string {
        return "In B static trampoline for $name!";
    }
    public function __call(string $name, array $arguments): string {
        return "In B trampoline for $name!";
    }
}

$a = new A();
$r = zend_call_method_if_exists($a, 'bar');
var_dump($r);

$b = new B();
$r = zend_call_method_if_exists($b, 'bar');
var_dump($r);


?>
--EXPECT--
string(24) "In A trampoline for bar!"
string(24) "In B trampoline for bar!"
