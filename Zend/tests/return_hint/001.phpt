--TEST--
Basic return hints at compilation
--FILE--
<?php
function test1() : array {

}
/* this is bad ... */
?>
--EXPECT--
