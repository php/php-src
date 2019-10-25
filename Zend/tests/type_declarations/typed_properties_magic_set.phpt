--TEST--
__set() should not be invoked when setting an uninitialized typed property
--FILE--
<?php

class Test {
    public int $foo;
    public function __set($name, $value) {
        echo "__set ", $name, " = ", $value, "\n";
    }
}

$test = new Test;
$test->foo = 42;
var_dump($test->foo);

// __set will be called after unset()
unset($test->foo);
$test->foo = 42;

// __set will be called after unset() without prior initialization
$test = new Test;
unset($test->foo);
$test->foo = 42;

class Test2 extends Test {
}

// Check that inherited properties work correctly
$test = new Test;
$test->foo = 42;
var_dump($test->foo);
unset($test->foo);
$test->foo = 42;

// Test that cloning works correctly
$test = clone $test;
$test->foo = 42;
$test = clone new Test;
$test->foo = 42;
var_dump($test->foo);
unset($test->foo);
$test->foo = 42;

?>
--EXPECT--
int(42)
__set foo = 42
__set foo = 42
int(42)
__set foo = 42
__set foo = 42
int(42)
__set foo = 42
