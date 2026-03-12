--TEST--
GH-20564: Don't call autoloaders with pending exception
--CREDITS--
Viet Hoang Luu (@vi3tL0u1s)
--FILE--
<?php

class A {
    function __call($method, $args) {
        eval("<<<ENDOFSTRING\n Test\n ENDOFSTRING;");
        spl_autoload_register('A::test');
        array_map('B::test', []);
    }
}

try {
    (new A)->test();
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
array_map(): Argument #1 ($callback) must be a valid callback or null, class "B" not found
