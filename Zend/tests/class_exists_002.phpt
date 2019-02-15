--TEST--
Testing several valid and invalid parameters
--FILE--
<?php

class foo {

}

var_dump(class_exists(''));
var_dump(class_exists(NULL));
var_dump(class_exists('FOO'));
var_dump(class_exists('bar'));
var_dump(class_exists(1));
var_dump(class_exists(new stdClass));

?>
--EXPECTF--
bool(false)
bool(false)
bool(true)
bool(false)
bool(false)

Warning: class_exists() expects parameter 1 to be string, object given in %s on line %d
NULL
