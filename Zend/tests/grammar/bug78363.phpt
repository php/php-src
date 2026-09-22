--TEST--
Bug #78363: Buffer overflow in zendparse
--FILE--
<?php
<<<X
%0$a
 X;
?>
--EXPECTF--
Parse error: Invalid body indentation level (expecting an indentation level of at least 1) in %s on line %d
