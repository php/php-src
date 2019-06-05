--TEST--
Testing trait_exists()
--FILE--
<?php

trait foo {
}

var_dump(trait_exists('foo'));
var_dump(trait_exists(1));
var_dump(trait_exists(NULL));
var_dump(trait_exists(new stdClass));

?>
--EXPECTF--
bool(true)
bool(false)

Deprecated: Passing null to argument of type string is deprecated in %s on line %d
bool(false)

Warning: trait_exists() expects parameter 1 to be string, object given in %s on line %d
NULL
