--TEST--
zend_call_method_if_exists() shadowing a private method
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    private function foo() {
        return 'Not available';
    }
    public function __call(string $name, array $arguments): string {
        return "In trampoline for $name!";
    }
}

$a = new A();

$r = zend_call_method_if_exists($a, 'foo');
var_dump($r);

?>
--EXPECT--
string(22) "In trampoline for foo!"
