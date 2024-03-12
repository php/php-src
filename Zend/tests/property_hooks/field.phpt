--TEST--
Special $field variable refers to property backing store
--FILE--
<?php

class Test {
    public $prop {
        get => $field;
        set => $field = $value;
    }
}

$test = new Test;
$test->prop = 42;
var_dump($test->prop);

?>
--EXPECT--
int(42)
