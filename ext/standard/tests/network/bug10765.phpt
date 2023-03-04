--TEST--
setcookie() expiry time negative warning
--FILE--
<?php
setcookie('name', 'value', -1);
?>
--EXPECTF--
Warning: setcookie(): "expires" option cannot be negative in %s on line %d

Warning: Cannot modify header information - headers already sent by (output started at %s:%d) in %s on line %d
