--TEST--
setcookie() allows empty cookie name
--FILE--
<?php
setcookie('', 'foo');
?>
--EXPECTF--
Warning: Cookie names must not be empty in %s on line %d
