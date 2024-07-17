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
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
int(42)
Property Test::$prop is read-only
