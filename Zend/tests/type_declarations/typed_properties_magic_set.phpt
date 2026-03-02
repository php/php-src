--TEST--
__set() should not be invoked when setting an uninitialized typed property
--FILE--
<?php

class Test {
    public int $foo;
    public function __get($name) {
        echo "__get ", $name, "\n";
        return null;
    }
    public function __set($name, $value) {
        echo "__set ", $name, " = ", $value, "\n";
    }
    public function __isset($name) {
        echo "__isset ", $name, "\n";
        return true;
    }
    public function __unset($name) {
        echo "__unset ", $name, "\n";
    }
}

$test = new Test;
try {
    var_dump($test->foo);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(isset($test->foo));
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
Typed property Test::$foo must not be accessed before initialization
bool(false)
int(42)
__set foo = 42
__set foo = 42
int(42)
__set foo = 42
__set foo = 42
int(42)
__set foo = 42
