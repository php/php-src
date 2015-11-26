--TEST--
Calling get_called_class() outside a class
--FILE--
<?php

var_dump(get_called_class());

?>
--EXPECTF--
Warning: get_called_class() called from outside a class in %s on line %d
bool(false)
