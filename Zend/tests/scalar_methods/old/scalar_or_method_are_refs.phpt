--TEST--
Scalar or called method can be referenced variables
--FILE--
<?php

use extension string StringHandler;

class StringHandler {
    public static function test($self) {
        echo "Called\n";
    }
}

$str = "foo";
$method = "test";
$ref1 =& $str;
$ref2 =& $method;
$str->$method();

?>
--EXPECT--
Called
