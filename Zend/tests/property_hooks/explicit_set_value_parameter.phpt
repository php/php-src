--TEST--
Explicit set property hook $value parameter
--FILE--
<?php

class Test {
    public $prop {
        set($customName) {
            var_dump($customName);
        }
    }
}

$test = new Test();
$test->prop = 42;

?>
--EXPECT--
int(42)
