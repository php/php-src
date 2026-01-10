--TEST--
__PROPERTY__ magic constant in nested function
--FILE--
<?php

class Test {
    public $prop {
        get => (function () {
            return __PROPERTY__;
        })();
    }
}

$test = new Test;
var_dump($test->prop);

?>
--EXPECT--
string(0) ""
