--TEST--
Computed property shorthand
--FILE--
<?php

class Test {
    public $prop => strtoupper(__PROPERTY__);
}

$test = new Test;
var_dump($test->prop);

?>
--EXPECT--
string(4) "PROP"
