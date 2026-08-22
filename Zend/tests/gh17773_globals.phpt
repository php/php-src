--TEST--
GH-17773 (Cannot initialize array element with reference to $GLOBALS)
--FILE--
<?php
$array = [&$GLOBALS];
?>
--EXPECTF--
Fatal error: Cannot acquire reference to $GLOBALS in %s on line %d
