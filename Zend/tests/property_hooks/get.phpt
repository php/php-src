--TEST--
Basic get only property hook
--FILE--
<?php

class Test {
    public $prop {
        get { return 42; }
    }
}

$test = new Test;
var_dump($test->prop);

try {
    $test->prop = 0;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
int(42)
Error: Cannot write to get-only virtual property Test::$prop
