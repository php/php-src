--TEST--
Testing several valid and invalid parameters
--FILE--
<?php

class foo {

}

var_dump(class_exists(''));
var_dump(class_exists('FOO'));
var_dump(class_exists('bar'));
var_dump(class_exists(1));

?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(false)
