--TEST--
Get property hook must respect property type
--FILE--
<?php

class Test {
    public int $prop1 {
        get { return "foobar"; }
    }
    public int $prop2 {
        get { return "42"; }
    }
}

$test = new Test;
try {
    var_dump($test->prop1);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test->prop2);

?>
--EXPECT--
Test::$prop1::get(): Return value must be of type int, string returned
int(42)
