--TEST--
__PROPERTY__ magic constant
--FILE--
<?php

class Test {
    public $prop {
        get => __PROPERTY__;
        set => var_dump(__PROPERTY__);
    }

    public function method() {
        return __PROPERTY__;
    }
}

$test = new Test;
var_dump($test->prop);
$test->prop = 'foo';
var_dump($test->method());
var_dump(__PROPERTY__);

?>
--EXPECT--
string(4) "prop"
string(4) "prop"
string(0) ""
string(0) ""
