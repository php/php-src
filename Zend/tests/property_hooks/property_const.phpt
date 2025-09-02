--TEST--
__PROPERTY__ magic constant
--FILE--
<?php

class Test {
    public $prop {
        get => __PROPERTY__;
        set { var_dump(__PROPERTY__); }
    }

    private $privProp {
        get => __PROPERTY__;
    }

    public function test() {
        var_dump(__PROPERTY__);
        var_dump($this->privProp);
    }
}

$test = new Test;
var_dump($test->prop);
$test->prop = 'foo';
$test->test();
var_dump(__PROPERTY__);

?>
--EXPECT--
string(4) "prop"
string(4) "prop"
string(0) ""
string(8) "privProp"
string(0) ""
