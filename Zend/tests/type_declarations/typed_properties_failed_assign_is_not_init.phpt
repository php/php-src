--TEST--
A failed assignment should not be considered an initialization
--FILE--
<?php

class Test {
    public int $prop;

    public function __get($name) {
        echo "__get() called\n";
        return 0;
    }
}

$test = new Test;
try {
    $test->prop;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $test->prop = "foo";
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $test->prop;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Typed property Test::$prop must not be accessed before initialization
TypeError: Cannot assign string to property Test::$prop of type int
Error: Typed property Test::$prop must not be accessed before initialization
