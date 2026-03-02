--TEST--
Bug #78810: RW fetches do not throw "uninitialized property" exception
--FILE--
<?php

class Test {
    public int $i;
}

$test = new Test;
try {
    $test->i++;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $test->i += 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Typed property Test::$i must not be accessed before initialization
Typed property Test::$i must not be accessed before initialization
