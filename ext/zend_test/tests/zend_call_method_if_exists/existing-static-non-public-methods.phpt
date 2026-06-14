--TEST--
zend_call_method_if_exists() with existing non public static methods
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    protected static function foo() {
        return __METHOD__;
    }
    private static function bar() {
        return __METHOD__;
    }
}

$a = new A();

$r = zend_call_method_if_exists($a, 'foo');
var_dump($r);

$r = zend_call_method_if_exists($a, 'bar');
var_dump($r);
?>
--EXPECT--
NULL
NULL
