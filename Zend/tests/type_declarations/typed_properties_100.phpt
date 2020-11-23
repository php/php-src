--TEST--
Property type not enforced for __get if the property is not visible
--FILE--
<?php

class Test {
    private int $prop;

    public function __get($name) {
        return "foobar";
    }
}

$test = new Test;
var_dump($test->prop);

?>
--EXPECT--
string(6) "foobar"
