--TEST--
Magic constants in property hooks
--FILE--
<?php

class Test {
    public $prop {
        get {
            var_dump(__FUNCTION__);
            var_dump(__METHOD__);
            var_dump(__CLASS__);
            return null;
        }
    }
}

$test = new Test;
$test->prop;

?>
--EXPECT--
string(10) "$prop::get"
string(16) "Test::$prop::get"
string(4) "Test"
