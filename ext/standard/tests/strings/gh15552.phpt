--TEST--
Bug GH-15552 (Signed integer overflow in ext/standard/scanf.c)
--FILE--
<?php
var_dump(sscanf('hello','%2147483648$s'));
?>
--EXPECTF--
Fatal error: Uncaught ValueError: "%n$" argument index out of range in %s:%d
Stack trace:%A
