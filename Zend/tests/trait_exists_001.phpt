--TEST--
Testing trait_exists()
--FILE--
<?php

trait foo {
}

var_dump(trait_exists('foo'));
var_dump(trait_exists(1));
var_dump(trait_exists(NULL));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
