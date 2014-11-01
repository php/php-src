--TEST--
Using array_keys() on $GLOBALS
--FILE--
<?php

$foo = 'bar';
unset($foo);
var_dump(in_array('foo', array_keys($GLOBALS)));

?>
--EXPECT--
bool(false)
