--TEST--
Basic return hints at compilation
--FILE--
<?php
function test1() : array {

}
?>
--EXPECTF--
Fatal error: the function test1 was expected to return an array and returned nothing in %s on line %d
