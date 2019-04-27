--TEST--
Bug #69948 (path/domain are not sanitized for special characters in setcookie)
--FILE--
<?php
var_dump(
    setcookie('foo', 'bar', 0, 'asdf;asdf'),
    setcookie('foo', 'bar', 0, '/', 'foobar; secure')
);
?>
===DONE===
--EXPECTHEADERS--
--EXPECTF--
Warning: Cookie paths cannot contain any of the following ',; \t\r\n\013\014' in %s on line %d

Warning: Cookie domains cannot contain any of the following ',; \t\r\n\013\014' in %s on line %d
bool(false)
bool(false)
===DONE===
