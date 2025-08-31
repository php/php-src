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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop = "foo";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->prop;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Typed property Test::$prop must not be accessed before initialization
Cannot assign string to property Test::$prop of type int
Typed property Test::$prop must not be accessed before initialization
