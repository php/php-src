--TEST--
Return by reference from handler
--FILE--
<?php

use extension string StringHandler;

class StringHandler {
    public static function &returnRef($self, &$var) {
        return $var;
    }
}

$string = "foo";
$var = 42;
$ref =& $string->returnRef($var);
$ref = 24;
var_dump($var);

?>
--EXPECT--
int(24)
