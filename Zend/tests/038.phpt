--TEST--
Trying to use lambda as array key
--FILE--
<?php

var_dump(array(function() { } => 1));

?>
--EXPECTF--
Warning: Illegal offset type in %s on line %d
array(0) {
}
