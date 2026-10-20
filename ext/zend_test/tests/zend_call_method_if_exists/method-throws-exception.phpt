--TEST--
zend_call_method_if_exists() with throwing method
--EXTENSIONS--
zend_test
--FILE--
<?php

class A {
    public function foo() {
        throw new Exception("Error");
    }
}

$a = new A();

try {
    $r = zend_call_method_if_exists($a, 'foo');
    var_dump($r);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Exception: Error
