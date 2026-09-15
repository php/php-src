--TEST--
GH-17773 (Cannot initialize array element with reference returned by built-in function)
--FILE--
<?php
$array = [&strlen('value')];
?>
--EXPECTF--
Fatal error: Cannot use result of built-in function in write context in %s on line %d
