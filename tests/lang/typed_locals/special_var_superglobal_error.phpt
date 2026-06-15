--TEST--
Typed local variables: typing a superglobal is a compile error
--FILE--
<?php
string $_GET = 1;
?>
--EXPECTF--
Fatal error: Cannot declare a type for the special variable $_GET in %s on line %d
