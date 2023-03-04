--TEST--
setcookie() expiry time negative warning
--FILE--
<?php
setcookie('name', 'value', -1);
?>
--EXPECT--
Warning: setcookie(): "expires" option cannot be negative in %s on line %d
